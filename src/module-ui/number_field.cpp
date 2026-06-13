// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/number_field.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Window/Event.hpp"

#include <iomanip>
#include <sstream>
#include <cstdlib>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Number_field::Number_field(std::string const& name):Widget(name){
    enable_click_events(); set_focusable(true);
    auto& sfml=events::SFML_event_manager::instance();
    _text=sfml.register_callback(events::SFML_event_type::WINDOW_TEXT_ENTERED,
        [this](events::SFML_event_data const& d){
            if(!is_focused()) return;
            auto const* te=d.sfml_event.getIf<sf::Event::TextEntered>(); if(!te) return;
            std::uint32_t ch=te->unicode;
            if((ch>='0'&&ch<='9')||ch=='-'||ch=='.') _buf+=static_cast<char>(ch);
        }, listener_id());
    _enter=sfml.register_callback(events::SFML_event_type::KEYPRESS_RETURN,
        [this](events::SFML_event_data const&){ if(is_focused()) _commit(); }, listener_id());
    _bs=sfml.register_callback(events::SFML_event_type::KEYPRESS_BACKSPACE,
        [this](events::SFML_event_data const&){ if(is_focused()&&!_buf.empty()) _buf.pop_back(); }, listener_id());
}
void Number_field::set_range(double a,double b){_min=a;_max=b;_val=clamp(_val,_min,_max);}
void Number_field::set_value(double v){_val=clamp(v,_min,_max);_buf=_fmt();}
double Number_field::value() const { return _val; }
void Number_field::set_decimals(int d){_decimals=d;_buf=_fmt();}
void Number_field::set_on_changed(std::function<void(double)> cb){_on_changed=std::move(cb);}
std::string Number_field::_fmt() const { std::ostringstream o; o<<std::fixed<<std::setprecision(_decimals)<<_val; return o.str(); }
void Number_field::_commit(){
    try{ double v=std::strtod(_buf.c_str(),nullptr); _val=clamp(v,_min,_max); }catch(...){}
    _buf=_fmt(); if(_on_changed)_on_changed(_val);
}
void Number_field::on_pressed(){
    if(is_hovered()){ if(!is_focused()){ _buf=_fmt(); } request_focus(); }
    else if(is_focused()){ _commit(); ui_manager().set_focused(nullptr); }
}
void Number_field::on_update(float dt){ _blink+=dt; if(_blink>1.f)_blink-=1.f; }
void Number_field::on_render(render::Renderer& renderer){
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    sf::FloatRect r=rect();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    std::string disp = is_focused()? _buf : _fmt();
    sf::Text t(*font,disp,static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
    sf::FloatRect tb=t.getLocalBounds();
    t.setPosition({r.position.x+8.f,r.position.y+(r.size.y-tb.size.y)*0.5f-tb.position.y});
    draw_buffer().add_text(renderer,layer,std::move(t));
    if(is_focused()&&_blink<0.5f){
        float cx=r.position.x+8.f+tb.size.x+2.f;
        sf::RectangleShape cur; cur.setPosition({cx,r.position.y+4.f}); cur.setSize({1.5f,r.size.y-8.f}); cur.setFillColor(ta.color);
        draw_buffer().add_rect(renderer,layer,std::move(cur));
    }
}
} // namespace titan::ui
