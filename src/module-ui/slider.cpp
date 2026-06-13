// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/slider.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/CircleShape.hpp"

#include <cmath>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Slider::Slider(std::string const& name) : Widget(name) { enable_click_events(); set_focusable(true); }
void Slider::set_range(float a,float b){_min=a;_max=b;_value=clamp(_value,_min,_max);}
void Slider::set_value(float v){_value=clamp(v,_min,_max);}
float Slider::value() const { return _value; }
void Slider::set_step(float s){_step=s;}
void Slider::set_orientation(Orientation o){_orient=o;}
void Slider::set_on_changed(std::function<void(float)> cb){_on_changed=std::move(cb);}
void Slider::on_pressed(){_drag=true;}
void Slider::on_released(){_drag=false;}
void Slider::on_update(float){
    if(!_drag) return;
    sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
    float t = _orient==Orientation::HORIZONTAL ? clamp01((m.x-r.position.x)/r.size.x)
                                               : clamp01((m.y-r.position.y)/r.size.y);
    float nv=lerp(_min,_max,t);
    if(_step>0.f){nv=_min+std::round((nv-_min)/_step)*_step;nv=clamp(nv,_min,_max);}
    if(nv!=_value){_value=nv;if(_on_changed)_on_changed(_value);}
}
void Slider::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    auto& rm=resource_manager();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    float t=(_max>_min)?(_value-_min)/(_max-_min):0.f;
    Theme const& th=ui_manager().theme();
    Visual const& track = th.has("slider_track")? th.get("slider_track").visual_for(Widget_state::NORMAL):resolved_visual();
    Visual const& fill  = th.has("slider_fill") ? th.get("slider_fill").visual_for(Widget_state::NORMAL):track;
    Visual const& knobv = th.has("slider_knob") ? th.get("slider_knob").visual_for(current_state()):track;
    if(_orient==Orientation::HORIZONTAL){
        float th_h=r.size.y*0.3f;
        sf::FloatRect tr({r.position.x,r.position.y+(r.size.y-th_h)*0.5f},{r.size.x,th_h});
        visual_renderer::draw(track,tr,renderer,draw_buffer(),rm,layer);
        sf::FloatRect fr({r.position.x,r.position.y+(r.size.y-th_h)*0.5f},{r.size.x*t,th_h});
        visual_renderer::draw(fill,fr,renderer,draw_buffer(),rm,layer);
        float kr=r.size.y*0.5f;
        sf::FloatRect kk({r.position.x+r.size.x*t-kr,r.position.y},{kr*2.f,kr*2.f});
        visual_renderer::draw(knobv,kk,renderer,draw_buffer(),rm,layer);
    } else {
        float th_w=r.size.x*0.3f;
        sf::FloatRect tr({r.position.x+(r.size.x-th_w)*0.5f,r.position.y},{th_w,r.size.y});
        visual_renderer::draw(track,tr,renderer,draw_buffer(),rm,layer);
        sf::FloatRect fr({r.position.x+(r.size.x-th_w)*0.5f,r.position.y},{th_w,r.size.y*t});
        visual_renderer::draw(fill,fr,renderer,draw_buffer(),rm,layer);
        float kr=r.size.x*0.5f;
        sf::FloatRect kk({r.position.x,r.position.y+r.size.y*t-kr},{kr*2.f,kr*2.f});
        visual_renderer::draw(knobv,kk,renderer,draw_buffer(),rm,layer);
    }
}
} // namespace titan::ui
