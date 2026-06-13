#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics.hpp"

#include <any>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Structs
// ----------------------------------------------------------------------------

/**
 * @brief Data associated with SFML events.
 */
struct SFML_event_data final {

    sf::Event sfml_event; ///< Original SFML event
};

/**
 * @brief Data associated with custom game events.
 */
struct Game_event_data final {

    std::any payload; ///< Custom payload (can be any type)
};

} // namespace titan::events
