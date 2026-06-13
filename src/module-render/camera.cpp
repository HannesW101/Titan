// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-render/include/camera.hpp"
#include "module-utils/include/math.hpp"
#include "module-utils/include/random.hpp"

#include <cmath>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Camera
// ----------------------------------------------------------------------------

Camera::Camera()
    : _view()
    , _size(800.0f, 600.0f)
    , _current_position(0.0f, 0.0f)
    , _target_position(0.0f, 0.0f)
    , _position_smoothing(0.0f)
    , _current_zoom(1.0f)
    , _target_zoom(1.0f)
    , _zoom_smoothing(0.0f)
    , _shake_magnitude(0.0f)
    , _shake_time_remaining(0.0f)
    , _shake_duration(1.0f)
    , _shake_offset(0.0f, 0.0f)
    , _has_bounds(false)
    , _bounds()
    {

    _rebuild_view();
}

// ----------------------------------------------------------------------------
Camera::Camera(
    sf::Vector2f const size,
    sf::Vector2f const initial_position
    )
    : _view()
    , _size(size)
    , _current_position(initial_position)
    , _target_position(initial_position)
    , _position_smoothing(0.0f)
    , _current_zoom(1.0f)
    , _target_zoom(1.0f)
    , _zoom_smoothing(0.0f)
    , _shake_magnitude(0.0f)
    , _shake_time_remaining(0.0f)
    , _shake_duration(1.0f)
    , _shake_offset(0.0f, 0.0f)
    , _has_bounds(false)
    , _bounds()
    {

    _rebuild_view();
}

// ----------------------------------------------------------------------------
void Camera::set_target_position(
    sf::Vector2f const world_pos
    ) {

    _target_position = world_pos;
}

// ----------------------------------------------------------------------------
void Camera::snap_to(
    sf::Vector2f const world_pos
    ) {

    _current_position = world_pos;
    _target_position  = world_pos;
    _rebuild_view();
}

// ----------------------------------------------------------------------------
void Camera::set_follow_smoothing(
    float const seconds_to_reach
    ) {

    _position_smoothing = seconds_to_reach;
}

// ----------------------------------------------------------------------------
sf::Vector2f Camera::position() const {

    return _current_position;
}

// ----------------------------------------------------------------------------
sf::Vector2f Camera::target_position() const {

    return _target_position;
}

// ----------------------------------------------------------------------------
void Camera::set_zoom(
    float const zoom
    ) {

    _target_zoom = zoom;
}

// ----------------------------------------------------------------------------
void Camera::set_zoom_immediate(
    float const zoom
    ) {

    _current_zoom = zoom;
    _target_zoom  = zoom;
    _rebuild_view();
}

// ----------------------------------------------------------------------------
void Camera::set_zoom_smoothing(
    float const seconds_to_reach
    ) {

    _zoom_smoothing = seconds_to_reach;
}

// ----------------------------------------------------------------------------
float Camera::zoom() const {

    return _current_zoom;
}

// ----------------------------------------------------------------------------
void Camera::shake(
    float const magnitude_pixels,
    float const duration_seconds
    ) {

    _shake_magnitude      = magnitude_pixels;
    _shake_time_remaining = duration_seconds;
    _shake_duration       = duration_seconds;
}

// ----------------------------------------------------------------------------
void Camera::stop_shake() {

    _shake_magnitude      = 0.0f;
    _shake_time_remaining = 0.0f;
    _shake_offset         = {0.0f, 0.0f};
}

// ----------------------------------------------------------------------------
void Camera::set_bounds(
    sf::FloatRect const world_bounds
    ) {

    _has_bounds = true;
    _bounds     = world_bounds;
}

// ----------------------------------------------------------------------------
void Camera::clear_bounds() {

    _has_bounds = false;
}

// ----------------------------------------------------------------------------
sf::FloatRect Camera::bounds() const {

    return _bounds;
}

// ----------------------------------------------------------------------------
void Camera::set_size(
    sf::Vector2f const size
    ) {

    _size = size;
    _rebuild_view();
}

// ----------------------------------------------------------------------------
sf::Vector2f Camera::size() const {

    return _size;
}

// ----------------------------------------------------------------------------
sf::View const& Camera::view() const {

    return _view;
}

