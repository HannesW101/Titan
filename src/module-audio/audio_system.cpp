// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-audio/include/audio_system.hpp"

#include "module-utils/include/math.hpp"
#include "module-utils/include/logger.hpp"

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
// Class Audio_system
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Audio_system::Audio_system(
    resources::Resource_manager& rm
    )
    : _rm          (rm        )
    , _bus_volume  {          }
    , _bus_muted   {          }
    , _sfx         (rm        )
    , _music       (rm        )
    , _listener_pos(0.0f, 0.0f)
    {

    // Default all buses to full volume, unmuted
    _bus_volume.fill(1.0f);
    _bus_muted.fill(false);
}

// ----------------------------------------------------------------------------
void Audio_system::set_bus_volume(
    Audio_bus const bus,
    float     const volume
    ) {

    _bus_volume[static_cast<std::size_t>(bus)] = clamp01(volume);
}

// ----------------------------------------------------------------------------
float Audio_system::bus_volume(
    Audio_bus const bus
    ) const {

    return _bus_volume[static_cast<std::size_t>(bus)];
}

// ----------------------------------------------------------------------------
float Audio_system::effective_volume(
    Audio_bus const bus
    ) const {

    if (is_bus_muted(bus) || is_bus_muted(Audio_bus::MASTER)) {

        return 0.0f;
    }

    float const master = _bus_volume[static_cast<std::size_t>(Audio_bus::MASTER)];
    float const raw    = _bus_volume[static_cast<std::size_t>(bus              )];

    return clamp01(master * raw);
}

// ----------------------------------------------------------------------------
void Audio_system::set_bus_muted(
    Audio_bus const bus,
    bool      const muted
    ) {

    _bus_muted[static_cast<std::size_t>(bus)] = muted;
}

// ----------------------------------------------------------------------------
bool Audio_system::is_bus_muted(
    Audio_bus const bus
    ) const {

    return _bus_muted[static_cast<std::size_t>(bus)];
}

// ----------------------------------------------------------------------------
void Audio_system::load_mix(
    core::Config const& config
    ) {

    auto load_bus = [&](Audio_bus const bus, std::string const& key) {

        if (auto vol = config.get<float>("audio", key + "_volume")) {

            set_bus_volume(bus, *vol);
        }

        if (auto muted = config.get<bool>("audio", key + "_muted")) {

            set_bus_muted(bus, *muted);
        }
    };

    load_bus(Audio_bus::MASTER,  "master" );
    load_bus(Audio_bus::MUSIC,   "music"  );
    load_bus(Audio_bus::SFX,     "sfx"    );
    load_bus(Audio_bus::UI,      "ui"     );
    load_bus(Audio_bus::VOICE,   "voice"  );
    load_bus(Audio_bus::AMBIENT, "ambient");
}

// ----------------------------------------------------------------------------
void Audio_system::save_mix(
    core::Config& config
    ) const {

    auto save_bus = [&](Audio_bus const bus, std::string const& key) {

        config.set("audio", key + "_volume", bus_volume  (bus));
        config.set("audio", key + "_muted",  is_bus_muted(bus));
    };

    save_bus(Audio_bus::MASTER,  "master" );
    save_bus(Audio_bus::MUSIC,   "music"  );
    save_bus(Audio_bus::SFX,     "sfx"    );
    save_bus(Audio_bus::UI,      "ui"     );
    save_bus(Audio_bus::VOICE,   "voice"  );
    save_bus(Audio_bus::AMBIENT, "ambient");
}

// ----------------------------------------------------------------------------
int Audio_system::play_sfx(
    std::string const& resource_name,
    Audio_bus   const  bus,
    Sfx_options const& opts
    ) {

    float const vol = effective_volume(bus);

    if (vol <= 0.0f) { return -1; }

    // Scale opts volume by effective bus volume
    Sfx_options scaled_opts = opts;
    scaled_opts.volume      = opts.volume * vol;

    return _sfx.play(resource_name, scaled_opts);
}

// ----------------------------------------------------------------------------
void Audio_system::stop_sfx(
    int const play_id
    ) {

    _sfx.stop(play_id);
}

// ----------------------------------------------------------------------------
void Audio_system::stop_all_sfx() {

    _sfx.stop_all();
}

// ----------------------------------------------------------------------------
Music_player& Audio_system::music() {

    return _music;
}

// ----------------------------------------------------------------------------
Music_player const& Audio_system::music() const {

    return _music;
}

// ----------------------------------------------------------------------------
void Audio_system::set_listener_position(
    sf::Vector2f const world_pos
    ) {

    _listener_pos = world_pos;
}

// ----------------------------------------------------------------------------
sf::Vector2f Audio_system::listener_position() const {

    return _listener_pos;
}

// ----------------------------------------------------------------------------
void Audio_system::update(
    float const dt
    ) {

    _sfx.update  (_listener_pos, effective_volume(Audio_bus::SFX  ));
    _music.update(dt,            effective_volume(Audio_bus::MUSIC));
}

} // namespace titan::audio
