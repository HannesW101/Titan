#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/visual_set.hpp"

#include "module-core/config/include/config.hpp"

#include <string>
#include <unordered_map>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Theme
// ----------------------------------------------------------------------------

/**
 * @brief Holds default Visual_sets keyed by widget style name ("button",
 * "label", "panel", "slider_track", ...).
 *
 * Widgets look up their defaults via style_key(); users override per-widget on
 * top. Themes can be built in code or loaded from a TOML config (sections named
 * "ui.<key>") using your Config system.
 *
 * A built-in dark theme is provided via Theme::dark() so the UI looks
 * reasonable out of the box with zero setup.
 */
class Theme final {

public:
    Theme() = default;

    /// Register / replace the style for a key.
    /**
     * @brief Set.
     * @param key key
     * @param vs vs
     */
    void set(std::string const& key, Visual_set vs);

    /// Look up a style. Returns an empty default if missing.
    /**
     * @brief Get.
     * @param key key
     */
    Visual_set const& get(std::string const& key) const;
    /**
     * @brief Has.
     * @param key key
     */
    bool has(std::string const& key) const;

    /// Load styles from TOML config. Sections "ui.<key>" with per-state props:
    ///   <state>_bg, <state>_border, <state>_border_thickness,
    ///   <state>_text, <state>_text_size, <state>_font,
    ///   <state>_texture, <state>_nine (+ <state>_nine_border)
    /// where <state> is normal|hovered|pressed|focused|disabled.
    /**
     * @brief Load from config.
     * @param config config
     */
    void load_from_config(core::Config const& config);

    /// Built-in dark theme covering all standard widget keys.
    /**
     * @brief Get the dark.
     * @return The dark
     */
    static Theme dark();

private:
    std::unordered_map<std::string, Visual_set> _styles;
    Visual_set _empty;

    /**
     * @brief Parse color.
     * @param hex hex
     */
    static sf::Color parse_color(std::string const& hex);
};

} // namespace titan::ui
