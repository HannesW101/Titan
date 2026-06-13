// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/menu.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Menu_bar::Menu_bar(std::string const& name):Widget(name){
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_enabled()||!is_hovered()||_entries.empty()) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            float ew=r.size.x/(float)_entries.size();
            std::size_t idx=(std::size_t)((m.x-r.position.x)/ew);
            if(idx<_entries.size() && _entries[idx].action) _entries[idx].action();
        }, listener_id());
}
void Menu_bar::add_entry(std::string const& l,std::function<void()> a){_entries.push_back({l,std::move(a)});}
void Menu_bar::clear(){_entries.clear();}
void Menu_bar::on_render(render::Renderer& renderer){
    if(_entries.empty()) return;
    sf::FloatRect r=rect();
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    Theme const& th=ui_manager().theme();
    Visual hov = th.has("menu_item")? th.get("menu_item").visual_for(Widget_state::HOVERED):resolved_visual();
    float ew=r.size.x/(float)_entries.size();
    sf::Vector2f m=mouse_position();
    for(std::size_t i=0;i<_entries.size();++i){
        sf::FloatRect er({r.position.x+ew*(float)i,r.position.y},{ew,r.size.y});
        if(contains(er,m)) visual_renderer::draw(hov,er,renderer,draw_buffer(),resource_manager(),layer);
        sf::Text t(*font,_entries[i].label,static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({er.position.x+(ew-tb.size.x)*0.5f-tb.position.x,er.position.y+(er.size.y-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
}

Context_menu::Context_menu(std::string const& name):Widget(name){
    set_on_overlay(true); set_visible(false);
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!_shown) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            if(!contains(r,m)){ hide(); return; }
            float ry=m.y-r.position.y; std::size_t idx=(std::size_t)(ry/_row_h);
            if(idx<_entries.size()){ if(_entries[idx].action) _entries[idx].action(); hide(); }
        }, listener_id());
}
void Context_menu::add_entry(std::string const& l,std::function<void()> a){_entries.push_back({l,std::move(a)});}
void Context_menu::clear(){_entries.clear();}
void Context_menu::show_at(sf::Vector2f p){ _pos=p; _shown=true; set_visible(true); set_anchor(UI_anchor::TOP_LEFT); set_offset(p); set_height(UI_length::px((float)_entries.size()*_row_h)); }
void Context_menu::hide(){ _shown=false; set_visible(false); }
bool Context_menu::is_shown() const { return _shown; }
void Context_menu::on_render(render::Renderer& renderer){
    if(!_shown||_entries.empty()) return;
    sf::FloatRect r=rect();
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    Theme const& th=ui_manager().theme();
    Visual hov = th.has("menu_item")? th.get("menu_item").visual_for(Widget_state::HOVERED):resolved_visual();
    sf::Vector2f m=mouse_position();
    for(std::size_t i=0;i<_entries.size();++i){
        sf::FloatRect ir({r.position.x,r.position.y+_row_h*(float)i},{r.size.x,_row_h});
        if(contains(ir,m)) visual_renderer::draw(hov,ir,renderer,draw_buffer(),resource_manager(),render::Render_layer::UI_OVERLAY);
        sf::Text t(*font,_entries[i].label,static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({ir.position.x+8.f,ir.position.y+(_row_h-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,render::Render_layer::UI_OVERLAY,std::move(t));
    }
}
} // namespace titan::ui
