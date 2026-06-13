#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/ui_anchor.hpp"
#include "module-ui/include/ui_length.hpp"
#include "module-ui/include/visual.hpp"
#include "module-ui/include/visual_set.hpp"
#include "module-ui/include/widget_state.hpp"
#include "module-ui/include/draw_buffer.hpp"

#include "module-core/events/include/event_listener.hpp"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::render    { class Renderer; }
namespace titan::resources { class Resource_manager; }
namespace titan::ui        { class UI_manager; class Theme; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Enums
// ----------------------------------------------------------------------------

enum class Layout_mode : std::uint8_t { NONE, HORIZONTAL, VERTICAL, GRID };
enum class Text_align  : std::uint8_t { LEFT, CENTER, RIGHT };

// ============================================================================
// Class Widget
// ----------------------------------------------------------------------------

/**
 * @brief Base class for all named UI widgets.
 *
 * Provides the shared machinery every widget needs: tree structure, layout
 * (anchor + offset, px/% sizing, optional auto-arrange of children), per-state
 * appearance via a Visual_set, visibility/enable/hover/focus/press state, and
 * automatic registration/cleanup with the SFML event manager.
 *
 * Concrete widgets (Button, Slider, ...) derive from this, add their own typed
 * API (plain setter methods, no chaining), and override the protected hooks
 * on_update() / on_render() / on_layout(). They draw through draw_buffer() so
 * submitted drawables stay alive until the renderer flushes.
 *
 * Styling: a widget pulls its default Visual_set from the active Theme using
 * its style_key() (set by each subclass), then applies any per-widget overrides
 * the user supplied via set_visual()/set_text_style().
 */
class Widget : public events::Event_listener {

public:
    /**
     * @brief Widget.
     * @param name Widget name (unique within its parent)
     */
    explicit Widget(std::string name);
    ~Widget() override;

    Widget(Widget const&)            = delete;
    Widget& operator=(Widget const&) = delete;
    Widget(Widget&&)                 = delete;
    Widget& operator=(Widget&&)      = delete;

    // -------------------------------------------------------------------------
    // Identity / tree
    // -------------------------------------------------------------------------

    /**
     * @brief Get the name.
     * @return The name
     */
    std::string const& name() const;

    /// Add a child; returns raw pointer (ownership stays in the tree).
    /**
     * @brief Add child.
     * @param child child
     */
    Widget* add_child(std::unique_ptr<Widget> child);
    /**
     * @brief Remove child.
     * @param name Widget name (unique within its parent)
     */
    void    remove_child(std::string const& name);
    Widget* find(std::string const& name);          ///< recursive
    /**
     * @brief Get the children.
     * @return The children
     */
    std::vector<std::unique_ptr<Widget>> const& children() const;
    /**
     * @brief Get the parent.
     * @return The parent
     */
    Widget* parent() const;

    // -------------------------------------------------------------------------
    // Layout (plain setters)
    // -------------------------------------------------------------------------

    /**
     * @brief Set the anchor.
     * @param a New anchor
     */
    void set_anchor(UI_anchor a);
    /**
     * @brief Set the offset.
     * @param o New offset
     */
    void set_offset(sf::Vector2f o);
    /**
     * @brief Set the offset.
     * @param x x
     * @param y y
     */
    void set_offset(float x, float y);
    /**
     * @brief Set the width.
     * @param w New width
     */
    void set_width(UI_length w);
    /**
     * @brief Set the height.
     * @param h New height
     */
    void set_height(UI_length h);
    /**
     * @brief Set the size.
     * @param w w
     * @param h h
     */
    void set_size(UI_length w, UI_length h);
    /**
     * @brief Set the padding.
     * @param all New padding
     */
    void set_padding(float all);
    /**
     * @brief Set the padding.
     * @param top top
     * @param right right
     * @param bottom bottom
     * @param left left
     */
    void set_padding(float top, float right, float bottom, float left);

    /**
     * @brief Set the layout mode.
     * @param m New layout mode
     */
    void set_layout_mode(Layout_mode m);
    /**
     * @brief Set the spacing.
     * @param s New spacing
     */
    void set_spacing(float s);
    /**
     * @brief Set the grid columns.
     * @param c New grid columns
     */
    void set_grid_columns(std::size_t c);

