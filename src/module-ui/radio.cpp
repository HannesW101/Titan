// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/radio.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Radio_button::Radio_button(std::string const& name):Widget(name){ enable_click_events(); set_focusable(true); }
void Radio_button::set_label(std::string const& l){_label=l;}
bool Radio_button::is_selected() const { return _selected; }
void Radio_button::on_clicked(){ if(_group) _group->_notify(this); }
void Radio_button::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect(); float d=r.size.y;
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    sf::CircleShape outer(d*0.5f); outer.setPosition(r.position);
    outer.setFillColor(sf::Color(34,34,52)); outer.setOutlineColor(sf::Color(60,60,90)); outer.setOutlineThickness(-1.5f);
    draw_buffer().add_circle(renderer,layer,std::move(outer));
    if(_selected){ float ir=d*0.25f; sf::CircleShape inner(ir); inner.setPosition({r.position.x+d*0.5f-ir,r.position.y+d*0.5f-ir}); inner.setFillColor(sf::Color(80,140,230)); draw_buffer().add_circle(renderer,layer,std::move(inner)); }
    if(!_label.empty()){
        auto const& ta=resolved_text_style();
        std::shared_ptr<sf::Font> font;
        try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
        sf::Text t(*font,_label,static_cast<unsigned int>(ta.size>0.f?ta.size:14.f)); t.setFillColor(sf::Color(235,235,240));
        sf::FloatRect tb=t.getLocalBounds(); t.setPosition({r.position.x+d+8.f,r.position.y+(d-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
}
Radio_button* Radio_group::add(Widget& parent,std::string const& name,std::string const& label){
    auto rb=std::make_unique<Radio_button>(name); rb->set_label(label); rb->_group=this;
    Radio_button* raw=static_cast<Radio_button*>(parent.add_child(std::move(rb)));
    if(_buttons.empty()) raw->_selected=true;
    _buttons.push_back(raw); return raw;
}
void Radio_group::select(std::size_t i){ if(i>=_buttons.size())return; for(std::size_t k=0;k<_buttons.size();++k)_buttons[k]->_selected=(k==i); _selected=i; if(_on_changed)_on_changed(i); }
void Radio_group::_notify(Radio_button* b){ for(std::size_t k=0;k<_buttons.size();++k){ if(_buttons[k]==b){ select(k); return; } } }
} // namespace titan::ui
