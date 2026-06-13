#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Spacer
// ----------------------------------------------------------------------------

/** @brief Invisible fixed-size gap for use inside layout containers. */
class Spacer final : public Widget {

public:
    /**
     * @brief Construct a Spacer.
     * @param name Widget name (unique within its parent)
     */
    explicit Spacer(std::string const& name);
};
} // namespace titan::ui
