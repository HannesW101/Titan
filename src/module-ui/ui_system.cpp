// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/ui_system.hpp"

#include "module-app/include/application.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Mouse.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

UI_system::UI_system(app::Application& application)
    : _app(application)
{
    _manager = std::make_unique<UI_manager>(_app.renderer(), _app.resources());
    _focus   = std::make_unique<Focus_manager>(*_manager);
}

UI_manager&    UI_system::manager() { return *_manager; }
Focus_manager& UI_system::focus()   { return *_focus; }

// ----------------------------------------------------------------------------
void UI_system::update(float dt) {
    sf::RenderWindow& window = _app.window();
    sf::Vector2i const pixel = sf::Mouse::getPosition(window);
    _manager->set_mouse_position(sf::Vector2f(pixel));
    _manager->layout();
    _manager->update(dt);
}

// ----------------------------------------------------------------------------
void UI_system::render() {
    _manager->render();
}

// ----------------------------------------------------------------------------
void UI_system::layout() {
    _manager->layout();
}

} // namespace titan::ui