    /**
     * @brief Get the anchor.
     * @return The anchor
     */
    UI_anchor     anchor() const;
    /**
     * @brief Get the offset.
     * @return The offset
     */
    sf::Vector2f  offset() const;
    /**
     * @brief Get the rect.
     * @return The rect
     */
    sf::FloatRect rect() const;
    /**
     * @brief Get the inner rect.
     * @return The inner rect
     */
    sf::FloatRect inner_rect() const;

    // -------------------------------------------------------------------------
    // Appearance overrides (per-widget, on top of Theme defaults)
    // -------------------------------------------------------------------------

    /**
     * @brief Set the visual.
     * @param state state
     * @param v v
     */
    void set_visual(Widget_state state, Visual v);
    void set_visual_all(Visual v);                   ///< set NORMAL (fallback for all)
    /**
     * @brief Set the text style.
     * @param state state
     * @param t t
     */
    void set_text_style(Widget_state state, Text_appearance t);
    /**
     * @brief Set the text style all.
     * @param t New text style all
     */
    void set_text_style_all(Text_appearance t);

    /// Replace the entire Visual_set for this widget.
    /**
     * @brief Set the visuals.
     * @param vs New visuals
     */
    void set_visuals(Visual_set vs);
    /**
     * @brief Get the visuals.
     * @return The visuals
     */
    Visual_set&       visuals();
    /**
     * @brief Get the visuals.
     * @return The visuals
     */
    Visual_set const& visuals() const;

    // -------------------------------------------------------------------------
    // State
    // -------------------------------------------------------------------------

    /**
     * @brief Set the visible.
     * @param v New visible
     */
    void set_visible(bool v);
    /**
     * @brief Check whether visible.
     * @return True if visible
     */
    bool is_visible() const;
    /**
     * @brief Set the enabled.
     * @param e New enabled
     */
    void set_enabled(bool e);
    /**
     * @brief Check whether enabled.
     * @return True if enabled
     */
    bool is_enabled() const;
    /**
     * @brief Check whether hovered.
     * @return True if hovered
     */
    bool is_hovered() const;
    /**
     * @brief Check whether pressed.
     * @return True if pressed
     */
    bool is_pressed() const;
    /**
     * @brief Check whether focused.
     * @return True if focused
     */
    bool is_focused() const;

    /**
     * @brief Request focus.
     */
    void request_focus();

    /**
     * @brief Whether this widget participates in keyboard focus traversal
     * (tab order). Inputs (text fields, sliders, buttons) set this true; static
     * widgets (labels, panels) leave it false. Default false.
     */
    bool is_focusable() const;
    /**
     * @brief Set the focusable.
     * @param focusable New focusable
     */
    void set_focusable(bool focusable);

    /**
     * @brief Get the current state.
     * @return The current state
     */
    Widget_state current_state() const;

    /// Draw to UI_OVERLAY layer instead of UI (for popups, modals, tooltips).
    /**
     * @brief Set the "on overlay" callback.
     * @param cb Callback to invoke
     */
    void set_on_overlay(bool on);
    /**
     * @brief Handle the overlay event.
     */
    bool on_overlay() const;

    // -------------------------------------------------------------------------
    // Per-frame (driven by UI_manager)
    // -------------------------------------------------------------------------

    /**
     * @brief Do layout.
     * @param parent_rect parent rect
     */
    void do_layout(sf::FloatRect const& parent_rect);
    /**
     * @brief Update.
     * @param dt dt
     */
    void update(float dt);
    /**
     * @brief Render.
     * @param renderer renderer
     */
    void render(render::Renderer& renderer);

    // -------------------------------------------------------------------------
    // Manager access
    // -------------------------------------------------------------------------

    /**
     * @brief Get the ui manager.
     * @return The ui manager
     */
    UI_manager&                  ui_manager();
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

protected:
    // ---- Subclass hooks -----------------------------------------------------

    /// Called after this widget's rect is resolved, before children layout.
    /**
     * @brief Handle the layout event.
     */
    virtual void on_layout() {}
    /// Per-frame logic.
    /**
     * @brief Handle the update event.
     * @param dt dt
     */
    virtual void on_update(float dt) { (void)dt; }
    /// Draw widget-specific content (background is drawn by base first).
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    virtual void on_render(render::Renderer& renderer) { (void)renderer; }

