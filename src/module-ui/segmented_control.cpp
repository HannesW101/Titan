// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/segmented_control.hpp"

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
Segmented_control::Segmented_control(std::string const& name):Widget(name){ set_focusable(true);
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_enabled()||!is_hovered()||_segs.empty()) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            float sw=r.size.x/(float)_segs.size();
            std::size_t idx=(std::size_t)((m.x-r.position.x)/sw);
            if(idx<_segs.size()&&idx!=_sel){ _sel=idx; if(_on_changed)_on_changed(_sel); }
        }, listener_id());
}
void Segmented_control::set_segments(std::vector<std::string> s){_segs=std::move(s);_sel=0;}
void Segmented_control::set_selected(std::size_t i){ if(i<_segs.size())_sel=i; }
std::size_t Segmented_control::selected() const { return _sel; }
void Segmented_control::set_on_changed(std::function<void(std::size_t)> cb){_on_changed=std::move(cb);}
void Segmented_control::on_render(render::Renderer& renderer){
    if(_segs.empty()) return;
    sf::FloatRect r=rect();
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    Theme const& th=ui_manager().theme();
    Visual sel = th.has("dropdown")? th.get("dropdown").visual_for(Widget_state::HOVERED):resolved_visual();
    float sw=r.size.x/(float)_segs.size();
    for(std::size_t i=0;i<_segs.size();++i){
        sf::FloatRect sr({r.position.x+sw*(float)i,r.position.y},{sw,r.size.y});
        if(i==_sel) visual_renderer::draw(sel,sr,renderer,draw_buffer(),resource_manager(),layer);
        sf::Text t(*font,_segs[i],static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({sr.position.x+(sw-tb.size.x)*0.5f-tb.position.x,sr.position.y+(r.size.y-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
}
} // namespace titan::ui
