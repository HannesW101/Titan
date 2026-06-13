// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/modal.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Modal::Modal(std::string const& name):Widget(name){ enable_click_events(); set_on_overlay(true); set_visible(false); }
void Modal::set_title(std::string const& t){_title=t;}
void Modal::set_closeable(bool c){_closeable=c;}
void Modal::show(){_shown=true;set_visible(true);}
void Modal::hide(){_shown=false;set_visible(false);}
bool Modal::is_shown() const { return _shown; }
void Modal::set_on_close(std::function<void()> cb){_on_close=std::move(cb);}
void Modal::on_pressed(){
    if(!_shown||!_closeable) return;
    sf::Vector2f m=mouse_position();
    if(!contains(rect(),m)){ hide(); if(_on_close)_on_close(); }
}
void Modal::on_render(render::Renderer& renderer){
    if(!_shown) return;
    auto const& win=ui_manager().renderer().window(); sf::Vector2u ws=win.getSize();
    sf::RectangleShape back; back.setPosition({0,0}); back.setSize({(float)ws.x,(float)ws.y}); back.setFillColor(sf::Color(0,0,0,150));
    draw_buffer().add_rect(renderer,render::Render_layer::UI_OVERLAY,std::move(back),-1.f);
    visual_renderer::draw(resolved_visual(),rect(),renderer,draw_buffer(),resource_manager(),render::Render_layer::UI_OVERLAY);
    if(!_title.empty()){
        auto const& ta=resolved_text_style();
        std::shared_ptr<sf::Font> font;
        try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
        sf::FloatRect r=rect();
        sf::RectangleShape bar; bar.setPosition(r.position); bar.setSize({r.size.x,_title_h}); bar.setFillColor(sf::Color(80,140,230));
        draw_buffer().add_rect(renderer,render::Render_layer::UI_OVERLAY,std::move(bar));
        sf::Text t(*font,_title,static_cast<unsigned int>(ta.size>0.f?ta.size:16.f)); t.setFillColor(sf::Color::White);
        sf::FloatRect b=t.getLocalBounds(); t.setPosition({r.position.x+12.f,r.position.y+(_title_h-b.size.y)*0.5f-b.position.y});
        draw_buffer().add_text(renderer,render::Render_layer::UI_OVERLAY,std::move(t));
    }
}
} // namespace titan::ui
