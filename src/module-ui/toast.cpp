// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/toast.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Toast::Toast(std::string const& name):Widget(name){ set_on_overlay(true); set_visible(false); }
void Toast::set_text(std::string const& t){_text=t;}
void Toast::show(float d){ _duration=d; _time=0.f; _op=0.f; set_visible(true); }
void Toast::on_update(float dt){
    if(!is_visible()) return;
    _time+=dt;
    float fade_in=0.2f, fade_out=0.4f;
    if(_time<fade_in) _op=_time/fade_in;
    else if(_time>_duration-fade_out) _op=clamp01((_duration-_time)/fade_out);
    else _op=1.f;
    if(_time>=_duration) set_visible(false);
}
void Toast::on_render(render::Renderer& renderer){
    if(_op<0.01f||_text.empty()) return;
    Visual v=resolved_visual();
    if(v.kind==Visual::Kind::COLOR){ v.fill_color.a=(std::uint8_t)(v.fill_color.a*_op); v.outline_color.a=(std::uint8_t)(v.outline_color.a*_op); }
    visual_renderer::draw(v,rect(),renderer,draw_buffer(),resource_manager(),render::Render_layer::UI_OVERLAY);
    auto const& ta=resolved_text_style();
    sf::Color c=ta.color; c.a=(std::uint8_t)(c.a*_op);
    Text_appearance ta2=ta; ta2.color=c;
    set_text_style(Widget_state::NORMAL,ta2);
    draw_text(renderer,rect(),_text,Text_align::CENTER);
}
} // namespace titan::ui
