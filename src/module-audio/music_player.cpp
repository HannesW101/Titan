// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-audio/include/music_player.hpp"

#include "module-utils/include/math.hpp"
#include "module-utils/include/easing.hpp"
#include "module-utils/include/logger.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Music_player
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Music_player::Music_player(
    resources::Resource_manager& rm
    )
    : _rm      (rm   )
    , _active  {     }
    , _outgoing{     }
    , _paused  (false)
{}

// ----------------------------------------------------------------------------
void Music_player::play(
    std::string const& resource_name,
    bool        const  loop
    ) {

    _kill_outgoing();

    if (_active.music) {

        _active.music->stop();
    }

    _active               = {};
    _active.resource_name = resource_name;
    _active.fading        = false;
    _active.volume_start  = 1.0f;
    _active.volume_target = 1.0f;

    // Resolve through resource manager
    _active.music = _rm.get<sf::Music>(resource_name);
    _active.music->setLooping(loop);
    _active.music->setVolume(100.0f);
    _active.music->play();

    _paused = false;
}

// ----------------------------------------------------------------------------
void Music_player::crossfade_to(
    std::string const& resource_name,
    float       const  seconds,
    bool        const  loop
    ) {

    // Move active to outgoing for fade-out
    if (_active.music && _active.music->getStatus() == sf::Music::Status::Playing) {

        _kill_outgoing();
        _outgoing               = std::move(_active);
        _outgoing.fading        = true;
        _outgoing.volume_start  = 1.0f;
        _outgoing.volume_target = 0.0f;
        _outgoing.fade_duration = seconds;
        _outgoing.fade_elapsed  = 0.0f;
    }

    // Set up new active fading in
    _active               = {};
    _active.resource_name = resource_name;
    _active.fading        = true;
    _active.volume_start  = 0.0f;
    _active.volume_target = 1.0f;
    _active.fade_duration = seconds;
    _active.fade_elapsed  = 0.0f;

    // Resolve through resource manager
    _active.music = _rm.get<sf::Music>(resource_name);
    _active.music->setLooping(loop);
    _active.music->setVolume(0.0f);
    _active.music->play();

    _paused = false;
}

// ----------------------------------------------------------------------------
void Music_player::stop() {

    _kill_outgoing();

    if (_active.music) {

        _active.music->stop();
    }

    _active = {};
    _paused = false;
}

// ----------------------------------------------------------------------------
void Music_player::pause() {

    if (_active.music  ) { _active.music->pause();   }
    if (_outgoing.music) { _outgoing.music->pause(); }

    _paused = true;
}

// ----------------------------------------------------------------------------
void Music_player::resume() {

    if (_active.music  ) { _active.music->play();   }
    if (_outgoing.music) { _outgoing.music->play(); }

    _paused = false;
}

// ----------------------------------------------------------------------------
bool Music_player::is_playing() const {

    if (!_active.music) { return false; }

    return _active.music->getStatus() == sf::Music::Status::Playing;
}

// ----------------------------------------------------------------------------
std::string const& Music_player::current_track() const {

    return _active.resource_name;
}

// ----------------------------------------------------------------------------
void Music_player::update(
    float const dt,
    float const effective_volume
    ) {

    if (_paused) { return; }

    // --- Active slot ---
    if (_active.music) {

        float vol = 1.0f;

        if (_active.fading && _active.fade_duration > 0.0f) {

            _active.fade_elapsed += dt;

            float const t     = clamp01    (_active.fade_elapsed / _active.fade_duration      );
            float const eased = in_out_sine(t                                                 );
            vol               = lerp       (_active.volume_start, _active.volume_target, eased);

            if (_active.fade_elapsed >= _active.fade_duration) {

                vol            = _active.volume_target;
                _active.fading = false;
            }
        }

        _active.music->setVolume(clamp01(vol * effective_volume) * 100.0f);
    }

    // --- Outgoing slot ---
    if (_outgoing.music) {

        float vol = 0.0f;

        if (_outgoing.fading && _outgoing.fade_duration > 0.0f) {

            _outgoing.fade_elapsed += dt;

            float const t     = clamp01    (_outgoing.fade_elapsed / _outgoing.fade_duration      );
            float const eased = in_out_sine(t                                                     );
            vol               = lerp       (_outgoing.volume_start, _outgoing.volume_target, eased);
        }

        _outgoing.music->setVolume(clamp01(vol * effective_volume) * 100.0f);

        if (_outgoing.fade_elapsed >= _outgoing.fade_duration) {

            _kill_outgoing();
        }
    }
}

// ----------------------------------------------------------------------------
void Music_player::_kill_outgoing() {

    if (_outgoing.music) {

        _outgoing.music->stop();
        _outgoing = {};
    }
}

} // namespace titan::audio
