// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/virtual_list.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

#include <algorithm>
#include <cmath>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Virtual_list::Virtual_list(std::string const& name):Widget(name){
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_hovered()||!_on_clicked) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            std::size_t idx=(std::size_t)((m.y-r.position.y+_scroll)/_ih);
            if(idx<_count) _on_clicked(idx);
        }, listener_id());
    _up=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_UP,
        [this](events::SFML_event_data const&){ if(is_hovered()) _scroll=std::max(0.f,_scroll-_ih*3.f); }, listener_id());
    _dn=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_DOWN,
        [this](events::SFML_event_data const&){ if(is_hovered()){ float tot=(float)_count*_ih,mx=std::max(0.f,tot-rect().size.y); _scroll=std::min(mx,_scroll+_ih*3.f);} }, listener_id());
}
void Virtual_list::set_item_count(std::size_t n){_count=n;}
void Virtual_list::set_item_height(float h){_ih=h;}
void Virtual_list::set_text_provider(std::function<std::string(std::size_t)> fn){_provider=std::move(fn);}
void Virtual_list::set_on_clicked(std::function<void(std::size_t)> cb){_on_clicked=std::move(cb);}
std::size_t Virtual_list::item_count() const { return _count; }
void Virtual_list::on_render(render::Renderer& renderer){
    if(!_provider||_count==0) return;
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    sf::FloatRect r=rect();
    std::size_t first=(std::size_t)std::floor(_scroll/_ih);
    std::size_t visible=(std::size_t)std::ceil(r.size.y/_ih)+1u;
    for(std::size_t i=first;i<std::min(_count,first+visible);++i){
        float y=r.position.y+(float)i*_ih-_scroll;
        sf::Text t(*font,_provider(i),static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({r.position.x+8.f,y+(_ih-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
}
} // namespace titan::ui
