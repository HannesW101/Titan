// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/text_area.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Window/Event.hpp"

#include <sstream>
#include <vector>
#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Text_area::Text_area(std::string const& name):Widget(name){
    enable_click_events(); set_focusable(true);
    auto& sfml=events::SFML_event_manager::instance();
    _text_cb=sfml.register_callback(events::SFML_event_type::WINDOW_TEXT_ENTERED,
        [this](events::SFML_event_data const& d){
            if(!is_focused()||!is_enabled()) return;
            auto const* te=d.sfml_event.getIf<sf::Event::TextEntered>();
            if(!te) { return; }
            std::uint32_t ch=te->unicode;
            if(ch<32u||ch==127u) { return; }
            _text+=static_cast<char>(ch);
            if(_on_changed)_on_changed(_text);
        }, listener_id());
    _enter_cb=sfml.register_callback(events::SFML_event_type::KEYPRESS_RETURN,
        [this](events::SFML_event_data const&){ if(is_focused()){ _text+='\n'; if(_on_changed)_on_changed(_text);} }, listener_id());
    _bs_cb=sfml.register_callback(events::SFML_event_type::KEYPRESS_BACKSPACE,
        [this](events::SFML_event_data const&){ if(is_focused()&&!_text.empty()){_text.pop_back(); if(_on_changed)_on_changed(_text);} }, listener_id());
    _up=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_UP,
        [this](events::SFML_event_data const&){ if(is_hovered()) _scroll=std::max(0.f,_scroll-18.f); }, listener_id());
    _dn=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_DOWN,
        [this](events::SFML_event_data const&){ if(is_hovered()) _scroll+=18.f; }, listener_id());
}
void Text_area::set_text(std::string const& t){_text=t;}
std::string const& Text_area::text() const { return _text; }
void Text_area::set_placeholder(std::string const& p){_placeholder=p;}
void Text_area::set_on_changed(std::function<void(std::string const&)> cb){_on_changed=std::move(cb);}
void Text_area::on_pressed(){ if(is_hovered()) request_focus(); else if(is_focused()) ui_manager().set_focused(nullptr); }
void Text_area::on_update(float dt){ _blink+=dt; if(_blink>1.f)_blink-=1.f; }
void Text_area::on_render(render::Renderer& renderer){
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    sf::FloatRect r=rect();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    unsigned int cs=static_cast<unsigned int>(ta.size);
    float lh=ta.size*1.3f;
    std::string src = _text.empty()? _placeholder : _text;
    sf::Color col=ta.color; if(_text.empty()) col.a=(std::uint8_t)(col.a/2u);
    // wrap on width and explicit newlines
    std::vector<std::string> lines;
    std::istringstream iss(src); std::string para;
    while(std::getline(iss,para)){
        std::istringstream w(para); std::string word; std::string line;
        while(w>>word){
            std::string test=line.empty()?word:line+" "+word;
            sf::Text probe(*font,test,cs);
            if(probe.getLocalBounds().size.x>r.size.x-12.f && !line.empty()){ lines.push_back(line); line=word; }
            else { line=test; }
        }
        lines.push_back(line);
    }
    float y=r.position.y+4.f-_scroll;
    for(auto const& ln:lines){
        if(y+lh>=r.position.y && y<=r.position.y+r.size.y){
            sf::Text t(*font,ln,cs); t.setFillColor(col);
            t.setPosition({r.position.x+6.f,y});
            draw_buffer().add_text(renderer,layer,std::move(t));
        }
        y+=lh;
    }
}
} // namespace titan::ui
