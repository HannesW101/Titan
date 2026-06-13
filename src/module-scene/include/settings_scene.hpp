#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-scene/include/scene.hpp"

#include "module-ui/include/ui_system.hpp"

#include <functional>
#include <memory>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::scene {

// ============================================================================
// Class Settings_scene
// ----------------------------------------------------------------------------

/**
 * @brief A ready-made settings overlay: volume sliders bound to the audio buses
 * (Master / Music / SFX / UI), live-updating the mix and persisting it to a
 * named Config on close.
 *
 * Demonstrates the full UI integration path: Scene + UI_system + Audio_system +
 * Config working together. Push it over gameplay as an overlay (it renders the
 * scene below but freezes its updates):
 *
 *   scenes().push(std::make_unique<Settings_scene>("settings"));
 *
 * On "Back", it saves the audio mix to the config (via Audio_system::save_mix)
 * and pops itself.
 */
class Settings_scene final : public Scene {

public:
    /**
     * @brief Construct the settings overlay scene.
     * @param name Scene name (defaults to "settings")
     */
    explicit Settings_scene(std::string name = "settings");

    /**
     * @brief Config name (in Config_manager) to persist the mix into.
     * Defaults to "settings".
     */
    /**
     * @brief Set the Config name to persist the audio mix into.
     * @param config_name Config identifier (defaults to "settings")
     */
    void set_config_name(std::string config_name);

    /**
     * @brief Build the UI when the scene is entered.
     */
    void on_enter() override;

    /**
     * @brief Update the UI system.
     * @param dt Frame delta time in seconds
     */
    void update(float dt) override;

    /**
     * @brief Render the UI.
     * @param renderer Renderer (UI draws via the shared renderer)
     */
    void render(render::Renderer& renderer) override;

private:
    std::unique_ptr<ui::UI_system> _ui;
    std::string                    _config_name = "settings";

    /**
     * @brief Build the settings panel, sliders, and back button.
     */
    void _build_ui();

    /**
     * @brief Persist the audio mix to config and pop the scene.
     */
    void _save_and_close();
};

} // namespace titan::scene
