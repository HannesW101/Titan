#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-app/include/application_config.hpp"

#include "module-render/include/renderer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-audio/include/audio_system.hpp"
#include "module-core/time/include/timer.hpp"
#include "module-core/events/include/event_listener.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

#include <functional>
#include <memory>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::app {

// ============================================================================
// Class Application
// ----------------------------------------------------------------------------

/**
 * @brief The engine's runtime shell.
 *
 * Owns the window and every core subsystem (renderer, resources, audio, the
 * frame timer) and runs the main loop:
 *
 *   while running:
 *     poll events (SFML_event_manager)
 *     accumulate dt
 *     while accumulator >= fixed_dt: fixed_update(fixed_dt) // gameplay/physics
 *     update(dt) // per-frame logic, animations, UI
 *     render(renderer) // submit drawables
 *
 * Game code does not subclass Application. Instead it installs callbacks
 * (set_update / set_fixed_update / set_render) or, more commonly, hands control
 * to a Scene_manager whose hooks Application calls. This keeps
 * the shell decoupled from any particular game structure.
 *
 * Lifetime: construct with an Application_config, optionally wire callbacks,
 * then call run(). run() returns when the window closes or quit() is called.
 */
class Application final : public events::Event_listener {

public:
    using Update_fn       = std::function<void(float dt         )>; ///< Per-frame update callback
    using Fixed_update_fn = std::function<void(float fixed_dt   )>; ///< Fixed-step update callback
    using Render_fn       = std::function<void(render::Renderer&)>; ///< Render callback
    using Event_hook_fn   = std::function<void(                 )>; ///< Start/stop hook

    /**
     * @brief Construct the application and bring up all subsystems.
     * @param config Window and loop configuration
     */
    explicit Application(Application_config config = {});

    /**
     * @brief Tear down subsystems and deregister event listeners.
     */
    ~Application();

    Application           (Application const&) = delete;
    Application& operator=(Application const&) = delete;
    Application           (Application&&     ) = delete;
    Application& operator=(Application&&     ) = delete;

    // -------------------------------------------------------------------------
    // Main loop
    // -------------------------------------------------------------------------

    /**
     * @brief Run the main loop until the window closes or quit() is called.
     * @return Process exit code (0 = clean).
     */
    int run();

    /**
     * @brief Request the loop to stop after the current frame.
     */
    void quit();

    /**
     * @brief Whether the loop is currently running.
     */
    bool is_running() const;

    // -------------------------------------------------------------------------
    // Callbacks (the integration points)
    // -------------------------------------------------------------------------

    /**
     * @brief Set the per-frame update callback.
     * @param fn Callback receiving the frame delta
     */
    void set_update(Update_fn fn);

    /**
     * @brief Set the fixed-step update callback.
     * @param fn Callback receiving the fixed delta
     */
    void set_fixed_update(Fixed_update_fn fn);

    /**
     * @brief Set the render callback.
     * @param fn Callback receiving the renderer
     */
    void set_render(Render_fn fn);

    /**
     * @brief Called once after subsystems are up but before the first frame.
     * Good place to load the initial scene / build initial UI.
     */
    void set_on_start(Event_hook_fn fn);

    /**
     * @brief Called once after the loop ends, before subsystems tear down.
     */
    void set_on_stop(Event_hook_fn fn);

    // -------------------------------------------------------------------------
    // Subsystem access
    // -------------------------------------------------------------------------

    /**
     * @brief Get the render window.
     * @return Reference to the window
     */
    sf::RenderWindow& window();

    /**
     * @brief Get the renderer.
     * @return Reference to the renderer
     */
    render::Renderer& renderer();

    /**
     * @brief Get the resource manager.
     * @return Reference to the resource manager
     */
    resources::Resource_manager& resources();

    /**
     * @brief Get the audio system.
     * @return Reference to the audio system
     */
    audio::Audio_system& audio();

    /**
     * @brief Get the frame timer.
     * @return Reference to the frame timer
     */
    core::Timer& frame_timer();

    /**
     * @brief Get the active configuration.
     * @return Const reference to the config
     */
    Application_config const& config() const;

    // -------------------------------------------------------------------------
    // Window control
    // -------------------------------------------------------------------------

    /**
     * @brief Rebuild the window from a (possibly changed) config. Use after the
     * user changes resolution / fullscreen in a settings menu.
     */
    void apply_config(Application_config const& config);

private:
    Application_config _config;

    std::unique_ptr<sf::RenderWindow> _window;
    std::unique_ptr<render::Renderer> _renderer;
    resources::Resource_manager& _resources;
    std::unique_ptr<audio::Audio_system> _audio;

    core::Timer _frame_timer;

    bool _running = false;
    float _accumulator = 0.0f;

    Update_fn _update;
    Fixed_update_fn _fixed_update;
    Render_fn _render;
    Event_hook_fn _on_start;
    Event_hook_fn _on_stop;

    /**
     * @brief Create the window, or recreate its contents in place if it exists.
     */
    void _create_window();

    /**
     * @brief Register the WINDOW_CLOSED handler that triggers quit().
     */
    void _register_close_handler();
};

} // namespace titan::app
