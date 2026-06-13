#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <optional>
#include <string>
#include <memory>
#include "module-audio/include/sfx_options.hpp"

#include "module-resources/include/resource_manager.hpp"

#include "SFML/Audio.hpp"

#include <vector>
#include <cstddef>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Class Sfx_player
// ----------------------------------------------------------------------------

/**
 * @brief Pooled sound effect player.
 *
 * Maintains a fixed pool of sf::Sound instances. play() finds a free slot,
 * applies options, and starts playback. If the pool is full, the oldest
 * playing sound is evicted to make room.
 *
 * Spatial attenuation is calculated manually in 2D against the listener
 * position updated each frame by Audio_system.
 */
class Sfx_player final {

public:
    /**
     * @brief Construct with resource manager and pool size.
     * @param rm Resource manager for resolving sound buffers
     * @param pool_size Maximum simultaneous sounds (default 32)
     */
    explicit Sfx_player(
        resources::Resource_manager& rm,
        std::size_t const            pool_size = 32u
        );

    ~Sfx_player() = default;

    Sfx_player           (Sfx_player const&) = delete;
    Sfx_player& operator=(Sfx_player const&) = delete;
    Sfx_player           (Sfx_player&&     ) = delete;
    Sfx_player& operator=(Sfx_player&&     ) = delete;

    // -------------------------------------------------------------------------
    // Playback
    // -------------------------------------------------------------------------

    /**
     * @brief Play a sound buffer by resource name.
     * @param resource_name Name of the sf::SoundBuffer in Resource_manager
     * @param opts Playback options
     * @return Play ID for stopping this instance, or -1 if dropped
     */
    int play(
        std::string const& resource_name,
        Sfx_options const& opts = {}
        );

    /**
     * @brief Stop a specific sound instance.
     * @param play_id ID returned from play()
     */
    void stop(int const play_id);

    /**
     * @brief Stop all currently playing sounds.
     */
    void stop_all();

    // -------------------------------------------------------------------------
    // Per frame
    // -------------------------------------------------------------------------

    /**
     * @brief Update all slots - apply spatial attenuation, free finished sounds.
     * @param listener_pos Current listener world position
     * @param effective_volume Combined bus volume (master * sfx bus)
     */
    void update(
        sf::Vector2f const listener_pos,
        float        const effective_volume
        );

private:
    struct Slot {

        int                              id      = -1;
        std::shared_ptr<sf::SoundBuffer> buffer  = {}; ///< Keep buffer alive while sound plays
        std::optional<sf::Sound>         sound   = std::nullopt; ///< Constructed with buffer, reset when free
        Sfx_options                      options = {};
        bool                             in_use  = false;
        int                              age     = 0;
    };

    /**
     * @brief Find a free slot or evict the oldest active one.
     * @return Pointer to slot to use
     */
    Slot* _acquire_slot();

    resources::Resource_manager& _rm;
    std::vector<Slot>            _pool;
    int                          _next_id = 0;
};

} // namespace titan::audio
