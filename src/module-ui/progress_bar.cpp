// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/progress_bar.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-utils/include/math.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Progress_bar::Progress_bar(std::string const& name) : Widget(name) {}
void Progress_bar::set_value(float v){_value=clamp01(v);if(!_animated)_display=_value;}
float Progress_bar::value() const { return _value; }
void Progress_bar::set_animated(bool on,float s){_animated=on;_speed=s;if(!on)_display=_value;}
void Progress_bar::set_orientation(Orientation o){_orient=o;}
void Progress_bar::set_show_label(bool s){_label=s;}
void Progress_bar::on_update(float dt){_display=_animated?lerp(_display,_value,clamp01(dt*_speed)):_value;}
void Progress_bar::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    auto& rm=resource_manager();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    Theme const& th=ui_manager().theme();
    Visual const& fill = th.has("progress_fill")?th.get("progress_fill").visual_for(Widget_state::NORMAL):resolved_visual();
    sf::FloatRect fr;
    if(_orient==Orientation::HORIZONTAL) fr=sf::FloatRect(r.position,{r.size.x*_display,r.size.y});
    else fr=sf::FloatRect({r.position.x,r.position.y+r.size.y*(1.f-_display)},{r.size.x,r.size.y*_display});
    visual_renderer::draw(fill,fr,renderer,draw_buffer(),rm,layer);
    if(_label){
        int pct=(int)(_display*100.f);
        draw_text(renderer,r,std::to_string(pct)+"%",Text_align::CENTER);
    }
}
} // namespace titan::ui
