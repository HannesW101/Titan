#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include "module-audio/include/audio_bus.hpp"

#include <string>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::audio { class Audio_system; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Audio_emitter
// ----------------------------------------------------------------------------

/**
 * @brief Plays sound effects positioned at the owner's world location.
 *
 * Components don't own the Audio_system (the Application does), so the scene
 * wires a reference in via set_audio_system(). play() then emits the named SFX
 * with spatial attenuation centered on the owner's transform, so distant
 * emitters are quieter.
 *
 *   auto* em = obj->add_component<Audio_emitter>();
 *   em->set_audio_system(app.audio());
 *   em->set_falloff_radius(600.f);
 *   em->play("footstep");
 */
class Audio_emitter final : public Game_component {

public:
    /**
     * @brief Construct an emitter with no audio system bound.
     */
    Audio_emitter() = default;

    /**
     * @brief Bind the audio system used to play sounds.
     * @param system Audio system reference
     */
    void set_audio_system(audio::Audio_system& system);

    /**
     * @brief Set the audio bus to route sounds through.
     * @param bus Target audio bus
     */
    void set_bus(audio::Audio_bus const bus);

    /**
     * @brief Get the audio bus.
     * @return Current bus
     */
    audio::Audio_bus bus() const;

    /**
     * @brief Set the spatial falloff radius (distance to silence).
     * @param radius Falloff radius in pixels
     */
    void set_falloff_radius(float const radius);

    /**
     * @brief Get the spatial falloff radius.
     * @return Falloff radius in pixels
     */
    float falloff_radius() const;

    /**
     * @brief Enable or disable spatial positioning.
     * @param spatial True for positioned audio, false for non-spatial
     */
    void set_spatial(bool const spatial);

    /**
     * @brief Check whether spatial positioning is enabled.
     * @return True if spatial
     */
    bool is_spatial() const;

    /**
     * @brief Play a one-shot SFX by resource name.
     * @param resource_name Sound buffer key in the Resource_manager
     * @return Play id, or -1 if not played
     */
    int play(std::string const& resource_name);

    /**
     * @brief Play a one-shot SFX with volume/pitch overrides.
     * @param resource_name Sound buffer key in the Resource_manager
     * @param volume Volume scalar [0, 1] before bus scaling
     * @param pitch Pitch multiplier (1.0 = normal)
     * @return Play id, or -1 if not played
     */
    int play(std::string const& resource_name, float const volume, float const pitch);

private:
    audio::Audio_system* _system         = nullptr; ///< Bound audio system
    audio::Audio_bus     _bus            = audio::Audio_bus::SFX; ///< Output bus
    float                _falloff_radius = 500.0f; ///< Spatial falloff
    bool                 _spatial        = true; ///< Spatial flag
};

} // namespace titan::game
