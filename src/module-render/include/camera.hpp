#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML//Graphics.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

// ============================================================================
// Class Camera
// ----------------------------------------------------------------------------

/**
 * @brief 2D camera wrapping sf::View.
 *
 * Supports target following with smoothing, screen shake,
 * zoom interpolation, and world bounds clamping.
 * Call tick(dt) each frame.
 */
class Camera {

public:
    /**
     * @brief Default constructor.
     */
    Camera();

    /**
     * @brief Construct with size and initial position.
     * @param size World units visible
     * @param initial_position Starting position
     */
    Camera(sf::Vector2f const size, sf::Vector2f const initial_position = {});

    // -------------------------------------------------------------------------
    // Following target
    // -------------------------------------------------------------------------

    /**
     * @brief Set target position the camera smoothly moves towards.
     * @param world_pos Target world position
     */
    void set_target_position(sf::Vector2f const world_pos);

    /**
     * @brief Instantly teleport camera to position.
     * @param world_pos Position to snap to
     */
    void snap_to(sf::Vector2f const world_pos);

    /**
     * @brief Set follow smoothing time.
     * @param seconds_to_reach 0 = instant
     */
    void set_follow_smoothing(float const seconds_to_reach);

    /**
     * @brief Get current (smoothed) camera position.
     * @return Current position
     */
    sf::Vector2f position() const;

    /**
     * @brief Get target position.
     * @return Target position
     */
    sf::Vector2f target_position() const;

    // -------------------------------------------------------------------------
    // Zoom
    // -------------------------------------------------------------------------

    /**
     * @brief Set zoom target (smoothed). 1.0 = nominal, higher = more zoomed in.
     * @param zoom Target zoom level
     */
    void set_zoom(float const zoom);

    /**
     * @brief Set zoom immediately.
     * @param zoom Zoom level
     */
    void set_zoom_immediate(float const zoom);

    /**
     * @brief Set zoom smoothing time.
     * @param seconds_to_reach 0 = instant
     */
    void set_zoom_smoothing(float const seconds_to_reach);

    /**
     * @brief Get current zoom level.
     * @return Zoom level
     */
    float zoom() const;

    // -------------------------------------------------------------------------
    // Shake
    // -------------------------------------------------------------------------

    /**
     * @brief Trigger a screen shake effect.
     * @param magnitude_pixels Max shake offset in pixels
     * @param duration_seconds How long shake lasts
     */
    void shake(float const magnitude_pixels, float const duration_seconds);

    /**
     * @brief Stop shake immediately.
     */
    void stop_shake();

    // -------------------------------------------------------------------------
    // Bounds
    // -------------------------------------------------------------------------

    /**
     * @brief Clamp camera so viewport stays within world bounds.
     * @param world_bounds Bounds rect
     */
    void set_bounds(sf::FloatRect const world_bounds);

    /**
     * @brief Disable bounds clamping.
     */
    void clear_bounds();

    /**
     * @brief Get current bounds.
     * @return Bounds rect
     */
    sf::FloatRect bounds() const;

    // -------------------------------------------------------------------------
    // View
    // -------------------------------------------------------------------------

    /**
     * @brief Set world units visible.
     * @param size View size
     */
    void set_size(sf::Vector2f const size);

    /**
     * @brief Get view size.
     * @return View size
     */
    sf::Vector2f size() const;

    /**
     * @brief Get the underlying sf::View.
     * @return Const reference to view
     */
    sf::View const& view() const;

    /**
     * @brief Get the currently visible world rect.
     * @return Visible rect
     */
    sf::FloatRect visible_rect() const;

    // -------------------------------------------------------------------------
    // Coordinate conversion
    // -------------------------------------------------------------------------

    /**
     * @brief Convert window pixel to world position.
     * @param pixel Window pixel coordinate
     * @param window_size Window size
     * @return World position
     */
    sf::Vector2f window_to_world(sf::Vector2i const pixel, sf::Vector2u const window_size) const;

    /**
     * @brief Convert world position to window pixel.
     * @param world World position
     * @param window_size Window size
     * @return Window pixel
     */
    sf::Vector2i world_to_window(sf::Vector2f const world, sf::Vector2u const window_size) const;

    // -------------------------------------------------------------------------
    // Update
    // -------------------------------------------------------------------------

    /**
     * @brief Tick camera - advance smoothing and shake.
     * @param dt Delta time in seconds
     */
    void tick(float const dt);

private:
    void _rebuild_view();
    void _clamp_to_bounds();

    sf::View     _view;
    sf::Vector2f _size;

    sf::Vector2f _current_position;
    sf::Vector2f _target_position;
    float        _position_smoothing;

    float _current_zoom;
    float _target_zoom;
    float _zoom_smoothing;

    float        _shake_magnitude;
    float        _shake_time_remaining;
    float        _shake_duration;
    sf::Vector2f _shake_offset;

    bool          _has_bounds;
    sf::FloatRect _bounds;
};

} // namespace titan::render
