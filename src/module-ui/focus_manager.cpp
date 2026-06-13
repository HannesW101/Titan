// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/focus_manager.hpp"
#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/widget.hpp"

#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-core/events/include/event_type.hpp"

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// A fixed listener id base for the focus manager's handlers.
Focus_manager::Focus_manager(UI_manager& manager)
    : _manager(manager)
{
    auto& sfml = events::SFML_event_manager::instance();
    std::uint64_t const id = listener_id();

    _tab_listener = sfml.register_callback(
        events::SFML_event_type::KEYPRESS_TAB,
        [this](events::SFML_event_data const&) {
            if (!_enabled) { return; }
            // Shift state read live from the keyboard for Shift+Tab.
            bool const shift =
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
            if (shift) { focus_previous(); }
            else       { focus_next(); }
        },
        id
    );
}

Focus_manager::~Focus_manager() {
    events::SFML_event_manager::instance().deregister_listener(listener_id());
}

// ----------------------------------------------------------------------------
void Focus_manager::_collect(Widget* widget, std::vector<Widget*>& out) const {
    if (!widget) { return; }
    if (!widget->is_visible()) { return; }   // hidden subtrees skip focus
    if (widget->is_focusable() && widget->is_enabled()) {
        out.push_back(widget);
    }
    for (auto const& child : widget->children()) {
        _collect(child.get(), out);
    }
}

std::vector<Widget*> Focus_manager::_focusables() const {
    std::vector<Widget*> out;
    _collect(_manager.root(), out);
    return out;
}

// ----------------------------------------------------------------------------
void Focus_manager::focus_next() {
    auto const list = _focusables();
    if (list.empty()) { return; }

    Widget* current = _manager.focused();
    std::size_t start = 0u;
    for (std::size_t i = 0; i < list.size(); ++i) {
        if (list[i] == current) { start = i + 1u; break; }
    }
    _manager.set_focused(list[start % list.size()]);
}

void Focus_manager::focus_previous() {
    auto const list = _focusables();
    if (list.empty()) { return; }

    Widget* current = _manager.focused();
    std::size_t start = list.size() - 1u;
    for (std::size_t i = 0; i < list.size(); ++i) {
        if (list[i] == current) {
            start = (i == 0u) ? (list.size() - 1u) : (i - 1u);
            break;
        }
    }
    _manager.set_focused(list[start]);
}

void Focus_manager::clear_focus() {
    _manager.set_focused(nullptr);
}

// ----------------------------------------------------------------------------
void Focus_manager::set_enabled(bool enabled) { _enabled = enabled; }
bool Focus_manager::is_enabled() const        { return _enabled; }

} // namespace titan::ui
