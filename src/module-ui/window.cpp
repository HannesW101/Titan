// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/window.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
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
Window::Window(std::string const& name):Widget(name){ enable_click_events(); }
void Window::set_title(std::string const& t){_title=t;}
void Window::set_closeable(bool c){_closeable=c;}
void Window::set_title_height(float h){_title_h=h;}
void Window::set_on_close(std::function<void()> cb){_on_close=std::move(cb);}
void Window::on_pressed(){
    sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
    sf::FloatRect title(r.position,{r.size.x,_title_h});
    if(contains(title,m)){
        if(_closeable){ float cs=_title_h*0.7f; sf::FloatRect cl({r.position.x+r.size.x-cs-4.f,r.position.y+(_title_h-cs)*0.5f},{cs,cs}); if(contains(cl,m)){ if(_on_close)_on_close(); set_visible(false); return; } }
        _drag=true; _grab={m.x-r.position.x,m.y-r.position.y};
    }
}
void Window::on_released(){_drag=false;}
void Window::on_update(float){
    if(!_drag) return;
    sf::Vector2f m=mouse_position();
    sf::Vector2f po = parent()? parent()->rect().position : sf::Vector2f{0,0};
    set_anchor(UI_anchor::TOP_LEFT);
    set_offset({ m.x-_grab.x-po.x, m.y-_grab.y-po.y });
}
void Window::on_render(render::Renderer& renderer){
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
    if(_closeable){
        float cs=_title_h*0.7f; sf::Vector2f cp={r.position.x+r.size.x-cs-4.f,r.position.y+(_title_h-cs)*0.5f};
        sf::Text x(*font,"X",static_cast<unsigned int>(ta.size>0.f?ta.size:15.f)); x.setFillColor(ta.color);
        sf::FloatRect xb=x.getLocalBounds(); x.setPosition({cp.x+(cs-xb.size.x)*0.5f-xb.position.x,cp.y+(cs-xb.size.y)*0.5f-xb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(x));
    }
}
} // namespace titan::ui
