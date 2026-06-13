// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-app/include/application.hpp"

#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-core/events/include/event_type.hpp"

#include "module-utils/include/logger.hpp"

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/WindowEnums.hpp"
#include "SFML/Window/ContextSettings.hpp"

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;
using namespace titan::events;

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::app {

// ============================================================================
// Construction
// ============================================================================

// ----------------------------------------------------------------------------
Application::Application(
    Application_config config
    )
    : _config   (std::move(config)                      )
    , _resources(resources::Resource_manager::instance())
    {

    _create_window();

    _renderer = std::make_unique<render::Renderer>(*_window);
    _audio    = std::make_unique<audio::Audio_system>(_resources);

    _renderer->set_clear_color(_config.clear_color);

    // Drive the fixed-step loop through the frame timer's built-in accumulator.
    _frame_timer.set_fixed_dt(_config.fixed_timestep);
    _frame_timer.set_max_delta(
        _config.fixed_timestep * static_cast<float>(_config.max_fixed_steps_per_frame)
        );

    _register_close_handler();

    LOG(Log_lvl::INFO) << "Application initialized";
}

// ----------------------------------------------------------------------------
Application::~Application() {

    // Subsystems tear down in reverse construction order automatically.
    SFML_event_manager::instance().deregister_listener(
        listener_id()
        );
}

// ============================================================================
// Window
// ============================================================================

// ----------------------------------------------------------------------------
void Application::_create_window() {

    sf::ContextSettings settings;
    settings.antiAliasingLevel = _config.antialiasing;

    std::uint32_t const style = _config.resizable
        ? sf::Style::Default
        : (sf::Style::Titlebar | sf::Style::Close);

    sf::State const state = _config.fullscreen
        ? sf::State::Fullscreen
        : sf::State::Windowed;

    sf::VideoMode const mode(
        sf::Vector2u{ _config.width, _config.height }
        );

    if (!_window) {

        // First creation: allocate the window object once. Its address stays
        // stable for the Application's lifetime, so a Renderer (and anything
        // else) holding sf::RenderWindow& remains valid across reconfigures.
        _window = std::make_unique<sf::RenderWindow>(mode, _config.title, style, state, settings);
    } else {

        // Reconfigure in place: recreate the window's contents without
        // replacing the object, keeping all existing references valid.
        _window->create(mode, _config.title, style, state, settings);
    }

    _window->setVerticalSyncEnabled(_config.vsync);

    if (!_config.vsync && _config.framerate_limit > 0u) {

        _window->setFramerateLimit(_config.framerate_limit);
    }
}

// ----------------------------------------------------------------------------
void Application::_register_close_handler() {

    // Stable unique id from Event_listener.
    std::uint64_t const id = listener_id();

    SFML_event_manager::instance().register_callback(
        SFML_event_type::WINDOW_CLOSED,
        [this](SFML_event_data const&) {

            quit();
        },
        id
        );
}

// ----------------------------------------------------------------------------
void Application::apply_config(
    Application_config const& config
    ) {

    _config = config;

    // Recreate the window contents in place (same object), then just push the
    // new clear color and timing. The Renderer keeps working because it holds a
    // reference to the same sf::RenderWindow, which we never destroyed.
    _create_window();

    _renderer->set_clear_color(_config.clear_color);

    _frame_timer.set_fixed_dt(_config.fixed_timestep);
    _frame_timer.set_max_delta(
        _config.fixed_timestep * static_cast<float>(_config.max_fixed_steps_per_frame)
        );
}

// ============================================================================
// Main loop
// ============================================================================

// ----------------------------------------------------------------------------
int Application::run() {

    _running = true;

    if (_on_start) { _on_start(); }

    auto& event_mgr = SFML_event_manager::instance();

    while (_running && _window->isOpen()) {

        // 1. Input
        event_mgr.process_events(*_window);

        // 2. Time
        _frame_timer.tick();
        float const dt = _frame_timer.delta_time();

        // 3. Fixed-step updates (gameplay / physics). The timer accumulates
        //    real time and hands back whole fixed steps, capping the total so a
        //    long frame can't trigger a spiral of death.
        while (_frame_timer.consume_fixed_step()) {

            if (_fixed_update) { _fixed_update(_config.fixed_timestep); }
        }

        // 4. Per-frame update (animations, UI, anything frame-rate dependent)
        if (_update) { _update(dt); }
        _audio->update(dt);

        // 5. Render
        _renderer->begin_frame();
        if (_render) { _render(*_renderer); }
        _renderer->end_frame();
    }

    if (_on_stop) { _on_stop(); }

    _running = false;
    return 0;
}

// ----------------------------------------------------------------------------
void Application::quit      ()       { _running = false; }
bool Application::is_running() const { return _running;  }

// ============================================================================
// Callbacks
// ============================================================================

// ----------------------------------------------------------------------------
void Application::set_update      (Update_fn       fn) { _update       = std::move(fn); }
void Application::set_fixed_update(Fixed_update_fn fn) { _fixed_update = std::move(fn); }
void Application::set_render      (Render_fn       fn) { _render       = std::move(fn); }
void Application::set_on_start    (Event_hook_fn   fn) { _on_start     = std::move(fn); }
void Application::set_on_stop     (Event_hook_fn   fn) { _on_stop      = std::move(fn); }

// ============================================================================
// Accessors
// ============================================================================

// ----------------------------------------------------------------------------
sf::RenderWindow&            Application::window     ()       { return *_window;     }
render::Renderer&            Application::renderer   ()       { return *_renderer;   }
resources::Resource_manager& Application::resources  ()       { return _resources;   }
audio::Audio_system&         Application::audio      ()       { return *_audio;      }
core::Timer&                 Application::frame_timer()       { return _frame_timer; }
Application_config const&    Application::config     () const { return _config;      }

} // namespace titan::app
