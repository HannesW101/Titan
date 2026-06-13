// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/list_box.hpp"

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

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
std::string const List_box::_empty;
List_box::List_box(std::string const& name):Widget(name){
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_enabled()||!is_hovered()) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            float ry=m.y-r.position.y+_scroll; auto idx=(std::size_t)(ry/_ih);
            if(idx>=_items.size()) return;
            if(_multisel){ auto it=std::find(_multi.begin(),_multi.end(),idx); if(it!=_multi.end())_multi.erase(it); else _multi.push_back(idx); }
            else _sel=idx;
            if(_on_changed)_on_changed(idx);
        }, listener_id());
    _up=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_UP,
        [this](events::SFML_event_data const&){ if(is_hovered())_scroll=std::max(0.f,_scroll-_ih); }, listener_id());
    _dn=sfml.register_callback(events::SFML_event_type::MOUSE_WHEEL_DOWN,
        [this](events::SFML_event_data const&){ if(is_hovered()){ float tot=static_cast<float>(_items.size())*_ih,mx=std::max(0.f,tot-rect().size.y); _scroll=std::min(mx,_scroll+_ih);} }, listener_id());
}
void List_box::set_items(std::vector<std::string> i){_items=std::move(i);_sel=0;_multi.clear();_scroll=0.f;}
void List_box::add_item(std::string const& s){_items.push_back(s);}
void List_box::clear_items(){_items.clear();_multi.clear();_sel=0;_scroll=0.f;}
void List_box::set_selected(std::size_t i){ if(i<_items.size())_sel=i; }
std::size_t List_box::selected() const { return _sel; }
std::string const& List_box::selected_text() const { return _items.empty()?_empty:_items[_sel]; }
void List_box::set_multi_select(bool on){_multisel=on;}
std::vector<std::size_t> const& List_box::selected_indices() const { return _multi; }
void List_box::set_item_height(float h){_ih=h;}
void List_box::set_on_changed(std::function<void(std::size_t)> cb){_on_changed=std::move(cb);}
void List_box::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    Theme const& th=ui_manager().theme();
    Visual sel_v = th.has("item")? th.get("item").visual_for(Widget_state::PRESSED):resolved_visual();
    for(std::size_t i=0;i<_items.size();++i){
        float y=r.position.y+(float)i*_ih-_scroll;
        if(y+_ih<r.position.y) { continue; }
        if(y>r.position.y+r.size.y) { break; }
        bool sel = _multisel ? (std::find(_multi.begin(),_multi.end(),i)!=_multi.end()) : (i==_sel);
        if(sel) visual_renderer::draw(sel_v,sf::FloatRect({r.position.x,y},{r.size.x,_ih}),renderer,draw_buffer(),resource_manager(),layer);
        sf::Text t(*font,_items[i],static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds(); t.setPosition({r.position.x+8.f,y+(_ih-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
}
} // namespace titan::ui
