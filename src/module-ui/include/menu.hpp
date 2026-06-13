#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Menu_bar
// ----------------------------------------------------------------------------

/** @brief Horizontal menu bar with top-level entries. Each entry fires a callback.
 *  For nested submenus combine with a Context_menu. */
class Menu_bar final : public Widget {

public:
// ============================================================================
// Class Entry
// ----------------------------------------------------------------------------

    struct Entry { std::string label; std::function<void()> action; };

    /**
     * @brief Construct a Menu_bar.
     * @param name Widget name (unique within its parent)
     */
    explicit Menu_bar(std::string const& name);
    /**
     * @brief Add entry.
     * @param label label
     */
    void add_entry(std::string const& label, std::function<void()> action);
    /**
     * @brief Clear.
     */
    void clear();

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "menu"; }

private:
    std::vector<Entry> _entries;
    std::uint64_t      _click=0;
};

// ============================================================================
// Class Context_menu
// ----------------------------------------------------------------------------

/** @brief A popup list of clickable entries shown at a position. */
class Context_menu final : public Widget {

public:
// ============================================================================
// Class Entry
// ----------------------------------------------------------------------------

    struct Entry { std::string label; std::function<void()> action; };
    /**
     * @brief Construct a Context_menu.
     * @param name Widget name (unique within its parent)
     */
    explicit Context_menu(std::string const& name);
    /**
     * @brief Add entry.
     * @param label label
     */
    void add_entry(std::string const& label, std::function<void()> action);
    /**
     * @brief Clear.
     */
    void clear();
    /**
     * @brief Show at.
     * @param pos pos
     */
    void show_at(sf::Vector2f pos);
    /**
     * @brief Hide.
     */
    void hide();
    /**
     * @brief Check whether shown.
     * @return True if shown
     */
    bool is_shown() const;

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "menu"; }

private:
    std::vector<Entry> _entries;
    bool _shown = false;
    sf::Vector2f _pos{0,0};
    float _row_h = 24.f;
    std::uint64_t _click = 0;
};

} // namespace titan::ui
