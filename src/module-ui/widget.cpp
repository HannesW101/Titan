// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/theme.hpp"
#include "module-ui/include/visual_renderer.hpp"

#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

#include "module-resources/include/resource_manager.hpp"

#include "module-core/events/include/sfml_event_manager.hpp"

#include "module-utils/include/math.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;

// ============================================================================
// Construction
// ============================================================================

Widget::Widget(std::string name)
    : _name(std::move(name))
{}

Widget::~Widget() {
    events::SFML_event_manager::instance().deregister_listener(listener_id());
}

// ============================================================================
// Tree
// ============================================================================

std::string const& Widget::name() const { return _name; }

Widget* Widget::add_child(std::unique_ptr<Widget> child) {
    Widget* raw = child.get();
    child->_parent = this;
    _propagate_manager(*child);
    _children.push_back(std::move(child));
    return raw;
}

void Widget::remove_child(std::string const& name) {
    auto it = std::remove_if(_children.begin(), _children.end(),
        [&](std::unique_ptr<Widget> const& c){ return c->name() == name; });
    _children.erase(it, _children.end());
}

Widget* Widget::find(std::string const& name) {
    if (_name == name) { return this; }
    for (auto const& c : _children) {
        if (auto* f = c->find(name)) { return f; }
    }
    return nullptr;
}

std::vector<std::unique_ptr<Widget>> const& Widget::children() const { return _children; }
Widget* Widget::parent() const { return _parent; }

void Widget::_propagate_manager(Widget& child) {
    child._ui_manager = _ui_manager;
    for (auto const& g : child._children) { child._propagate_manager(*g); }
}

// ============================================================================
// Layout setters
// ============================================================================

void Widget::set_anchor(UI_anchor a)            { _anchor = a; }
void Widget::set_offset(sf::Vector2f o)         { _offset = o; }
void Widget::set_offset(float x, float y)       { _offset = { x, y }; }
void Widget::set_width(UI_length w)             { _width = w; }
void Widget::set_height(UI_length h)            { _height = h; }
void Widget::set_size(UI_length w, UI_length h) { _width = w; _height = h; }
void Widget::set_padding(float a)               { _pad = { a, a, a, a }; }
void Widget::set_padding(float t, float r, float b, float l) { _pad = { t, r, b, l }; }
void Widget::set_layout_mode(Layout_mode m)     { _layout_mode = m; }
void Widget::set_spacing(float s)               { _spacing = s; }
void Widget::set_grid_columns(std::size_t c)    { _grid_columns = std::max<std::size_t>(1u, c); }

UI_anchor    Widget::anchor() const  { return _anchor; }
sf::Vector2f Widget::offset() const  { return _offset; }
sf::FloatRect Widget::rect() const   { return _rect; }

float Widget::px_scale() const {
    return _ui_manager ? _ui_manager->ui_scale() : 1.0f;
}

sf::FloatRect Widget::inner_rect() const {
    float const s = px_scale();
    return sf::FloatRect(
        { _rect.position.x + _pad.left * s, _rect.position.y + _pad.top * s },
        { _rect.size.x - (_pad.left + _pad.right) * s,
          _rect.size.y - (_pad.top  + _pad.bottom) * s }
    );
}

// ============================================================================
// Appearance overrides
// ============================================================================

void Widget::set_visual(Widget_state state, Visual v) {
    _overrides.set_visual(state, std::move(v));
    _has_override_visual[static_cast<std::size_t>(state)] = true;
    _resolved_dirty = true;
}

void Widget::set_visual_all(Visual v) {
    set_visual(Widget_state::NORMAL, std::move(v));
}

void Widget::set_text_style(Widget_state state, Text_appearance t) {
    _overrides.set_text(state, std::move(t));
    _has_override_text[static_cast<std::size_t>(state)] = true;
    _resolved_dirty = true;
}

void Widget::set_text_style_all(Text_appearance t) {
    set_text_style(Widget_state::NORMAL, std::move(t));
}

void Widget::set_visuals(Visual_set vs) {
    _overrides = std::move(vs);
    for (std::size_t i = 0; i < static_cast<std::size_t>(Widget_state::COUNT); ++i) {
        _has_override_visual[i] = _overrides.visuals[i].has_value();
        _has_override_text[i]   = _overrides.text[i].has_value();
    }
    _resolved_dirty = true;
}

Visual_set&       Widget::visuals()       { _resolved_dirty = true; return _overrides; }
Visual_set const& Widget::visuals() const { return _overrides; }

// ============================================================================
// Resolution: merge theme defaults with per-widget overrides
// ============================================================================

void Widget::_rebuild_resolved() const {

    Visual_set const* theme_vs = nullptr;
    if (_ui_manager) {
        std::string const key = style_key();
        if (!key.empty() && _ui_manager->theme().has(key)) {
            theme_vs = &_ui_manager->theme().get(key);
        }
    }

    _resolved_cache = Visual_set{};

    for (std::size_t i = 0; i < static_cast<std::size_t>(Widget_state::COUNT); ++i) {
        // visuals
        if (_has_override_visual[i]) {
            _resolved_cache.visuals[i] = _overrides.visuals[i];
        } else if (theme_vs && theme_vs->visuals[i].has_value()) {
            _resolved_cache.visuals[i] = theme_vs->visuals[i];
        }
        // text
        if (_has_override_text[i]) {
            _resolved_cache.text[i] = _overrides.text[i];
        } else if (theme_vs && theme_vs->text[i].has_value()) {
            _resolved_cache.text[i] = theme_vs->text[i];
        }
    }

    _resolved_dirty = false;
}

