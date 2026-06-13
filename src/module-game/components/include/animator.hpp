#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include "module-resources/include/animation.hpp"

#include <memory>
#include <string>

// ============================================================================
// Forward declarations (1/2)
// ----------------------------------------------------------------------------

namespace titan::resources { class Spritesheet; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Forward declarations (2/2)
// ----------------------------------------------------------------------------

class Sprite_renderer;

// ============================================================================
// Class Animator
// ----------------------------------------------------------------------------

/**
 * @brief Plays named animations from a Spritesheet and feeds the current frame
 * rect into the owner's Sprite_renderer each frame.
 *
 * Pairs with a Sprite_renderer on the same object: the Animator owns timing and
 * frame selection, the Sprite_renderer owns drawing. The Spritesheet resource
 * is loaded by name from the Resource_manager.
 *
 *   auto* sr  = obj->add_component<Sprite_renderer>();
 *   sr->set_texture("hero_sheet");
 *   auto* anim = obj->add_component<Animator>();
 *   anim->set_spritesheet("hero_sheet");
 *   anim->add_animation({ "run", { rect0, rect1, rect2 }, 12.f });
 *   anim->play("run");
 */
class Animator final : public Game_component {

public:
    /**
     * @brief Construct an animator with no spritesheet.
     */
    Animator() = default;

    /**
     * @brief Set the spritesheet by resource name.
     * @param resource_name Spritesheet key in the Resource_manager
     */
    void set_spritesheet(std::string const& resource_name);

    /**
     * @brief Add an animation clip to the spritesheet.
     * @param animation Animation definition (name, frames, fps)
     */
    void add_animation(resources::Animation const& animation);

    /**
     * @brief Play a named animation (deferred if sheet not yet loaded).
     * @param name Animation name
     */
    void play(std::string const& name);

    /**
     * @brief Pause playback.
     */
    void pause();

    /**
     * @brief Resume playback.
     */
    void resume();

    /**
     * @brief Reset the current animation to its first frame.
     */
    void reset();

    /**
     * @brief Check whether a non-looping animation has finished.
     * @return True if finished
     */
    bool is_finished() const;

    /**
     * @brief Get the current animation name.
     * @return Reference to the current animation name
     */
    std::string const& current_animation() const;

    /**
     * @brief Resolve the spritesheet and cache the Sprite_renderer.
     */
    void on_start() override;

    /**
     * @brief Advance the animation and push the frame to the Sprite_renderer.
     * @param dt Frame delta time in seconds
     */
    void update(float const dt) override;

private:
    std::string                             _sheet_name; ///< Spritesheet key
    std::shared_ptr<resources::Spritesheet> _sheet; ///< Resolved sheet
    std::string                             _pending_play; ///< Deferred play target
    std::string                             _current; ///< Current animation

    Sprite_renderer* _renderer = nullptr;  ///< Cached sibling renderer

    /**
     * @brief Resolve the spritesheet from the Resource_manager.
     */
    void _acquire();
};

} // namespace titan::game