    /// Theme key for default styling (e.g. "button", "label"). Empty = none.
    virtual std::string style_key() const { return {}; }

    /// If false, the base does not auto-draw the background visual; the widget
    /// draws everything itself in on_render (used by widgets that scale/clip).
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    virtual bool draws_own_background() const { return false; }

    // ---- Helpers for subclasses --------------------------------------------

    /**
     * @brief Get the draw buffer.
     * @return The draw buffer
     */
    Draw_buffer& draw_buffer();

    /// Draw the resolved background visual for the current state into `dst`.
    /**
     * @brief Draw background.
     * @param renderer renderer
     * @param dst dst
     */
    void draw_background(render::Renderer& renderer, sf::FloatRect const& dst);

    /// Draw text centered/aligned within `dst` using the current state style.
    void draw_text(
        render::Renderer& renderer,
        sf::FloatRect const& dst,
        std::string const& text,
        Text_align align = Text_align::CENTER,
        sf::Vector2f extra_offset = { 0.0f, 0.0f }
        );

    /// Resolve the Visual to draw for the current state (theme + overrides).
    /**
     * @brief Get the resolved visual.
     * @return The resolved visual
     */
    Visual const&          resolved_visual() const;
    /**
     * @brief Get the resolved text style.
     * @return The resolved text style
     */
    Text_appearance const& resolved_text_style() const;

    /// Register the standard press/release click handlers (idempotent). Sets
    /// _pressed and invokes the protected on_pressed()/on_released()/on_clicked.
    /**
     * @brief Enable click events.
     */
    void enable_click_events();

    /// Click hooks for subclasses (only fire if enable_click_events called).
    /**
     * @brief Handle the pressed event.
     */
    virtual void on_pressed()  {}
    /**
     * @brief Handle the released event.
     */
    virtual void on_released() {}
    virtual void on_clicked()  {}   ///< press + release while hovered

    // state flags accessible to subclasses
    /**
     * @brief Set the pressed.
     * @param p New pressed
     */
    void set_pressed(bool p) { _pressed = p; }

private:
    friend class UI_manager;

    struct Padding { float top = 0, right = 0, bottom = 0, left = 0; };

    std::string _name;
    Widget*     _parent = nullptr;
    std::vector<std::unique_ptr<Widget>> _children;

    // Layout
    UI_anchor    _anchor = UI_anchor::TOP_LEFT;
    sf::Vector2f _offset = { 0.0f, 0.0f };
    UI_length    _width  = UI_length::px(100.0f);
    UI_length    _height = UI_length::px(30.0f);
    Padding      _pad;
    Layout_mode  _layout_mode  = Layout_mode::NONE;
    float        _spacing      = 4.0f;
    std::size_t  _grid_columns = 1u;
    sf::FloatRect _rect;

    // Appearance: per-widget overrides merged over theme defaults
    Visual_set _overrides;
    bool       _has_override_visual[static_cast<std::size_t>(Widget_state::COUNT)] = {};
    bool       _has_override_text[static_cast<std::size_t>(Widget_state::COUNT)]   = {};
    mutable Visual_set _resolved_cache;   ///< theme merged with overrides
    mutable bool       _resolved_dirty = true;

    // State
    bool _visible = true;
    bool _enabled = true;
    bool _hovered = false;
    bool _pressed = false;
    bool _focused = false;
    bool _focusable = false;
    bool _on_overlay = false;

    // Click plumbing
    bool          _click_registered = false;
    std::uint64_t _press_cb   = 0u;
    std::uint64_t _release_cb = 0u;

    UI_manager* _ui_manager = nullptr;
    Draw_buffer _draw_buffer;

    /**
     * @brief Resolve anchor.
     * @param parent parent
     */
    void _resolve_anchor(sf::FloatRect const& parent);
    /**
     * @brief Arrange children.
     */
    void _arrange_children();
    /**
     * @brief Propagate manager.
     * @param child child
     */
    void _propagate_manager(Widget& child);
    /**
     * @brief Rebuild resolved.
     */
    void _rebuild_resolved() const;
    /**
     * @brief Get the layer.
     * @return The layer
     */
    render::Render_layer _layer() const;
};

} // namespace titan::ui
