// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/boxes.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

H_box::H_box(std::string const& n):Widget(n){ set_layout_mode(Layout_mode::HORIZONTAL); }
V_box::V_box(std::string const& n):Widget(n){ set_layout_mode(Layout_mode::VERTICAL); }
Grid::Grid(std::string const& n):Widget(n){ set_layout_mode(Layout_mode::GRID); }
void Grid::set_columns(std::size_t c){ set_grid_columns(c); }
} // namespace titan::ui
