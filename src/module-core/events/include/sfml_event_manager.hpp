#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/events/include/event_type.hpp"
#include "module-core/events/include/event_data.hpp"

#include "SFML/Graphics.hpp"

#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using SFML_callback = std::function<void(SFML_event_data const&)>;

// ============================================================================
// Class SFML_event_manager
// ----------------------------------------------------------------------------

/**
 * @brief Manages SFML window and input events with per-key/button granularity.
 */
class SFML_event_manager final {

public:
    SFML_event_manager           (SFML_event_manager const&) = delete;
    SFML_event_manager& operator=(SFML_event_manager const&) = delete;
    SFML_event_manager           (SFML_event_manager&&     ) = delete;
    SFML_event_manager& operator=(SFML_event_manager&&     ) = delete;

    /**
     * @brief Get singleton instance.
     * @return Reference to SFML_event_manager
     */
    static SFML_event_manager& instance();

    /**
     * @brief Process SFML events from window.
     * @param window SFML window to poll events from
     */
    void process_events(sf::Window& window);

    /**
     * @brief Process SFML events from render window.
     * @param window SFML render window to poll events from
     */
    void process_events(sf::RenderWindow& window);

    /**
     * @brief Register callback for SFML event type.
     * @param type SFML event type (e.g., KEYPRESS_W, MOUSE_BUTTON_LEFT_PRESS)
     * @param callback Function to call when event occurs
     * @param listener_id Unique listener ID for deregistration
     * @return Registration ID for this specific callback
     */
    std::uint64_t register_callback(
        SFML_event_type const  type,
        SFML_callback   const& callback,
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
    void deregister_type(SFML_event_type const type);

    /**
     * @brief Clear all registered callbacks.
     */
    void clear();

private:
    SFML_event_manager();

    ~SFML_event_manager() = default;

    void _dispatch                  (SFML_event_type const type, SFML_event_data const& data);
    void _process_held_keys         (                                                       );
    void _process_held_mouse_buttons(                                                       );

    SFML_event_type _key_to_press_event  (sf::Keyboard::Key const key);
    SFML_event_type _key_to_held_event   (sf::Keyboard::Key const key);
    SFML_event_type _key_to_release_event(sf::Keyboard::Key const key);

    SFML_event_type _mouse_to_press_event  (sf::Mouse::Button const button);
    SFML_event_type _mouse_to_held_event   (sf::Mouse::Button const button);
    SFML_event_type _mouse_to_release_event(sf::Mouse::Button const button);

    struct Callback_entry {

        std::uint64_t callback_id;
        std::uint64_t listener_id;
        SFML_callback callback;
    };

    std::unordered_map<SFML_event_type, std::vector<Callback_entry>> _callbacks;
    std::unordered_map<sf::Keyboard::Key, bool>                      _held_keys;
    std::unordered_map<sf::Mouse::Button, bool>                      _held_mouse_buttons;

    std::uint64_t _next_callback_id;
};

} // namespace titan::events
