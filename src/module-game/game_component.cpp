// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include <stdexcept>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Game_component
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
bool Game_component::is_enabled (                  ) const { return _enabled;    }
void Game_component::set_enabled(bool const enabled)       { _enabled = enabled; }

// ----------------------------------------------------------------------------
bool Game_component::has_owner() const { return _owner != nullptr; }

// ----------------------------------------------------------------------------
Game_object& Game_component::owner() {

    if (!_owner) { throw std::runtime_error("Game_component has no owner"); }

    return *_owner;
}

// ----------------------------------------------------------------------------
Game_object const& Game_component::owner() const {

    if (!_owner) { throw std::runtime_error("Game_component has no owner"); }

    return *_owner;
}

} // namespace titan::game
