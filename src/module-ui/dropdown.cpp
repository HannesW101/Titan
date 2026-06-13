// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/dropdown.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
std::string const Dropdown::_empty;
Dropdown::Dropdown(std::string const& name) : Widget(name) {
    enable_click_events(); set_focusable(true);
    // a separate global handler closes/selects when the list is open
    auto& sfml = events::SFML_event_manager::instance();
    _global_click = sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!_open) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            float ih=r.size.y;
            for(std::size_t i=0;i<_opts.size();++i){
                sf::FloatRect item({r.position.x,r.position.y+r.size.y+ih*(float)i},{r.size.x,ih});
                if(contains(item,m)){ _sel=i; _open=false; if(_on_changed)_on_changed(_sel,_opts[_sel]); return; }
            }
            if(!contains(r,m)) _open=false;
        }, listener_id());
}
void Dropdown::set_options(std::vector<std::string> o){_opts=std::move(o);_sel=0;}
void Dropdown::set_selected(std::size_t i){ if(i<_opts.size())_sel=i; }
std::size_t Dropdown::selected_index() const { return _sel; }
std::string const& Dropdown::selected_text() const { return _opts.empty()?_empty:_opts[_sel]; }
void Dropdown::set_on_changed(std::function<void(std::size_t,std::string const&)> cb){_on_changed=std::move(cb);}
void Dropdown::on_clicked(){ _open=!_open; }
void Dropdown::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    if(!_opts.empty()){
        sf::Text t(*font,_opts[_sel],static_cast<unsigned int>(ta.size));
        t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({r.position.x+8.f,r.position.y+(r.size.y-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
    sf::ConvexShape arrow; arrow.setPointCount(3u);
    float as=r.size.y*0.3f; float ax=r.position.x+r.size.x-as-8.f; float ay=r.position.y+(r.size.y-as)*0.5f;
    arrow.setPoint(0u,{0.f,0.f}); arrow.setPoint(1u,{as,0.f}); arrow.setPoint(2u,{as*0.5f,as});
    arrow.setPosition({ax,ay}); arrow.setFillColor(ta.color);
    draw_buffer().add_convex(renderer,layer,std::move(arrow));
    if(_open){
        Theme const& th=ui_manager().theme();
        Visual item_n = th.has("item")? th.get("item").visual_for(Widget_state::NORMAL):resolved_visual();
        Visual item_h = th.has("item")? th.get("item").visual_for(Widget_state::HOVERED):resolved_visual();
        float ih=r.size.y; sf::Vector2f m=mouse_position();
        for(std::size_t i=0;i<_opts.size();++i){
            float y=r.position.y+r.size.y+ih*(float)i;
            sf::FloatRect ir({r.position.x,y},{r.size.x,ih});
            bool hov=contains(ir,m);
            visual_renderer::draw(hov?item_h:item_n,ir,renderer,draw_buffer(),resource_manager(),render::Render_layer::UI_OVERLAY);
            sf::Text t(*font,_opts[i],static_cast<unsigned int>(ta.size));
            t.setFillColor(ta.color);
            sf::FloatRect tb=t.getLocalBounds();
            t.setPosition({r.position.x+8.f,y+(ih-tb.size.y)*0.5f-tb.position.y});
            draw_buffer().add_text(renderer,render::Render_layer::UI_OVERLAY,std::move(t));
        }
    }
}
} // namespace titan::ui
