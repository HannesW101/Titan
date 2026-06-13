// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/scroll_view.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-utils/include/math.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Scroll_view::Scroll_view(std::string const& name):Widget(name){
    auto& sfml=events::SFML_event_manager::instance();
    _up=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_UP,
        [this](events::SFML_event_data const&){ if(!is_hovered())return; if(_dir==Dir::HORIZONTAL)_target.x-=_speed; else _target.y-=_speed; }, listener_id());
    _down=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_DOWN,
        [this](events::SFML_event_data const&){ if(!is_hovered())return; if(_dir==Dir::HORIZONTAL)_target.x+=_speed; else _target.y+=_speed; }, listener_id());
}
void Scroll_view::set_dir(Dir d){_dir=d;}
void Scroll_view::set_speed(float s){_speed=s;}
void Scroll_view::set_content_size(sf::Vector2f s){_content=s;}
void Scroll_view::scroll_to(sf::Vector2f o){_target=o;}
sf::Vector2f Scroll_view::scroll_offset() const { return _off; }
void Scroll_view::on_layout(){
    sf::FloatRect in=inner_rect();
    float mx=std::max(0.f,_content.x-in.size.x), my=std::max(0.f,_content.y-in.size.y);
    _target.x=clamp(_target.x,0.f,mx); _target.y=clamp(_target.y,0.f,my);
    for(auto const& c:children()){
        sf::FloatRect s({in.position.x-_off.x,in.position.y-_off.y},in.size);
        c->do_layout(s);
    }
}
void Scroll_view::on_update(float dt){ _off.x=lerp(_off.x,_target.x,clamp01(dt*15.f)); _off.y=lerp(_off.y,_target.y,clamp01(dt*15.f)); }
void Scroll_view::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    Theme const& th=ui_manager().theme();
    Visual thumb = th.has("scroll_thumb")? th.get("scroll_thumb").visual_for(Widget_state::NORMAL):resolved_visual();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    if(_content.y>r.size.y && _dir!=Dir::HORIZONTAL){
        float bw=6.f, th_h=std::max(20.f,r.size.y*(r.size.y/_content.y));
        float mo=_content.y-r.size.y, t=(mo>0.f)?_off.y/mo:0.f, ty=r.position.y+t*(r.size.y-th_h);
        visual_renderer::draw(thumb,sf::FloatRect({r.position.x+r.size.x-bw-2.f,ty},{bw,th_h}),renderer,draw_buffer(),resource_manager(),layer);
    }
    if(_content.x>r.size.x && _dir!=Dir::VERTICAL){
        float bh=6.f, th_w=std::max(20.f,r.size.x*(r.size.x/_content.x));
        float mo=_content.x-r.size.x, t=(mo>0.f)?_off.x/mo:0.f, tx=r.position.x+t*(r.size.x-th_w);
        visual_renderer::draw(thumb,sf::FloatRect({tx,r.position.y+r.size.y-bh-2.f},{th_w,bh}),renderer,draw_buffer(),resource_manager(),layer);
    }
}
} // namespace titan::ui
