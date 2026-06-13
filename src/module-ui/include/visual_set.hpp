#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/visual.hpp"
#include "module-ui/include/widget_state.hpp"

#include "SFML/Graphics/Color.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Struct Text_appearance
// ----------------------------------------------------------------------------

/**
 * @brief How text is drawn for a widget, per state where desired.
 */
struct Text_appearance {
    std::string font_name;               ///< Resource_manager key, empty = "default_font"
    float       size       = 14.0f;
    sf::Color   color      = sf::Color::White;
};

// ============================================================================
// Struct Visual_set
// ----------------------------------------------------------------------------

/**
 * @brief A complete appearance for a widget across all interaction states.
 *
 * Holds one Visual per state (NORMAL is the fallback for any unset state) and
 * a text appearance per state. This is what makes every widget fully skinnable
 * without touching widget code: assign Visuals for each state and you are done.
 *
 * Any state left unset falls back to NORMAL, so the common case (only setting
 * NORMAL + HOVERED) just works.
 */
struct Visual_set {

    std::array<std::optional<Visual>, static_cast<std::size_t>(Widget_state::COUNT)> visuals;
    std::array<std::optional<Text_appearance>, static_cast<std::size_t>(Widget_state::COUNT)> text;

    float transition_seconds = 0.1f;   ///< Cross-fade time between states

    // ---- Setters -----------------------------------------------------------

    Visual_set& set_visual(Widget_state state, Visual v) {
        visuals[static_cast<std::size_t>(state)] = std::move(v);
        return *this;
    }

    Visual_set& set_text(Widget_state state, Text_appearance t) {
        text[static_cast<std::size_t>(state)] = std::move(t);
        return *this;
    }

    /// Convenience: set the same visual on NORMAL.
    /**
     * @brief Normal.
     * @param v v
     */
    Visual_set& normal(Visual v)   { return set_visual(Widget_state::NORMAL, std::move(v)); }
    /**
     * @brief Hovered.
     * @param v v
     */
    Visual_set& hovered(Visual v)  { return set_visual(Widget_state::HOVERED, std::move(v)); }
    /**
     * @brief Pressed.
     * @param v v
     */
    Visual_set& pressed(Visual v)  { return set_visual(Widget_state::PRESSED, std::move(v)); }
    /**
     * @brief Focused.
     * @param v v
     */
    Visual_set& focused(Visual v)  { return set_visual(Widget_state::FOCUSED, std::move(v)); }
    /**
     * @brief Disabled.
     * @param v v
     */
    Visual_set& disabled(Visual v) { return set_visual(Widget_state::DISABLED, std::move(v)); }

    // ---- Resolution (with NORMAL fallback) ---------------------------------

    Visual const& visual_for(Widget_state state) const {
        auto const& slot = visuals[static_cast<std::size_t>(state)];
        /**
         * @brief If.
         */
        if (slot.has_value()) { return *slot; }
        auto const& norm = visuals[static_cast<std::size_t>(Widget_state::NORMAL)];
        /**
         * @brief If.
         */
        if (norm.has_value()) { return *norm; }
        /**
         * @brief Get the empty visual.
         * @return The empty visual
         */
        return _empty_visual();
    }

    Text_appearance const& text_for(Widget_state state) const {
        auto const& slot = text[static_cast<std::size_t>(state)];
        /**
         * @brief If.
         */
        if (slot.has_value()) { return *slot; }
        auto const& norm = text[static_cast<std::size_t>(Widget_state::NORMAL)];
        /**
         * @brief If.
         */
        if (norm.has_value()) { return *norm; }
        /**
         * @brief Get the empty text.
         * @return The empty text
         */
        return _empty_text();
    }

    bool has_text() const {
        for (auto const& t : text) { if (t.has_value()) { return true; } }
        return false;
    }

private:
    static Visual const& _empty_visual() {
        static Visual const v = Visual::none();
        return v;
    }
    static Text_appearance const& _empty_text() {
        static Text_appearance const t{};
        return t;
    }
};

} // namespace titan::ui
