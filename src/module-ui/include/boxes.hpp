#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class H_box
// ----------------------------------------------------------------------------

/** @brief Container that arranges children left-to-right. */
class H_box final : public Widget {

public:
    explicit H_box(std::string const& name);
};

// ============================================================================
// Class V_box
// ----------------------------------------------------------------------------

/** @brief Container that arranges children top-to-bottom. */
class V_box final : public Widget {

public:
    explicit V_box(std::string const& name);
};

// ============================================================================
// Class Grid
// ----------------------------------------------------------------------------

/** @brief Container that arranges children in a grid (set columns). */
class Grid final : public Widget {

public:
    /**
     * @brief Construct a Grid.
     * @param name Widget name (unique within its parent)
     */
    explicit Grid(std::string const& name);
    /**
     * @brief Set the columns.
     * @param c New columns
     */
    void set_columns(std::size_t c);
};
} // namespace titan::ui