// ----------------------------------------------------------------------------
sf::FloatRect Camera::visible_rect() const {

    sf::Vector2f const half_size = _size / (2.0f * _current_zoom);

    return sf::FloatRect(
        _current_position - half_size,
        _size / _current_zoom
        );
}

// ----------------------------------------------------------------------------
sf::Vector2f Camera::window_to_world(
    sf::Vector2i const pixel,
    sf::Vector2u const window_size
    ) const {

    // remap pixel coords from [0, window_size] to [-1, 1]
    float const nx = remap(
        static_cast<float>(pixel.x),
        0.0f, static_cast<float>(window_size.x),
        -1.0f, 1.0f
        );

    float const ny = remap(
        static_cast<float>(pixel.y),
        0.0f, static_cast<float>(window_size.y),
        -1.0f, 1.0f
        );

    float const half_w = (_size.x / _current_zoom) * 0.5f;
    float const half_h = (_size.y / _current_zoom) * 0.5f;

    return {
        _current_position.x + nx * half_w,
        _current_position.y + ny * half_h
        };
}

// ----------------------------------------------------------------------------
sf::Vector2i Camera::world_to_window(
    sf::Vector2f const world,
    sf::Vector2u const window_size
    ) const {

    float const half_w = (_size.x / _current_zoom) * 0.5f;
    float const half_h = (_size.y / _current_zoom) * 0.5f;

    // inverse_lerp to get normalized position, then remap to window pixels
    float const nx = (world.x - _current_position.x) / half_w;
    float const ny = (world.y - _current_position.y) / half_h;

    return {
        static_cast<int>(remap(nx, -1.0f, 1.0f, 0.0f, static_cast<float>(window_size.x))),
        static_cast<int>(remap(ny, -1.0f, 1.0f, 0.0f, static_cast<float>(window_size.y)))
        };
}

// ----------------------------------------------------------------------------
void Camera::tick(
    float const dt
    ) {

    // --- Position smoothing ---
    if (_position_smoothing <= 0.0f) {

        _current_position = _target_position;
    } else {

        // Exponential decay smoothing - framerate independent
        float const alpha = 1.0f - std::exp(-dt / _position_smoothing);
        _current_position = lerp(_current_position, _target_position, alpha);
    }

    // --- Zoom smoothing ---
    if (_zoom_smoothing <= 0.0f) {

        _current_zoom = _target_zoom;
    } else {

        float const alpha = 1.0f - std::exp(-dt / _zoom_smoothing);
        _current_zoom = lerp(_current_zoom, _target_zoom, alpha);
    }

    // --- Shake ---
    if (_shake_time_remaining > 0.0f) {

        _shake_time_remaining -= dt;

        // remap remaining time [0, duration] to magnitude [0, 1] for linear decay
        float const progress = remap(
            _shake_time_remaining,
            0.0f, _shake_duration,
            0.0f, 1.0f
            );

        float const current_magnitude = _shake_magnitude * progress;

        _shake_offset = Random::global().in_circle(current_magnitude);

        if (_shake_time_remaining <= 0.0f) {

            _shake_time_remaining = 0.0f;
            _shake_offset         = {0.0f, 0.0f};
        }
    }

    _clamp_to_bounds();
    _rebuild_view();
}

// ----------------------------------------------------------------------------
void Camera::_rebuild_view() {

    sf::Vector2f const final_pos   = _current_position + _shake_offset;
    sf::Vector2f const zoomed_size = _size / _current_zoom;

    _view.setCenter(final_pos);
    _view.setSize(zoomed_size);
}

// ----------------------------------------------------------------------------
void Camera::_clamp_to_bounds() {

    if (!_has_bounds) {

        return;
    }

    sf::Vector2f const half = (_size / _current_zoom) * 0.5f;

    // Use our own clamp from math library
    _current_position.x = clamp(
        _current_position.x,
        _bounds.position.x + half.x,
        _bounds.position.x + _bounds.size.x - half.x
        );

    _current_position.y = clamp(
        _current_position.y,
        _bounds.position.y + half.y,
        _bounds.position.y + _bounds.size.y - half.y
        );
}

} // namespace titan::render