Visual const& Widget::resolved_visual() const {
    if (_resolved_dirty) { _rebuild_resolved(); }
    return _resolved_cache.visual_for(current_state());
}

Text_appearance const& Widget::resolved_text_style() const {
    if (_resolved_dirty) { _rebuild_resolved(); }
    return _resolved_cache.text_for(current_state());
}

// ============================================================================
// State
// ============================================================================

void Widget::set_visible(bool v) { _visible = v; }
bool Widget::is_visible() const  { return _visible; }
void Widget::set_enabled(bool e) { _enabled = e; }
bool Widget::is_enabled() const  { return _enabled; }
bool Widget::is_hovered() const  { return _hovered; }
bool Widget::is_pressed() const  { return _pressed; }
bool Widget::is_focused() const  { return _focused; }
bool Widget::is_focusable() const          { return _focusable; }
void Widget::set_focusable(bool focusable) { _focusable = focusable; }

void Widget::request_focus() { if (_ui_manager) { _ui_manager->set_focused(this); } }

void Widget::set_on_overlay(bool on) { _on_overlay = on; }
bool Widget::on_overlay() const      { return _on_overlay; }

Widget_state Widget::current_state() const {
    if (!_enabled) { return Widget_state::DISABLED; }
    if (_pressed)  { return Widget_state::PRESSED; }
    if (_hovered)  { return Widget_state::HOVERED; }
    if (_focused)  { return Widget_state::FOCUSED; }
    return Widget_state::NORMAL;
}

// ============================================================================
// Click plumbing
// ============================================================================

void Widget::enable_click_events() {
    if (_click_registered) { return; }
    _click_registered = true;

    auto& sfml = events::SFML_event_manager::instance();

    _press_cb = sfml.register_callback(
        events::SFML_event_type::MOUSE_BUTTON_LEFT_PRESS,
        [this](events::SFML_event_data const&) {
            if (!_enabled || !_visible || !_hovered) { return; }
            _pressed = true;
            on_pressed();
        },
        listener_id()
    );

    _release_cb = sfml.register_callback(
        events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&) {
            if (!_enabled || !_visible) { _pressed = false; return; }
            bool const was = _pressed;
            _pressed = false;
            on_released();
            if (was && _hovered) { on_clicked(); }
        },
        listener_id()
    );
}

// ============================================================================
// Layout pass
// ============================================================================

void Widget::_resolve_anchor(sf::FloatRect const& parent) {

    float const s = px_scale();
    float const w = _width.resolve(parent.size.x, s);
    float const h = _height.resolve(parent.size.y, s);

    sf::Vector2f base = parent.position;
    switch (_anchor) {
        case UI_anchor::TOP_LEFT:                                      break;
        case UI_anchor::TOP_CENTER:    base.x += parent.size.x*0.5f;   break;
        case UI_anchor::TOP_RIGHT:     base.x += parent.size.x;        break;
        case UI_anchor::MIDDLE_LEFT:   base.y += parent.size.y*0.5f;   break;
        case UI_anchor::MIDDLE_CENTER: base.x += parent.size.x*0.5f;
                                       base.y += parent.size.y*0.5f;   break;
        case UI_anchor::MIDDLE_RIGHT:  base.x += parent.size.x;
                                       base.y += parent.size.y*0.5f;   break;
        case UI_anchor::BOTTOM_LEFT:   base.y += parent.size.y;        break;
        case UI_anchor::BOTTOM_CENTER: base.x += parent.size.x*0.5f;
                                       base.y += parent.size.y;        break;
        case UI_anchor::BOTTOM_RIGHT:  base.x += parent.size.x;
                                       base.y += parent.size.y;        break;
        default: break;
    }

    sf::Vector2f origin{ 0.0f, 0.0f };
    switch (_anchor) {
        case UI_anchor::TOP_CENTER: case UI_anchor::MIDDLE_CENTER:
        case UI_anchor::BOTTOM_CENTER: origin.x = w*0.5f; break;
        case UI_anchor::TOP_RIGHT: case UI_anchor::MIDDLE_RIGHT:
        case UI_anchor::BOTTOM_RIGHT:  origin.x = w;      break;
        default: break;
    }
    switch (_anchor) {
        case UI_anchor::MIDDLE_LEFT: case UI_anchor::MIDDLE_CENTER:
        case UI_anchor::MIDDLE_RIGHT:  origin.y = h*0.5f; break;
        case UI_anchor::BOTTOM_LEFT: case UI_anchor::BOTTOM_CENTER:
        case UI_anchor::BOTTOM_RIGHT:  origin.y = h;      break;
        default: break;
    }

    _rect = sf::FloatRect(
        { base.x + _offset.x * s - origin.x, base.y + _offset.y * s - origin.y },
        { w, h }
    );
}

