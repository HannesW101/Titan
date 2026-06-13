#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Class Event_listener
// ----------------------------------------------------------------------------

/**
 * @brief Base class for objects that listen to events.
 *
 * Automatically generates unique listener IDs for event deregistration.
 */
class Event_listener {

public:
    virtual ~Event_listener() = default;

protected:
    /**
     * @brief Construct listener with unique ID.
     */
    Event_listener()
        : _listener_id(_generate_id())
    {}

    /**
     * @brief Get this listener's unique ID.
     * @return Listener ID
     */
    std::uint64_t listener_id() const { return _listener_id; }

private:
    /**
     * @brief Generate unique listener ID.
     * @return New unique ID
     */
    static std::uint64_t _generate_id() {

        static std::uint64_t id = 0u;
        return id++;
    }

    std::uint64_t _listener_id; ///< Unique listener identifier
};

} // namespace titan::events
