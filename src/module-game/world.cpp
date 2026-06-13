// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/world.hpp"

#include <algorithm>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Spawning
// ============================================================================

// ----------------------------------------------------------------------------
Game_object* World::spawn(
    std::string name
    ) {

    auto obj = std::make_unique<Game_object>(std::move(name), _next_id++);
    obj->_world = this;
    Game_object* raw = obj.get();

    // Buffer new spawns so spawning during iteration is safe; merged at frame
    // boundaries in update().
    _spawned_this_frame.push_back(std::move(obj));
    return raw;
}

// ----------------------------------------------------------------------------
void World::clear() {

    _objects.clear();
    _spawned_this_frame.clear();
}

// ============================================================================
// Queries
// ============================================================================

// ----------------------------------------------------------------------------
Game_object* World::find(
    std::string const& name
    ) {

    for (auto& o : _objects) {

        if (!o->is_destroyed() && o->name() == name) { return o.get(); }
    }

    for (auto& o : _spawned_this_frame) {

        if (!o->is_destroyed() && o->name() == name) { return o.get(); }
    }

    return nullptr;
}

// ----------------------------------------------------------------------------
Game_object* World::find_by_id(
    std::uint64_t const id
    ) {

    for (auto& o : _objects) {

        if (o->id() == id && !o->is_destroyed()) { return o.get(); }
    }

    for (auto& o : _spawned_this_frame) {

        if (o->id() == id && !o->is_destroyed()) { return o.get(); }
    }

    return nullptr;
}

// ----------------------------------------------------------------------------
std::size_t World::count() const { return _objects.size() + _spawned_this_frame.size(); }

// ----------------------------------------------------------------------------
std::vector<std::unique_ptr<Game_object>>&       World::objects()       { return _objects; }
std::vector<std::unique_ptr<Game_object>> const& World::objects() const { return _objects; }

// ============================================================================
// Per-frame
// ============================================================================

// ----------------------------------------------------------------------------
void World::_reap_destroyed() {

    _objects.erase(
        std::remove_if(
            _objects.begin(),
            _objects.end(),
            [](std::unique_ptr<Game_object> const& o) { return o->is_destroyed(); }
            ),
        _objects.end()
        );
}

// ----------------------------------------------------------------------------
void World::fixed_update(
    float const fixed_dt
    ) {

    for (auto& o : _objects) {

        o->fixed_update(fixed_dt);
    }
}

// ----------------------------------------------------------------------------
void World::update(
    float const dt
    ) {

    // Merge objects spawned last frame (or during the previous update) so they
    // become live before this update runs.
    if (!_spawned_this_frame.empty()) {

        for (auto& o : _spawned_this_frame) {

            _objects.push_back(std::move(o));
        }

        _spawned_this_frame.clear();
    }

    for (auto& o : _objects) {

        o->update(dt);
    }

    // Merge anything spawned *during* this update too, so it's tracked.
    if (!_spawned_this_frame.empty()) {

        for (auto& o : _spawned_this_frame) {

            _objects.push_back(std::move(o));
        }

        _spawned_this_frame.clear();
    }

    _reap_destroyed();
}

// ----------------------------------------------------------------------------
void World::render(
    render::Renderer& renderer
    ) {

    for (auto& o : _objects) {

        o->render(renderer);
    }
}

} // namespace titan::game
