// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/audio_emitter.hpp"
#include "module-game/include/game_object.hpp"
#include "module-game/components/include/transform.hpp"

#include "module-audio/include/audio_system.hpp"
#include "module-audio/include/sfx_options.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Audio_emitter
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void Audio_emitter::set_audio_system(audio::Audio_system& system) { _system = &system; }

// ----------------------------------------------------------------------------
void             Audio_emitter::set_bus(audio::Audio_bus const bus)       { _bus = bus;  }
audio::Audio_bus Audio_emitter::bus    (                          ) const { return _bus; }

// ----------------------------------------------------------------------------
void  Audio_emitter::set_falloff_radius(float const radius)       { _falloff_radius = radius; }
float Audio_emitter::falloff_radius    (                  ) const { return _falloff_radius;   }

// ----------------------------------------------------------------------------
void Audio_emitter::set_spatial(bool const spatial)       { _spatial = spatial; }
bool Audio_emitter::is_spatial (                  ) const { return _spatial;    }

// ----------------------------------------------------------------------------
int Audio_emitter::play(
    std::string const& resource_name
    ) {

    if (!_system) { return -1; }

    audio::Sfx_options opts;
    opts.falloff_radius = _falloff_radius;

    if (_spatial) {

        opts.world_position = owner().transform().world_position();
    }

    return _system->play_sfx(resource_name, _bus, opts);
}

// ----------------------------------------------------------------------------
int Audio_emitter::play(
    std::string const& resource_name,
    float       const  volume,
    float       const  pitch
    ) {

    if (!_system) { return -1; }

    audio::Sfx_options opts;
    opts.volume         = volume;
    opts.pitch          = pitch;
    opts.falloff_radius = _falloff_radius;

    if (_spatial) {

        opts.world_position = owner().transform().world_position();
    }

    return _system->play_sfx(resource_name, _bus, opts);
}

} // namespace titan::game
