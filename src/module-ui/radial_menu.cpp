// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/radial_menu.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Radial_menu::Radial_menu(std::string const& name):Widget(name){
    set_on_overlay(true); set_visible(false);
    auto& sfml=events::SFML_event_manager::instance();
    _release=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!_shown) return;
            int w=_hover_wedge(mouse_position());
            if(w>=0 && _on_selected) _on_selected((std::size_t)w,_entries[(std::size_t)w]);
            hide();
        }, listener_id());
}
void Radial_menu::set_entries(std::vector<std::string> e){_entries=std::move(e);}
void Radial_menu::set_radius(float i,float o){_rin=i;_rout=o;}
void Radial_menu::show_at(sf::Vector2f c){_center=c;_shown=true;set_visible(true);}
void Radial_menu::hide(){_shown=false;set_visible(false);}
bool Radial_menu::is_shown() const { return _shown; }
void Radial_menu::set_on_selected(std::function<void(std::size_t,std::string const&)> cb){_on_selected=std::move(cb);}
int Radial_menu::_hover_wedge(sf::Vector2f m) const {
    if(_entries.empty()) return -1;
    sf::Vector2f d=m-_center;
    float dist=length(d);                 // math lib
    if(dist<_rin||dist>_rout) return -1;
    float ang=angle_of(d);                // math lib
    if(ang<0.f) ang+=TAU;                 // math lib constant
    float per=TAU/(float)_entries.size();
    return (int)(ang/per)%(int)_entries.size();
}
void Radial_menu::on_render(render::Renderer& renderer){
    if(!_shown||_entries.empty()) return;
    int hov=_hover_wedge(mouse_position());
    float per=TAU/(float)_entries.size();
    for(std::size_t i=0;i<_entries.size();++i){
        float a0=(float)i*per, a1=a0+per;
        sf::VertexArray wedge(sf::PrimitiveType::TriangleStrip);
        int seg=8;
        sf::Color col = ((int)i==hov)? sf::Color(80,140,230) : sf::Color(40,40,60,230);
        for(int s=0;s<=seg;++s){
            float t=(float)s/(float)seg; float a=a0+(a1-a0)*t;
            sf::Vector2f dir=from_angle(a,1.0f);     // math lib
            wedge.append(sf::Vertex{ .position=_center+dir*_rin, .color=col });
            wedge.append(sf::Vertex{ .position=_center+dir*_rout, .color=col });
        }
        draw_buffer().add_vertex_array(renderer,render::Render_layer::UI_OVERLAY,std::move(wedge));
    }
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    float rmid=(_rin+_rout)*0.5f;
    for(std::size_t i=0;i<_entries.size();++i){
        float a=((float)i+0.5f)*per;
        sf::Vector2f p=_center+from_angle(a,rmid);   // math lib
        sf::Text t(*font,_entries[i],static_cast<unsigned int>(ta.size>0.f?ta.size:14.f));
        t.setFillColor(sf::Color::White);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({p.x-tb.size.x*0.5f-tb.position.x,p.y-tb.size.y*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,render::Render_layer::UI_OVERLAY,std::move(t));
    }
}
} // namespace titan::ui
