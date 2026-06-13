// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/minimap.hpp"

#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Minimap::Minimap(std::string const& name):Widget(name){ enable_click_events(); }
void Minimap::set_world_bounds(sf::Vector2f tl,sf::Vector2f s){_wtl=tl;_wsize=s;}
void Minimap::clear_blips(){_blips.clear();}
void Minimap::add_blip(sf::Vector2f w,sf::Color c,float r){_blips.push_back({w,c,r});}
void Minimap::set_viewport(sf::Vector2f tl,sf::Vector2f s){_has_vp=true;_vp_tl=tl;_vp_size=s;}
void Minimap::set_on_clicked(std::function<void(sf::Vector2f)> cb){_on_clicked=std::move(cb);}
sf::Vector2f Minimap::_world_to_local(sf::Vector2f w) const {
    sf::FloatRect r=rect();
    // normalized world fraction via inverse_lerp, then map into the rect
    float fx=clamp01(inverse_lerp(_wtl.x,_wtl.x+_wsize.x,w.x));
    float fy=clamp01(inverse_lerp(_wtl.y,_wtl.y+_wsize.y,w.y));
    return { lerp(r.position.x,r.position.x+r.size.x,fx),
             lerp(r.position.y,r.position.y+r.size.y,fy) };
}
void Minimap::on_clicked(){
    if(!_on_clicked) return;
    sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
    // screen fraction -> world via remap
    float fx=clamp01(inverse_lerp(r.position.x,r.position.x+r.size.x,m.x));
    float fy=clamp01(inverse_lerp(r.position.y,r.position.y+r.size.y,m.y));
    _on_clicked({ lerp(_wtl.x,_wtl.x+_wsize.x,fx),
                  lerp(_wtl.y,_wtl.y+_wsize.y,fy) });
}
void Minimap::on_render(render::Renderer& renderer){
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    if(_has_vp){
        sf::Vector2f a=_world_to_local(_vp_tl), b=_world_to_local(_vp_tl+_vp_size);
        sf::RectangleShape vp; vp.setPosition(a); vp.setSize({b.x-a.x,b.y-a.y});
        vp.setFillColor(sf::Color(255,255,255,30)); vp.setOutlineColor(sf::Color(255,255,255,160)); vp.setOutlineThickness(-1.f);
        draw_buffer().add_rect(renderer,layer,std::move(vp));
    }
    for(auto const& b:_blips){
        sf::Vector2f p=_world_to_local(b.world);
        sf::CircleShape dot(b.radius); dot.setOrigin({b.radius,b.radius}); dot.setPosition(p); dot.setFillColor(b.color);
        draw_buffer().add_circle(renderer,layer,std::move(dot));
    }
}
} // namespace titan::ui
