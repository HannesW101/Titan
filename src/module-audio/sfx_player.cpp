// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-audio/include/sfx_player.hpp"

#include "module-utils/include/math.hpp"
#include "module-utils/include/random.hpp"
#include "module-utils/include/logger.hpp"

#include <algorithm>
#include <stdexcept>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Sfx_player
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Sfx_player::Sfx_player(
    resources::Resource_manager& rm,
    std::size_t const            pool_size
    )
    : _rm     (rm       )
    , _pool   (pool_size)
    , _next_id(0        )
{}

// ----------------------------------------------------------------------------
int Sfx_player::play(
    std::string const& resource_name,
    Sfx_options const& opts
    ) {

    std::shared_ptr<sf::SoundBuffer> buffer;

    try {

        buffer = _rm.get<sf::SoundBuffer>(resource_name);
    } catch (std::runtime_error const&) {

        LOG(Log_lvl::WARN) << "Sfx_player: sound not loaded: " << resource_name;
        return -1;
    }

    Slot* slot    = _acquire_slot();
    slot->id      = _next_id++;
    slot->options = opts;
    slot->in_use  = true;
    slot->age     = 0;
    slot->buffer  = buffer;
    slot->sound.emplace(*buffer); // construct sf::Sound with buffer

    float pitch = opts.pitch;

    if (opts.pitch_jitter > 0.0f) {

        pitch += Random::global().range(-opts.pitch_jitter, opts.pitch_jitter);
    }

    slot->sound->setPitch(pitch);
    slot->sound->setLooping(opts.loop);
    slot->sound->setVolume(opts.volume * 100.0f);
    slot->sound->play();

    return slot->id;
}

// ----------------------------------------------------------------------------
void Sfx_player::stop(
    int const play_id
    ) {

    for (auto& slot : _pool) {

        if (slot.in_use && slot.id == play_id) {

            slot.sound->stop();
            slot.sound.reset();
            slot.buffer.reset();
            slot.in_use = false;
            return;
        }
    }
}

// ----------------------------------------------------------------------------
void Sfx_player::stop_all() {

    for (auto& slot : _pool) {

        if (slot.in_use) {

            slot.sound->stop();
            slot.sound.reset();
            slot.buffer.reset();
            slot.in_use = false;
        }
    }
}

// ----------------------------------------------------------------------------
void Sfx_player::update(
    sf::Vector2f const listener_pos,
    float        const effective_volume
    ) {

    for (auto& slot : _pool) {

        if (!slot.in_use) { continue; }

        if (slot.sound->getStatus() == sf::Sound::Status::Stopped) {

            slot.sound.reset();
            slot.buffer.reset();
            slot.in_use = false;
            continue;
        }

        ++slot.age;

        float vol = slot.options.volume * effective_volume;

        if (slot.options.world_position.has_value()) {

            float const dist  = distance(listener_pos, *slot.options.world_position);
            float const t     = clamp01(1.0f - (dist / slot.options.falloff_radius));
            vol              *= t;
        }

        slot.sound->setVolume(clamp01(vol) * 100.0f);
    }
}

// ----------------------------------------------------------------------------
Sfx_player::Slot* Sfx_player::_acquire_slot() {

    // First pass: find a free slot
    for (auto& slot : _pool) {

        if (!slot.in_use) {

            return &slot;
        }
    }

    // Pool full: evict the oldest slot
    Slot* oldest = _pool.data();

    for (auto& slot : _pool) {

        if (slot.age > oldest->age) {

            oldest = &slot;
        }
    }

    LOG(Log_lvl::WARN) << "Sfx_player: pool full, evicting oldest sound (id=" << oldest->id << ")";

    oldest->sound->stop();
    oldest->sound.reset();
    oldest->buffer.reset();
    oldest->in_use = false;

    return oldest;
}

} // namespace titan::audio
