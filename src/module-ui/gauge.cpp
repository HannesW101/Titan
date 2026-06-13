// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/gauge.hpp"

#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/VertexArray.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Gauge::Gauge(std::string const& name):Widget(name){}
void Gauge::set_value(float v){_v=clamp01(v);}
float Gauge::value() const { return _v; }
void Gauge::set_angles(float s,float sw){_start=s;_sweep=sw;}
void Gauge::set_thickness(float t){_thick=t;}
void Gauge::set_colors(sf::Color t,sf::Color f){_track=t;_fill=f;}
void Gauge::set_show_text(bool on){_text=on;}
static void build_arc(sf::VertexArray& va,sf::Vector2f c,float rin,float rout,float a0_deg,float a1_deg,sf::Color col){
    int seg=std::max(2,(int)(std::abs(a1_deg-a0_deg)/4.f));
    for(int i=0;i<=seg;++i){
        float t=(float)i/(float)seg;
        float a_rad=deg_to_rad(a0_deg+(a1_deg-a0_deg)*t);
        // from_angle gives the unit direction; scale by inner/outer radius
        sf::Vector2f dir=from_angle(a_rad,1.0f);
        va.append(sf::Vertex{ .position=c+dir*rin, .color=col });
        va.append(sf::Vertex{ .position=c+dir*rout, .color=col });
    }
}
void Gauge::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    sf::Vector2f c=center_of(r);
    float rout=std::min(r.size.x,r.size.y)*0.5f; float rin=rout-_thick;
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    sf::VertexArray track(sf::PrimitiveType::TriangleStrip);
    build_arc(track,c,rin,rout,_start,_start+_sweep,_track);
    draw_buffer().add_vertex_array(renderer,layer,std::move(track));
    if(_v>0.f){
        sf::VertexArray fill(sf::PrimitiveType::TriangleStrip);
        build_arc(fill,c,rin,rout,_start,_start+_sweep*_v,_fill);
        draw_buffer().add_vertex_array(renderer,layer,std::move(fill));
    }
    if(_text){ int pct=(int)(_v*100.f); draw_text(renderer,r,std::to_string(pct)+"%",Text_align::CENTER); }
}
} // namespace titan::ui
