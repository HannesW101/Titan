// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/script.hpp"
#include "module-game/include/game_object.hpp"

#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Script
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void Script::set_start_fn       (Start_fn        fn) { _start = std::move(fn);        }
void Script::set_update_fn      (Update_fn       fn) { _update = std::move(fn);       }
void Script::set_fixed_update_fn(Fixed_update_fn fn) { _fixed_update = std::move(fn); }
void Script::set_detach_fn      (Detach_fn       fn) { _detach = std::move(fn);       }

// ----------------------------------------------------------------------------
void Script::on_start() {

    if (_start) { _start(owner()); }
}

// ----------------------------------------------------------------------------
void Script::on_detach() {

    if (_detach) { _detach(owner()); }
}

// ----------------------------------------------------------------------------
void Script::update(
    float const dt
    ) {

    if (_update) { _update(owner(), dt); }
}

// ----------------------------------------------------------------------------
void Script::fixed_update(
    float const fixed_dt
    ) {

    if (_fixed_update) { _fixed_update(owner(), fixed_dt); }
}

} // namespace titan::game
