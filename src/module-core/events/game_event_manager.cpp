// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/events/include/game_event_manager.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Class Game_event_manager
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Game_event_manager::Game_event_manager()
    : _callbacks       (  )
    , _next_callback_id(1u)
{}

// ----------------------------------------------------------------------------
Game_event_manager& Game_event_manager::instance() {

    static Game_event_manager instance;
    return instance;
}

// ----------------------------------------------------------------------------
void Game_event_manager::emit(
    Game_event_type const& type,
    Game_event_data const& data
    ) {

    auto const it = _callbacks.find(type);

    if (it == _callbacks.cend()) { return; }

    // Copy vector in case callbacks modify the list
    auto const entries = it->second;

    for (auto const& entry : entries) {

        entry.callback(data);
    }
}

// ----------------------------------------------------------------------------
std::uint64_t Game_event_manager::register_callback(
    Game_event_type const& type,
    Game_callback   const& callback,
    std::uint64_t   const  listener_id
    ) {

    std::uint64_t const callback_id = _next_callback_id++;

    _callbacks[type].push_back({callback_id, listener_id, callback});

    return callback_id;
}

// ----------------------------------------------------------------------------
void Game_event_manager::deregister_callback(
    std::uint64_t const callback_id
    ) {

    for (auto& [type, entries] : _callbacks) {

        auto const it = std::remove_if(
            entries.begin(),
            entries.end(),
            [callback_id](Callback_entry const& entry) {

                return entry.callback_id == callback_id;
            }
            );

        entries.erase(it, entries.cend());
    }
}

// ----------------------------------------------------------------------------
void Game_event_manager::deregister_listener(
    std::uint64_t const listener_id
    ) {

    for (auto& [type, entries] : _callbacks) {

        auto const it = std::remove_if(
            entries.begin(),
            entries.end(),
            [listener_id](Callback_entry const& entry) {

                return entry.listener_id == listener_id;
            }
            );

        entries.erase(it, entries.cend());
    }
}

// ----------------------------------------------------------------------------
void Game_event_manager::deregister_type(
    Game_event_type const& type
    ) {

    _callbacks.erase(type);
}

// ----------------------------------------------------------------------------
void Game_event_manager::clear() {

    _callbacks.clear();
}

} // namespace titan::events
