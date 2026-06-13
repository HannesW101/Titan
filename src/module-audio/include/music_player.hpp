#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-resources/include/resource_manager.hpp"

#include "SFML/Audio/Music.hpp"

#include <memory>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Class Music_player
// ----------------------------------------------------------------------------

/**
 * @brief Streaming music player with crossfade support.
 *
 * Manages two slots: an active track and an outgoing track fading out.
 * Crossfade smoothly transitions between them. Direct play() replaces
 * the current track immediately with no fade.
 *
 * Music is loaded via Resource_manager using string keys. The resource
 * manager handles the sf::Music lifetime and streaming from disk.
 */
class Music_player final {

public:
    /**
     * @brief Construct with resource manager.
     * @param rm Resource manager for resolving music assets
     */
    explicit Music_player(resources::Resource_manager& rm);

    ~Music_player() = default;

    Music_player           (Music_player const&) = delete;
    Music_player& operator=(Music_player const&) = delete;
    Music_player           (Music_player&&     ) = delete;
    Music_player& operator=(Music_player&&     ) = delete;

    // -------------------------------------------------------------------------
    // Playback
    // -------------------------------------------------------------------------

    /**
     * @brief Play a track immediately, replacing the current one without fade.
     * @param resource_name Name of sf::Music in Resource_manager
     * @param loop Whether to loop
     */
    void play(std::string const& resource_name, bool const loop = true);

    /**
     * @brief Crossfade to a new track over a given duration.
     * @param resource_name Name of sf::Music in Resource_manager
     * @param seconds Crossfade duration in seconds
     * @param loop Whether to loop the new track
     */
    void crossfade_to(
        std::string const& resource_name,
        float const seconds,
        bool const loop = true
        );

    /**
     * @brief Stop all playback immediately.
     */
    void stop();

    /**
     * @brief Pause playback.
     */
    void pause();

    /**
     * @brief Resume paused playback.
     */
    void resume();

    /**
     * @brief Check if music is currently playing.
     * @return True if playing
     */
    bool is_playing() const;

    /**
     * @brief Get the resource name of the current active track.
     * @return Current track resource name, empty if none
     */
    std::string const& current_track() const;

    // -------------------------------------------------------------------------
    // Per frame
    // -------------------------------------------------------------------------

    /**
     * @brief Drive crossfade interpolation and apply bus volume.
     * @param dt Delta time in seconds
     * @param effective_volume Combined bus volume (master * music bus)
     */
    void update(float const dt, float const effective_volume);

private:
    void _kill_outgoing();

    struct Slot {

        std::shared_ptr<sf::Music> music; ///< Shared ptr from Resource_manager
        float                      volume_start   = 1.0f;
        float                      volume_target  = 1.0f;
        float                      fade_duration  = 0.0f;
        float                      fade_elapsed   = 0.0f;
        std::string                resource_name  = "";
        bool                       fading         = false;
    };

    resources::Resource_manager& _rm;
    Slot                         _active;
    Slot                         _outgoing;
    bool                         _paused = false;
};

} // namespace titan::audio
