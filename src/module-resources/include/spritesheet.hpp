#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-resources/include/animation.hpp"

#include "SFML/Graphics.hpp"

#include <string>
#include <unordered_map>
#include <optional>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::resources {

// ============================================================================
// Class Spritesheet
// ----------------------------------------------------------------------------

/**
 * @brief A texture with named animations and baked-in animator state.
 */
class Spritesheet {

public:
    /**
     * @brief Construct from an existing texture.
     * @param texture Loaded SFML texture
     */
    explicit Spritesheet(sf::Texture texture);

    /**
     * @brief Add a named animation.
     * @param animation Animation to add
     */
    void add_animation(Animation const& animation);

    /**
     * @brief Set the current animation by name.
     * @param name Animation name
     */
    void set_animation(std::string const& name);

    /**
     * @brief Advance the animation by delta time.
     * @param dt Delta time in seconds
     */
    void tick(float const dt);

    /**
     * @brief Pause playback.
     */
    void pause();

    /**
     * @brief Resume playback.
     */
    void resume();

    /**
     * @brief Reset current animation to first frame.
     */
    void reset();

    /**
     * @brief Check if a one-shot animation has finished.
     * @return True if finished
     */
    bool is_finished() const;

    /**
     * @brief Get the current frame rect.
     * @return Current sf::IntRect
     */
    sf::IntRect current_frame() const;

    /**
     * @brief Get the underlying texture.
     * @return Reference to sf::Texture
     */
    sf::Texture const& texture() const;

    /**
     * @brief Get animation by name.
     * @param name Animation name
     * @return Optional animation
     */
    std::optional<Animation> get_animation(std::string const& name) const;

    /**
     * @brief Get current animation name.
     * @return Current animation name
     */
    std::string const& current_animation_name() const;

    /**
     * @brief Get current frame index.
     * @return Current frame index
     */
    int current_frame_index() const;

private:
    sf::Texture                                _texture; ///< Underlying texture
    std::unordered_map<std::string, Animation> _animations; ///< Named animations

    std::string _current_animation; ///< Name of active animation
    int         _current_frame  = 0; ///< Current frame index
    float       _elapsed        = 0.0f; ///< Elapsed time since last frame
    bool        _paused         = false; ///< Paused state
    bool        _finished       = false; ///< One-shot finished flag
    int         _ping_pong_dir  = 1; ///< Ping-pong direction (+1 or -1)
};

} // namespace titan::resources
