// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/tab_view.hpp"

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
Tab_view::Tab_view(std::string const& name):Widget(name){
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_enabled()||!is_visible()||_tabs.empty()) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            if(m.y<r.position.y||m.y>r.position.y+_tab_h) return;
            if(m.x<r.position.x||m.x>r.position.x+r.size.x) return;
            float tw=r.size.x/(float)_tabs.size();
            std::size_t idx=(std::size_t)((m.x-r.position.x)/tw);
            if(idx<_tabs.size()&&idx!=_active){ _active=idx; if(_on_changed)_on_changed(_active,_tabs[_active].name); }
        }, listener_id());
}
void Tab_view::add_tab(std::string const& n,std::unique_ptr<Widget> c){ c->set_anchor(UI_anchor::TOP_LEFT); _tabs.push_back({n,std::move(c)}); }
void Tab_view::set_active_tab(std::size_t i){ if(i<_tabs.size()){ _active=i; if(_on_changed)_on_changed(_active,_tabs[_active].name); } }
std::size_t Tab_view::active_tab() const { return _active; }
void Tab_view::set_tab_height(float h){_tab_h=h;}
void Tab_view::set_on_tab_changed(std::function<void(std::size_t,std::string const&)> cb){_on_changed=std::move(cb);}
void Tab_view::on_layout(){
    if(_tabs.empty()) return;
    sf::FloatRect r=rect();
    sf::FloatRect content({r.position.x,r.position.y+_tab_h},{r.size.x,r.size.y-_tab_h});
    // ensure content widget knows the manager (set via friend access in add)
    _tabs[_active].content->do_layout(content);
}
void Tab_view::on_update(float dt){ if(_active<_tabs.size()) _tabs[_active].content->update(dt); }
void Tab_view::on_render(render::Renderer& renderer){
    if(_tabs.empty()) return;
    sf::FloatRect r=rect();
    Theme const& th=ui_manager().theme();
    Visual_set const& tab_vs = th.has("tab")?th.get("tab"):Visual_set{};
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    float tw=r.size.x/(float)_tabs.size();
    sf::Vector2f m=mouse_position();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    for(std::size_t i=0;i<_tabs.size();++i){
        sf::FloatRect tr({r.position.x+tw*(float)i,r.position.y},{tw,_tab_h});
        Widget_state st = Widget_state::NORMAL;
        if(i==_active) st=Widget_state::PRESSED;
        else if(contains(tr,m)) st=Widget_state::HOVERED;
        Visual const& v = tab_vs.visual_for(st);
        visual_renderer::draw(v,tr,renderer,draw_buffer(),resource_manager(),layer);
        Text_appearance const& tta = tab_vs.text_for(st);
        sf::Text t(*font,_tabs[i].name,static_cast<unsigned int>(tta.size>0.f?tta.size:14.f));
        t.setFillColor(tta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({tr.position.x+(tw-tb.size.x)*0.5f-tb.position.x,tr.position.y+(_tab_h-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
    // render the active content widget through normal render() since we drove its layout
    _tabs[_active].content->render(renderer);
}
} // namespace titan::ui
