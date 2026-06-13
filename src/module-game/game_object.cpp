// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_object.hpp"

#include <stdexcept>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Construction
// ============================================================================

// ----------------------------------------------------------------------------
Game_object::Game_object(
    std::string name,
    std::uint64_t const id
    )
    : _name(std::move(name))
    , _id(id)
    {

    // Every object owns a Transform. add_component caches the pointer.
    _transform = add_component<Transform>();
}

// ----------------------------------------------------------------------------
Game_object::~Game_object() {

    // Detach in reverse order so dependents tear down before dependencies.
    for (auto it = _components.rbegin(); it != _components.rend(); ++it) {

        (*it)->on_detach();
    }
}

// ============================================================================
// Identity
// ============================================================================

// ----------------------------------------------------------------------------
std::string const& Game_object::name    (                ) const { return _name;            }
void               Game_object::set_name(std::string name)       { _name = std::move(name); }
std::uint64_t      Game_object::id      (                ) const { return _id;              }

// ============================================================================
// Active / destroy
// ============================================================================

// ----------------------------------------------------------------------------
bool Game_object::is_active (            ) const { return _active; }
void Game_object::set_active(bool const a)       { _active = a;    }

// ----------------------------------------------------------------------------
void Game_object::destroy     ()       { _destroyed = true; }
bool Game_object::is_destroyed() const { return _destroyed; }

// ============================================================================
// Transform
// ============================================================================

// ----------------------------------------------------------------------------
Transform&       Game_object::transform()        { return *_transform; }
Transform const& Game_object::transform() const  { return *_transform; }

// ============================================================================
// World context
// ============================================================================

// ----------------------------------------------------------------------------
bool Game_object::has_world() const { return _world != nullptr; }

// ----------------------------------------------------------------------------
World& Game_object::world() {

    if (!_world) { throw std::runtime_error("Game_object '" + _name + "' has no World"); }

    return *_world;
}

// ----------------------------------------------------------------------------
World const& Game_object::world() const {

    if (!_world) { throw std::runtime_error("Game_object '" + _name + "' has no World"); }

    return *_world;
}

// ============================================================================
// Per-frame
// ============================================================================

// ----------------------------------------------------------------------------
void Game_object::_start_if_needed() {

    if (_started) { return; }

    _started = true;

    for (auto& c : _components) {

        if (c->is_enabled() && !c->_started) {

            c->_started = true;
            c->on_start();
        }
    }
}

// ----------------------------------------------------------------------------
void Game_object::fixed_update(
    float const fixed_dt
    ) {

    if (!_active || _destroyed) { return; }

    _start_if_needed();

    for (auto& c : _components) {

        if (c->is_enabled()) { c->fixed_update(fixed_dt); }
    }
}

// ----------------------------------------------------------------------------
void Game_object::update(
    float const dt
    ) {

    if (!_active || _destroyed) { return; }

    _start_if_needed();

    // Newly added components (after first start) also get their on_start.
    for (auto& c : _components) {

        if (c->is_enabled() && !c->_started) {

            c->_started = true;
            c->on_start();
        }
    }

    for (auto& c : _components) {

        if (c->is_enabled()) { c->update(dt); }
    }
}

// ----------------------------------------------------------------------------
void Game_object::render(
    render::Renderer& renderer
    ) {

    if (!_active || _destroyed) { return; }

    for (auto& c : _components) {

        if (c->is_enabled()) { c->render(renderer); }
    }
}

} // namespace titan::game
