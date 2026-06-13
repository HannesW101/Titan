#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-audio/include/audio_bus.hpp"
#include "module-audio/include/sfx_options.hpp"
#include "module-audio/include/sfx_player.hpp"
#include "module-audio/include/music_player.hpp"

#include "module-core/config/include/config.hpp"

#include "SFML/Graphics.hpp"

#include <array>
#include <string>
#include <cstddef>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::resources { class Resource_manager; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Class Audio_system
// ----------------------------------------------------------------------------

/**
 * @brief Top-level audio facade.
 *
 * Manages all audio buses and routes play requests to Sfx_player or
 * Music_player. Game code calls play_sfx("explosion") and the system
 * resolves the sound buffer from Resource_manager and submits it to
 * the SFX pool.
 *
 * Effective volume for any bus = MASTER volume * bus volume, clamped [0, 1].
 * Muted buses produce no sound regardless of volume settings.
 */
class Audio_system final {

public:
    /**
     * @brief Construct with resource manager.
     * @param rm Resource manager for resolving sound assets
     */
    explicit Audio_system(resources::Resource_manager& rm);

    Audio_system           (Audio_system const&) = delete;
    Audio_system& operator=(Audio_system const&) = delete;
    Audio_system           (Audio_system&&     ) = delete;
    Audio_system& operator=(Audio_system&&     ) = delete;

    // -------------------------------------------------------------------------
    // Bus controls
    // -------------------------------------------------------------------------

    /**
     * @brief Set raw volume for a bus.
     * @param bus Target bus
     * @param volume Volume [0, 1]
     */
    void set_bus_volume(Audio_bus const bus, float const volume);

    /**
     * @brief Get raw volume for a bus (not scaled by master).
     * @param bus Target bus
     * @return Raw volume [0, 1]
     */
    float bus_volume(Audio_bus const bus) const;

    /**
     * @brief Get effective volume for a bus (master * bus, 0 if muted).
     * @param bus Target bus
     * @return Effective volume [0, 1]
     */
    float effective_volume(Audio_bus const bus) const;

    /**
     * @brief Set mute state for a bus.
     * @param bus Target bus
     * @param muted Muted state
     */
    void set_bus_muted(Audio_bus const bus, bool const muted);

    /**
     * @brief Check if a bus is muted.
     * @param bus Target bus
     * @return True if muted
     */
    bool is_bus_muted(Audio_bus const bus) const;

    // -------------------------------------------------------------------------
    // Mix persistence
    // -------------------------------------------------------------------------

    /**
     * @brief Load bus volumes and mute states from config.
     * @param config Config to read from
     */
    void load_mix(core::Config const& config);

    /**
     * @brief Save bus volumes and mute states to config.
     * @param config Config to write to
     */
    void save_mix(core::Config& config) const;

    // -------------------------------------------------------------------------
    // SFX
    // -------------------------------------------------------------------------

    /**
     * @brief Play a sound effect.
     * @param resource_name Name of sf::SoundBuffer in Resource_manager
     * @param bus Audio bus to route through
     * @param opts Playback options
     * @return Play ID for stopping this instance, or -1 if dropped
     */
    int play_sfx(
        std::string const& resource_name,
        Audio_bus const bus = Audio_bus::SFX,
        Sfx_options const& opts = {}
        );

    /**
     * @brief Stop a specific SFX instance.
     * @param play_id ID returned from play_sfx()
     */
    void stop_sfx(int const play_id);

    /**
     * @brief Stop all currently playing SFX.
     */
    void stop_all_sfx();

    // -------------------------------------------------------------------------
    // Music
    // -------------------------------------------------------------------------

    /**
     * @brief Get the music player for direct control.
     * @return Reference to Music_player
     */
    Music_player& music();

    /**
     * @brief Get the music player (const).
     * @return Const reference to Music_player
     */
    Music_player const& music() const;

    // -------------------------------------------------------------------------
    // Spatial audio
    // -------------------------------------------------------------------------

    /**
     * @brief Set listener world position for spatial SFX attenuation.
     * @param world_pos Listener position in world space
     */
    void set_listener_position(sf::Vector2f const world_pos);

    /**
     * @brief Get current listener world position.
     * @return Listener world position
     */
    sf::Vector2f listener_position() const;

    // -------------------------------------------------------------------------
    // Per frame
    // -------------------------------------------------------------------------

    /**
     * @brief Update SFX pool and music crossfades.
     * @param dt Delta time in seconds
     */
    void update(float const dt);

private:
    static constexpr std::size_t const BUS_COUNT = static_cast<std::size_t>(Audio_bus::COUNT);

    resources::Resource_manager& _rm;

    std::array<float, BUS_COUNT> _bus_volume; ///< Raw volume per bus [0, 1]
    std::array<bool,  BUS_COUNT> _bus_muted; ///< Mute state per bus

    Sfx_player   _sfx;
    Music_player _music;

    sf::Vector2f _listener_pos;
};

} // namespace titan::audio
