// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/ui_manager.hpp"

#include "module-render/include/renderer.hpp"

#include "module-utils/include/math.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;

UI_manager::UI_manager(render::Renderer& renderer, resources::Resource_manager& rm)
    : _renderer(renderer)
    , _rm(rm)
    , _theme(Theme::dark())
    , _root(std::make_unique<Widget>("__root__"))
{
    _root->_ui_manager = this;
}

Widget* UI_manager::add(std::unique_ptr<Widget> w) {
    w->_ui_manager = this;
    return _root->add_child(std::move(w));
}

Widget* UI_manager::find(std::string const& name) { return _root->find(name); }
void    UI_manager::remove(std::string const& name) { _root->remove_child(name); }

void UI_manager::clear() {
    _focused = nullptr;
    _root = std::make_unique<Widget>("__root__");
    _root->_ui_manager = this;
}

Widget* UI_manager::root() { return _root.get(); }

void         UI_manager::set_theme(Theme theme) { _theme = std::move(theme); }
Theme&       UI_manager::theme()                { return _theme; }
Theme const& UI_manager::theme() const          { return _theme; }
void         UI_manager::load_theme(core::Config const& config) { _theme.load_from_config(config); }

void UI_manager::set_focused(Widget* w) {
    if (_focused) { _focused->_focused = false; }
    _focused = w;
    if (_focused) { _focused->_focused = true; }
}
Widget* UI_manager::focused() const { return _focused; }

render::Renderer&            UI_manager::renderer()         { return _renderer; }
resources::Resource_manager& UI_manager::resource_manager() { return _rm; }
sf::Vector2f                 UI_manager::mouse_position() const { return _mouse_pos; }

void UI_manager::layout() {
    auto const& window = _renderer.window();
    sf::Vector2u const sz = window.getSize();
    sf::FloatRect const screen({ 0.0f, 0.0f },
        { static_cast<float>(sz.x), static_cast<float>(sz.y) });
    _root->_rect = screen;
    for (auto const& c : _root->children()) { c->do_layout(screen); }
}

void UI_manager::update(float dt) {
    _clear_hover(*_root);
    if (auto* hit = _find_hit(*_root)) { hit->_hovered = true; }
    _root->update(dt);
}

void UI_manager::render() { _root->render(_renderer); }
void UI_manager::set_mouse_position(sf::Vector2f p) { _mouse_pos = p; }


void UI_manager::begin_drag(std::any payload, std::int64_t kind) {
    _dragging = true; _drag_kind = kind; _drag_payload = std::move(payload);
}
bool UI_manager::is_dragging() const { return _dragging; }
std::int64_t UI_manager::drag_kind() const { return _drag_kind; }
std::any const& UI_manager::drag_payload() const { return _drag_payload; }
void UI_manager::end_drag() { _dragging = false; _drag_kind = 0; _drag_payload.reset(); }

void UI_manager::_clear_hover(Widget& w) {
    w._hovered = false;
    for (auto const& c : w.children()) { _clear_hover(*c); }
}

Widget* UI_manager::_find_hit(Widget& w) {
    if (!w._visible || !w._enabled) { return nullptr; }
    for (auto it = w.children().rbegin(); it != w.children().rend(); ++it) {
        if (auto* hit = _find_hit(**it)) { return hit; }
    }
    if (w._parent != nullptr && contains(w.rect(), _mouse_pos)) { return &w; }
    return nullptr;
}

} // namespace titan::ui
