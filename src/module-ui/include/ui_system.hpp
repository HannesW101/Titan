#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/ui_manager.hpp"
#include "module-ui/include/focus_manager.hpp"

#include <memory>

namespace titan::app { class Application; }
namespace sf { class RenderWindow; }

namespace titan::ui {

// ============================================================================
// Class UI_system
// ----------------------------------------------------------------------------

/**
 * @brief Bundles a UI_manager + Focus_manager and wires them into the engine
 * loop, so a Scene gets working UI with one object.
 *
 * It pulls the renderer + resource manager from the Application, syncs the UI
 * mouse position from the window each frame, runs layout/update, and renders on
 * the UI layers. Keyboard focus traversal (Tab) is handled by the embedded
 * Focus_manager.
 *
 *   class Settings : public scene::Scene {
 *       ui::UI_system _ui{ application() };
 *       void on_enter() override { build_widgets(_ui.manager()); }
 *       void update(float dt) override { _ui.update(dt); }
 *       void render(render::Renderer& r) override { _ui.render(); }
 *   };
 */
class UI_system final {

public:
    /**
     * @brief Construct a UI_system.
     * @param application application
     */
    explicit UI_system(app::Application& application);

    UI_system(UI_system const&)            = delete;
    UI_system& operator=(UI_system const&) = delete;
    UI_system(UI_system&&)                 = delete;
    UI_system& operator=(UI_system&&)      = delete;

    /**
     * @brief Get the manager.
     * @return The manager
     */
    UI_manager&    manager();
    /**
     * @brief Get the focus.
     * @return The focus
     */
    Focus_manager& focus();

    /**
     * @brief Sync mouse position from the window, run layout + update.
     */
    void update(float dt);

    /**
     * @brief Render the UI to the renderer's UI layers.
     */
    void render();

    /**
     * @brief Re-run layout (call on window resize).
     */
    void layout();

private:
    app::Application& _app;
    std::unique_ptr<UI_manager>    _manager;
    std::unique_ptr<Focus_manager> _focus;
};

} // namespace titan::ui
