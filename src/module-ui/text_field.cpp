// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/text_field.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Window/Event.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Text_field::Text_field(std::string const& name) : Widget(name) {
    enable_click_events(); set_focusable(true);
    auto& sfml = events::SFML_event_manager::instance();
    _text_cb = sfml.register_callback(events::SFML_event_type::WINDOW_TEXT_ENTERED,
        [this](events::SFML_event_data const& d){
            if(!is_focused()||!is_enabled()) return;
            auto const* te=d.sfml_event.getIf<sf::Event::TextEntered>();
            if(!te) return;
            std::uint32_t ch=te->unicode;
            if(ch<32u||ch==127u) return;
            if(_text.size()>=_max) return;
            _text+=static_cast<char>(ch);
            if(_on_changed)_on_changed(_text);
        }, listener_id());
    _enter_cb = sfml.register_callback(events::SFML_event_type::KEYPRESS_RETURN,
        [this](events::SFML_event_data const&){ if(is_focused()&&_on_submit)_on_submit(_text); }, listener_id());
    _bs_cb = sfml.register_callback(events::SFML_event_type::KEYPRESS_BACKSPACE,
        [this](events::SFML_event_data const&){ if(is_focused()&&!_text.empty()){_text.pop_back(); if(_on_changed)_on_changed(_text);} }, listener_id());
}
void Text_field::set_text(std::string const& t){_text=t;}
std::string const& Text_field::text() const { return _text; }
void Text_field::set_placeholder(std::string const& p){_placeholder=p;}
void Text_field::set_max_length(std::size_t n){_max=n;}
void Text_field::set_password(bool on){_password=on;}
void Text_field::set_on_submit(std::function<void(std::string const&)> cb){_on_submit=std::move(cb);}
void Text_field::set_on_changed(std::function<void(std::string const&)> cb){_on_changed=std::move(cb);}
void Text_field::on_pressed(){ if(is_hovered()) request_focus(); else if(is_focused()) ui_manager().set_focused(nullptr); }
void Text_field::on_update(float dt){ _blink+=dt; if(_blink>1.f)_blink-=1.f; }
void Text_field::on_render(render::Renderer& renderer){
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    sf::FloatRect r=rect();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    std::string disp; sf::Color col=ta.color;
    if(_text.empty()){ disp=_placeholder; col.a=(std::uint8_t)(col.a/2u); }
    else if(_password){ disp=std::string(_text.size(),'*'); }
    else { disp=_text; }
    float const pad=8.f;
    if(!disp.empty()){
        sf::Text t(*font,disp,static_cast<unsigned int>(ta.size));
        t.setFillColor(col);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({r.position.x+pad, r.position.y+(r.size.y-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
    if(is_focused() && _blink<0.5f){
        float cx=r.position.x+pad;
        if(!_text.empty()){ sf::Text m(*font,_password?std::string(_text.size(),'*'):_text,static_cast<unsigned int>(ta.size)); cx+=m.getLocalBounds().size.x; }
        sf::RectangleShape cur; cur.setPosition({cx,r.position.y+4.f}); cur.setSize({1.5f,r.size.y-8.f}); cur.setFillColor(ta.color);
        draw_buffer().add_rect(renderer,layer,std::move(cur));
    }
}
} // namespace titan::ui
