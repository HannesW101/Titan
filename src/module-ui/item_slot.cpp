// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/item_slot.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/visual.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Item_slot::Item_slot(std::string const& name):Widget(name){ enable_click_events(); }
void Item_slot::set_item(std::int64_t id,std::string const& icon){_item=id;_icon=icon;}
void Item_slot::clear_item(){_item=-1;_icon.clear();_count=0;}
std::int64_t Item_slot::item_id() const { return _item; }
void Item_slot::set_count(int c){_count=c;}
void Item_slot::set_on_changed(std::function<void()> cb){_on_changed=std::move(cb);}
void Item_slot::on_pressed(){
    if(_item>=0){ ui_manager().begin_drag(std::int64_t(_item), DRAG_KIND_ITEM); }
}
void Item_slot::on_released(){
    auto& gui=ui_manager();
    if(gui.is_dragging() && gui.drag_kind()==DRAG_KIND_ITEM && is_hovered()){
        try{
            std::int64_t incoming=std::any_cast<std::int64_t>(gui.drag_payload());
            _item=incoming; // (real game would also set icon via a lookup)
            if(_on_changed)_on_changed();
        }catch(std::bad_any_cast const&){}
        gui.end_drag();
    } else if(gui.is_dragging() && gui.drag_kind()==DRAG_KIND_ITEM){
        // dropped elsewhere; if this was the source, optionally clear (game decides)
    }
}
void Item_slot::on_render(render::Renderer& renderer){
    if(_icon.empty()||_item<0) return;
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    Visual v=Visual::texture(_icon,sf::Color::White,true);
    sf::FloatRect r=rect();
    sf::FloatRect inner({r.position.x+4.f,r.position.y+4.f},{r.size.x-8.f,r.size.y-8.f});
    visual_renderer::draw(v,inner,renderer,draw_buffer(),resource_manager(),layer);
    if(_count>1) draw_text(renderer,r,std::to_string(_count),Text_align::RIGHT);
}
} // namespace titan::ui
