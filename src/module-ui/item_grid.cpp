// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/item_grid.hpp"

#include "module-ui/include/item_slot.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Item_grid::Item_grid(std::string const& name):Widget(name){ set_layout_mode(Layout_mode::GRID); }
void Item_grid::configure(std::size_t c,std::size_t r,float s,float g){_cols=c;_rows=r;_slot=s;_gap=g;set_grid_columns(c);set_spacing(g);}
Item_slot* Item_grid::slot(std::size_t i){ return i<_slots.size()?_slots[i]:nullptr; }
std::size_t Item_grid::slot_count() const { return _slots.size(); }
void Item_grid::on_layout(){ if(!_built){ _build(); _built=true; } }
void Item_grid::_build(){
    std::size_t n=_cols*_rows;
    for(std::size_t i=0;i<n;++i){
        auto s=std::make_unique<Item_slot>(name()+"_slot"+std::to_string(i));
        s->set_size(UI_length::px(_slot),UI_length::px(_slot));
        _slots.push_back(static_cast<Item_slot*>(add_child(std::move(s))));
    }
}
} // namespace titan::ui
