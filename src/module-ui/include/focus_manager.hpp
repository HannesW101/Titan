#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "module-core/events/include/event_listener.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

class UI_manager;
class Widget;

// ============================================================================
// Class Focus_manager
// ----------------------------------------------------------------------------

/**
 * @brief Keyboard focus traversal for a UI_manager's widget tree.
 *
 * Collects focusable widgets (Widget::is_focusable(), visible + enabled) in
 * tree order and cycles focus with Tab / Shift+Tab, plus directional helpers.
 * It defers to the UI_manager for the actual focused-widget state so mouse
 * clicks and tab navigation stay in sync.
 *
 * Construct with the UI_manager; it registers Tab handlers automatically.
 *
 *   ui::Focus_manager focus(gui);   // now Tab cycles inputs in the UI
 */
class Focus_manager final : public events::Event_listener {

public:
    /**
     * @brief Construct a Focus_manager.
     * @param manager manager
     */
    explicit Focus_manager(UI_manager& manager);

    ~Focus_manager();

    Focus_manager           (Focus_manager const&) = delete;
    Focus_manager& operator=(Focus_manager const&) = delete;
    Focus_manager           (Focus_manager&&     ) = delete;
    Focus_manager& operator=(Focus_manager&&     ) = delete;

    /**
     * @brief Move focus to the next focusable widget (wraps around).
     */
    void focus_next();

    /**
     * @brief Move focus to the previous focusable widget (wraps around).
     */
    void focus_previous();

    /**
     * @brief Clear keyboard focus entirely.
     */
    void clear_focus();

    /**
     * @brief Whether Tab handling is active.
     */
    void set_enabled(bool enabled);
    /**
     * @brief Check whether enabled.
     * @return True if enabled
     */
    bool is_enabled() const;

private:
    UI_manager&   _manager;
    bool          _enabled       = true;
    std::uint64_t _tab_listener  = 0;
    std::uint64_t _shift_held_id = 0;
    bool          _shift_down    = false;

    /**
     * @brief Collect.
     * @param widget widget
     * @param out out
     */
    void _collect(Widget* widget, std::vector<Widget*>& out) const;
    /**
     * @brief Get the focusables.
     * @return The focusables
     */
    std::vector<Widget*> _focusables() const;
};

} // namespace titan::ui
