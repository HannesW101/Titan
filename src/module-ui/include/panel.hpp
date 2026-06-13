#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Panel
// ----------------------------------------------------------------------------

/** @brief Styled container. Use layout modes for auto-arrangement. */
class Panel final : public Widget {

public:
    /**
     * @brief Construct a Panel.
     * @param name Widget name (unique within its parent)
     */
    explicit Panel(std::string const& name);

protected:
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "panel"; }
};
} // namespace titan::ui
