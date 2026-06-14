// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-render/include/renderer.hpp"

#include "SFML/Graphics.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

// ============================================================================
// Class Renderer
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Renderer::Renderer(sf::RenderWindow& window)
    : _window(window)
    , _world_camera(
        sf::Vector2f{
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
            }
        )
    , _ui_camera(
        sf::Vector2f{
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
            },
        sf::Vector2f{
            static_cast<float>(window.getSize().x / 2),
            static_cast<float>(window.getSize().y / 2)
            }
        )
    , _layer_cameras{}
    , _queues{}
    , _clear_color(sf::Color::Black)
    , _last_draw_calls(0u)
    {

    // Assign default cameras per layer
    for (std::uint8_t i = 0u; i < LAYER_COUNT; ++i) {

        auto const layer = static_cast<Render_layer>(i);

        switch (layer) {

            case Render_layer::UI:
            case Render_layer::UI_OVERLAY:
            case Render_layer::DEBUG:
                _layer_cameras[i] = &_ui_camera;
                break;

            default:
                _layer_cameras[i] = &_world_camera;
                break;
        }
    }
}

// ----------------------------------------------------------------------------
void Renderer::submit(
    Render_layer const layer,
    sf::Drawable const& drawable,
    sf::RenderStates const& states,
    float const sort_key
    ) {

    auto const idx = static_cast<std::uint8_t>(layer);
    _queues[idx].push_back({&drawable, states, sort_key});
}

// ----------------------------------------------------------------------------
Camera& Renderer::world_camera() {

    return _world_camera;
}

// ----------------------------------------------------------------------------
Camera const& Renderer::world_camera() const {

    return _world_camera;
}

// ----------------------------------------------------------------------------
Camera& Renderer::ui_camera() {

    return _ui_camera;
}

// ----------------------------------------------------------------------------
Camera const& Renderer::ui_camera() const {

    return _ui_camera;
}

// ----------------------------------------------------------------------------
void Renderer::set_layer_camera(
    Render_layer const layer,
    Camera* const camera
    ) {

    _layer_cameras[static_cast<std::uint8_t>(layer)] = camera;
}

// ----------------------------------------------------------------------------
Camera* Renderer::layer_camera(
    Render_layer const layer
    ) {

    return _layer_cameras[static_cast<std::uint8_t>(layer)];
}

// ----------------------------------------------------------------------------
void Renderer::begin_frame() {

    _window.clear(_clear_color);

    for (auto& queue : _queues) {

        queue.clear();
    }
}

// ----------------------------------------------------------------------------
void Renderer::end_frame() {

    _last_draw_calls = 0u;

    for (std::uint8_t i = 0u; i < LAYER_COUNT; ++i) {

        _flush_layer(static_cast<Render_layer>(i));
    }

    _window.display();
}

// ----------------------------------------------------------------------------
void Renderer::set_clear_color(
    sf::Color const color
    ) {

    _clear_color = color;
}

// ----------------------------------------------------------------------------
sf::Color Renderer::clear_color() const {

    return _clear_color;
}

// ----------------------------------------------------------------------------
sf::RenderWindow& Renderer::window() {

    return _window;
}

// ----------------------------------------------------------------------------
std::size_t Renderer::last_frame_draw_calls() const {

    return _last_draw_calls;
}

// ----------------------------------------------------------------------------
void Renderer::_flush_layer(
    Render_layer const layer
    ) {

    auto const idx    = static_cast<std::uint8_t>(layer);
    auto&      queue  = _queues[idx];
    auto*      camera = _layer_cameras[idx];

    if (queue.empty()) {

        return;
    }

    // Sort by sort_key ascending
    std::stable_sort(
        queue.begin(),
        queue.end(),
        [](Command const& a, Command const& b) { return a.sort_key < b.sort_key; }
        );

    // Apply camera view
    if (camera != nullptr) {

        _window.setView(camera->view());
    } else {

        _window.setView(_window.getDefaultView());
    }

    // Draw all commands
    for (auto const& cmd : queue) {

        _window.draw(*cmd.drawable, cmd.states);
        ++_last_draw_calls;
    }
}

} // namespace titan::render
