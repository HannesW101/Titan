// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/tooltip.hpp"

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
Tooltip::Tooltip(std::string const& name):Widget(name){ set_on_overlay(true); }
void Tooltip::set_text(std::string const& t){_text=t;}
void Tooltip::set_delay(float s){_delay=s;}
void Tooltip::attach_to(Widget* t){_target=t;}
void Tooltip::on_update(float dt){
    if(!_target||!_target->is_hovered()){ _hover=0.f; _op=lerp(_op,0.f,clamp01(dt*10.f)); return; }
    _hover+=dt; if(_hover>=_delay) _op=lerp(_op,1.f,clamp01(dt*8.f));
}
void Tooltip::on_render(render::Renderer& renderer){
    if(_op<0.01f||_text.empty()) return;
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    sf::Text t(*font,_text,static_cast<unsigned int>(ta.size>0.f?ta.size:12.f));
    sf::FloatRect tb=t.getLocalBounds();
    float pad=6.f; sf::Vector2f m=mouse_position(); sf::Vector2f pos={m.x+14.f,m.y+18.f};
    sf::FloatRect bg(pos,{tb.size.x+pad*2.f,tb.size.y+pad*2.f});
    Visual v=resolved_visual();
    if(v.kind==Visual::Kind::COLOR){ v.fill_color.a=(std::uint8_t)(v.fill_color.a*_op); v.outline_color.a=(std::uint8_t)(v.outline_color.a*_op); }
    visual_renderer::draw(v,bg,renderer,draw_buffer(),resource_manager(),render::Render_layer::UI_OVERLAY);
    sf::Color c=ta.color; c.a=(std::uint8_t)(c.a*_op); t.setFillColor(c);
    t.setPosition({pos.x+pad-tb.position.x,pos.y+pad-tb.position.y});
    draw_buffer().add_text(renderer,render::Render_layer::UI_OVERLAY,std::move(t));
}
} // namespace titan::ui
