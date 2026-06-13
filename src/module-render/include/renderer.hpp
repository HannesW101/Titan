#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-render/include/render_layer.hpp"
#include "module-render/include/camera.hpp"

#include "SFML/Graphics.hpp"

#include <array>
#include <vector>
#include <cstddef>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace sf { class RenderWindow; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

// ============================================================================
// Class Renderer
// ----------------------------------------------------------------------------

/**
 * @brief Layered render frontend.
 *
 * Game code submits drawables to layers. On end_frame() queues are sorted
 * by sort_key and flushed in layer order with the appropriate camera applied.
 *
 * Submitted drawables MUST remain alive until end_frame() completes.
 */
class Renderer final {

public:
    /**
     * @brief Construct with a render window.
     * @param window SFML render window
     */
    explicit Renderer(sf::RenderWindow& window);

    // -------------------------------------------------------------------------
    // Submission
    // -------------------------------------------------------------------------

    /**
     * @brief Submit a drawable to a layer.
     * @param layer Render layer
     * @param drawable Drawable to submit (must outlive end_frame)
     * @param states Render states
     * @param sort_key Sort order within layer (lower = drawn first)
     */
    void submit(
        Render_layer layer,
        sf::Drawable const& drawable,
        sf::RenderStates const& states = sf::RenderStates::Default,
        float sort_key = 0.0f
        );

    // -------------------------------------------------------------------------
    // Cameras
    // -------------------------------------------------------------------------

    /**
     * @brief Get world camera (used by BACKGROUND..FOREGROUND layers).
     */
    Camera&       world_camera();
    Camera const& world_camera() const;

    /**
     * @brief Get UI camera (used by UI..DEBUG layers).
     */
    Camera&       ui_camera();
    Camera const& ui_camera() const;

    /**
     * @brief Override the camera for a specific layer.
     * @param layer Layer to override
     * @param camera Camera to use (nullptr = use default)
     */
    void set_layer_camera(Render_layer const layer, Camera* camera);

    /**
     * @brief Get the camera assigned to a layer.
     * @param layer Render layer
     * @return Camera pointer
     */
    Camera* layer_camera(Render_layer const layer);

    // -------------------------------------------------------------------------
    // Frame
    // -------------------------------------------------------------------------

    /**
     * @brief Clear window and submission queues. Call at start of frame.
     */
    void begin_frame();

    /**
     * @brief Sort queues, draw all layers, present. Call at end of frame.
     */
    void end_frame();

    /**
     * @brief Set window clear color.
     * @param color Clear color
     */
    void set_clear_color(sf::Color const color);

    /**
     * @brief Get window clear color.
     * @return Clear color
     */
    sf::Color clear_color() const;

    // -------------------------------------------------------------------------
    // Window
    // -------------------------------------------------------------------------

    /**
     * @brief Get the render window.
     * @return Reference to sf::RenderWindow
     */
    sf::RenderWindow& window();

    // -------------------------------------------------------------------------
    // Stats
    // -------------------------------------------------------------------------

    /**
     * @brief Get draw call count from last frame.
     * @return Draw call count
     */
    std::size_t last_frame_draw_calls() const;

private:
    /**
     * @brief Sort and flush a single layer to the window.
     * @param layer Layer to flush
     */
    void _flush_layer(Render_layer const layer);

    struct Command {

        sf::Drawable const* drawable;   ///< Non-owning pointer
        sf::RenderStates    states;
        float               sort_key;
    };

    sf::RenderWindow& _window;

    Camera _world_camera;
    Camera _ui_camera;

    std::array<Camera*, LAYER_COUNT>              _layer_cameras;
    std::array<std::vector<Command>, LAYER_COUNT> _queues;

    sf::Color   _clear_color;
    std::size_t _last_draw_calls;
};

} // namespace titan::render
