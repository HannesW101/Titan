#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/events/include/event_type.hpp"
#include "module-core/events/include/event_data.hpp"

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using Game_callback = std::function<void(Game_event_data const&)>;

// ============================================================================
// Class Game_event_manager
// ----------------------------------------------------------------------------

/**
 * @brief Manages custom game events.
 *
 * Allows emitting and listening to custom string-based events with any payload.
 * Examples: "player_died", "enemy_spawned", "level_completed", etc.
 */
class Game_event_manager final {

public:
    Game_event_manager           (Game_event_manager const&) = delete;
    Game_event_manager& operator=(Game_event_manager const&) = delete;
    Game_event_manager           (Game_event_manager&&     ) = delete;
    Game_event_manager& operator=(Game_event_manager&&     ) = delete;

    /**
     * @brief Get singleton instance.
     * @return Reference to Game_event_manager
     */
    static Game_event_manager& instance();

    /**
     * @brief Emit a custom game event.
     * @param type Event type name (e.g., "player_died")
     * @param data Event payload
     */
    void emit(
        Game_event_type const& type,
        Game_event_data const& data = {}
        );

    /**
     * @brief Register callback for game event type.
     * @param type Event type name
     * @param callback Function to call when event is emitted
     * @param listener_id Unique listener ID for deregistration
     * @return Registration ID for this specific callback
     */
    std::uint64_t register_callback(
        Game_event_type const& type,
        Game_callback   const& callback,
        std::uint64_t   const  listener_id
        );

    /**
     * @brief Deregister specific callback.
     * @param callback_id ID returned from register_callback
     */
    void deregister_callback(std::uint64_t const callback_id);

    /**
     * @brief Deregister all callbacks for a listener.
     * @param listener_id Listener ID to remove
     */
    void deregister_listener(std::uint64_t const listener_id);

    /**
     * @brief Deregister all callbacks for an event type.
     * @param type Event type to clear
     */
    void deregister_type(Game_event_type const& type);

    /**
     * @brief Clear all registered callbacks.
     */
    void clear();

private:
    Game_event_manager();

    ~Game_event_manager() = default;

    struct Callback_entry {

        std::uint64_t callback_id;
        std::uint64_t listener_id;
        Game_callback callback;
    };

    std::unordered_map<Game_event_type, std::vector<Callback_entry>> _callbacks;
    std::uint64_t                                                    _next_callback_id;
};

} // namespace titan::events
