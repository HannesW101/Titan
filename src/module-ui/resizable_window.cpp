// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/resizable_window.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Resizable_window::Resizable_window(std::string const& name):Widget(name){ enable_click_events(); }
void Resizable_window::set_title(std::string const& t){_title=t;}
void Resizable_window::set_min_size(sf::Vector2f s){_min=s;}
void Resizable_window::set_title_height(float h){_title_h=h;}
void Resizable_window::on_pressed(){
    sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
    sf::FloatRect corner({r.position.x+r.size.x-_handle,r.position.y+r.size.y-_handle},{_handle,_handle});
    if(contains(corner,m)){ _resize=true; _grab=m; _start_size=r.size; return; }
    sf::FloatRect title(r.position,{r.size.x,_title_h});
    if(contains(title,m)){ _drag=true; _grab={m.x-r.position.x,m.y-r.position.y}; }
}
void Resizable_window::on_released(){_drag=false;_resize=false;}
void Resizable_window::on_update(float){
    sf::Vector2f m=mouse_position();
    if(_drag){
        sf::Vector2f po=parent()?parent()->rect().position:sf::Vector2f{0,0};
        set_anchor(UI_anchor::TOP_LEFT); set_offset({m.x-_grab.x-po.x,m.y-_grab.y-po.y});
    } else if(_resize){
        sf::Vector2f d=m-_grab;
        float w=std::max(_min.x,_start_size.x+d.x);
        float h=std::max(_min.y,_start_size.y+d.y);
        set_size(UI_length::px(w),UI_length::px(h));
    }
}
void Resizable_window::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    Theme const& th=ui_manager().theme();
    Visual tb = th.has("window_titlebar")? th.get("window_titlebar").visual_for(Widget_state::NORMAL):resolved_visual();
    visual_renderer::draw(tb,sf::FloatRect(r.position,{r.size.x,_title_h}),renderer,draw_buffer(),resource_manager(),layer);
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    if(!_title.empty()){
        sf::Text t(*font,_title,static_cast<unsigned int>(ta.size>0.f?ta.size:15.f)); t.setFillColor(ta.color);
        sf::FloatRect b=t.getLocalBounds(); t.setPosition({r.position.x+8.f,r.position.y+(_title_h-b.size.y)*0.5f-b.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
    // resize grip (triangle)
    sf::ConvexShape grip; grip.setPointCount(3u);
    grip.setPoint(0u,{r.position.x+r.size.x,r.position.y+r.size.y-_handle});
    grip.setPoint(1u,{r.position.x+r.size.x,r.position.y+r.size.y});
    grip.setPoint(2u,{r.position.x+r.size.x-_handle,r.position.y+r.size.y});
    grip.setFillColor(sf::Color(120,120,150));
    draw_buffer().add_convex(renderer,layer,std::move(grip));
}
} // namespace titan::ui