void Widget::_arrange_children() {

    float const s = px_scale();

    if (_layout_mode == Layout_mode::NONE) {
        sf::FloatRect const in = inner_rect();
        for (auto const& c : _children) { c->do_layout(in); }
        return;
    }

    sf::FloatRect const in = inner_rect();
    float x = in.position.x, y = in.position.y;

    if (_layout_mode == Layout_mode::HORIZONTAL) {
        for (auto const& c : _children) {
            if (!c->_visible) { continue; }
            float const cw = c->_width.resolve(in.size.x, s);
            float const ch = c->_height.resolve(in.size.y, s);
            c->_rect = sf::FloatRect({ x, y + c->_offset.y * s }, { cw, ch });
            c->_arrange_children();
            x += cw + _spacing * s;
        }
    } else if (_layout_mode == Layout_mode::VERTICAL) {
        for (auto const& c : _children) {
            if (!c->_visible) { continue; }
            float const cw = c->_width.resolve(in.size.x, s);
            float const ch = c->_height.resolve(in.size.y, s);
            c->_rect = sf::FloatRect({ x + c->_offset.x * s, y }, { cw, ch });
            c->_arrange_children();
            y += ch + _spacing * s;
        }
    } else { // GRID
        std::size_t col = 0u; float row_h = 0.0f; float cx = in.position.x, cy = in.position.y;
        for (auto const& c : _children) {
            if (!c->_visible) { continue; }
            float const cw = c->_width.resolve(in.size.x, s);
            float const ch = c->_height.resolve(in.size.y, s);
            c->_rect = sf::FloatRect({ cx, cy }, { cw, ch });
            c->_arrange_children();
            cx += cw + _spacing * s;
            row_h = std::max(row_h, ch);
            if (++col >= _grid_columns) { col = 0u; cx = in.position.x; cy += row_h + _spacing * s; row_h = 0.0f; }
        }
    }
}

void Widget::do_layout(sf::FloatRect const& parent_rect) {
    if (!_visible) { return; }
    _resolve_anchor(parent_rect);
    on_layout();
    _arrange_children();
}

// ============================================================================
// Update / render
// ============================================================================

void Widget::update(float dt) {
    if (!_visible) { return; }
    on_update(dt);
    for (auto const& c : _children) { c->update(dt); }
}

render::Render_layer Widget::_layer() const {
    return _on_overlay ? render::Render_layer::UI_OVERLAY : render::Render_layer::UI;
}

void Widget::render(render::Renderer& renderer) {
    if (!_visible) { return; }
    _draw_buffer.clear();

    if (!draws_own_background()) {
        draw_background(renderer, _rect);
    }
    on_render(renderer);

    for (auto const& c : _children) { c->render(renderer); }
}

// ============================================================================
// Draw helpers
// ============================================================================

Draw_buffer& Widget::draw_buffer() { return _draw_buffer; }

void Widget::draw_background(render::Renderer& renderer, sf::FloatRect const& dst) {
    Visual const& vis = resolved_visual();
    visual_renderer::draw(vis, dst, renderer, _draw_buffer, resource_manager(), _layer());
}

void Widget::draw_text(
    render::Renderer& renderer,
    sf::FloatRect const& dst,
    std::string const& text,
    Text_align align,
    sf::Vector2f extra_offset
    ) {

    if (text.empty()) { return; }

    Text_appearance const& ta = resolved_text_style();

    std::shared_ptr<sf::Font> font;
    try {
        font = resource_manager().get<sf::Font>(ta.font_name.empty() ? "default_font" : ta.font_name);
    } catch (std::exception const&) { return; }

    float const s = px_scale();
    sf::Text t(*font, text, static_cast<unsigned int>(ta.size * s));
    t.setFillColor(ta.color);
    sf::FloatRect const tb = t.getLocalBounds();

    float x = dst.position.x;
    switch (align) {
        case Text_align::LEFT:   x += 6.0f - tb.position.x; break;
        case Text_align::CENTER: x += (dst.size.x - tb.size.x) * 0.5f - tb.position.x; break;
        case Text_align::RIGHT:  x += dst.size.x - tb.size.x - 6.0f - tb.position.x; break;
        default: break;
    }
    float const y = dst.position.y + (dst.size.y - tb.size.y) * 0.5f - tb.position.y;

    t.setPosition({ x + extra_offset.x, y + extra_offset.y });
    _draw_buffer.add_text(renderer, _layer(), std::move(t));
}

// ============================================================================
// Manager access
// ============================================================================

UI_manager& Widget::ui_manager() {
    if (!_ui_manager) { throw std::runtime_error("Widget not attached: " + _name); }
    return *_ui_manager;
}
resources::Resource_manager& Widget::resource_manager() { return ui_manager().resource_manager(); }
sf::Vector2f Widget::mouse_position() const {
    return _ui_manager ? _ui_manager->mouse_position() : sf::Vector2f{ 0.0f, 0.0f };
}

} // namespace titan::ui
