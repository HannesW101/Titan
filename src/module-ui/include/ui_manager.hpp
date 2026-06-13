#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "module-ui/include/theme.hpp"

#include "SFML/System/Vector2.hpp"

#include <any>
#include <cstdint>

#include <memory>
#include <string>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::render    { class Renderer; }
namespace titan::resources { class Resource_manager; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class UI_manager
// ----------------------------------------------------------------------------

/**
 * @brief Owns the widget tree, the active Theme, focus and hover tracking, and
 * drives the per-frame layout/update/render passes.
 */
class UI_manager final {

public:
    /**
     * @brief Construct a UI_manager.
     * @param renderer renderer
     * @param rm rm
     */
    UI_manager(render::Renderer& renderer, resources::Resource_manager& rm);

    UI_manager(UI_manager const&)            = delete;
    UI_manager& operator=(UI_manager const&) = delete;
    UI_manager(UI_manager&&)                 = delete;
    UI_manager& operator=(UI_manager&&)      = delete;

    // Tree
    /**
     * @brief Add.
     * @param w w
     */
    Widget* add(std::unique_ptr<Widget> w);
    /**
     * @brief Find.
     * @param name Widget name (unique within its parent)
     */
    Widget* find(std::string const& name);
    /**
     * @brief Remove.
     * @param name Widget name (unique within its parent)
     */
    void    remove(std::string const& name);
    /**
     * @brief Clear.
     */
    void    clear();
    /**
     * @brief Get the root.
     * @return The root
     */
    Widget* root();

    // Theme
    /**
     * @brief Set the theme.
     * @param theme New theme
     */
    void         set_theme(Theme theme);
    /**
     * @brief Get the theme.
     * @return The theme
     */
    Theme&       theme();
    /**
     * @brief Get the theme.
     * @return The theme
     */
    Theme const& theme() const;
    /**
     * @brief Load theme.
     * @param config config
     */
    void         load_theme(core::Config const& config);

    // Focus
    /**
     * @brief Set the focused.
     * @param w New focused
     */
    void    set_focused(Widget* w);
    /**
     * @brief Get the focused.
     * @return The focused
     */
    Widget* focused() const;

    // Access
    /**
     * @brief Get the renderer.
     * @return The renderer
     */
    render::Renderer&            renderer();
    /**
     * @brief Get the resource manager.
     * @return The resource manager
     */
    resources::Resource_manager& resource_manager();
    /**
     * @brief Get the mouse position.
     * @return The mouse position
     */
    sf::Vector2f                 mouse_position() const;

    // ------------------------------------------------------------------------
    // Drag and drop: any widget can begin a drag carrying an arbitrary payload;
    // drop targets query the active payload on release. Payload is type-erased.
    // ------------------------------------------------------------------------
    /**
     * @brief Begin drag.
     * @param payload payload
     * @param kind kind
     */
    void begin_drag(std::any payload, std::int64_t kind = 0);
    /**
     * @brief Check whether dragging.
     * @return True if dragging
     */
    bool is_dragging() const;
    /**
     * @brief Get the drag kind.
     * @return The drag kind
     */
    std::int64_t drag_kind() const;
    /**
     * @brief Get the drag payload.
     * @return The drag payload
     */
    std::any const& drag_payload() const;
    /**
     * @brief End drag.
     */
    void end_drag();

    // Frame
    /**
     * @brief Layout.
     */
    void layout();
    /**
     * @brief Update.
     * @param dt dt
     */
    void update(float dt);
    /**
     * @brief Render.
     */
    void render();
    /**
     * @brief Set the mouse position.
     * @param p New mouse position
     */
    void set_mouse_position(sf::Vector2f p);

private:
    render::Renderer&            _renderer;
    resources::Resource_manager& _rm;
    Theme                        _theme;
    std::unique_ptr<Widget>      _root;
    Widget*                      _focused = nullptr;
    sf::Vector2f                 _mouse_pos = { 0.0f, 0.0f };

    bool         _dragging = false;
    std::int64_t _drag_kind = 0;
    std::any     _drag_payload;

    /**
     * @brief Clear hover.
     * @param w w
     */
    void    _clear_hover(Widget& w);
    /**
     * @brief Find hit.
     * @param w w
     */
    Widget* _find_hit(Widget& w);
};

} // namespace titan::ui
