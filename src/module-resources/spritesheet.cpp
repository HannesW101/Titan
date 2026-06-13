// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-resources/include/spritesheet.hpp"
#include "module-utils/include/logger.hpp"

#include <stdexcept>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::resources {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Spritesheet
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Spritesheet::Spritesheet(
    sf::Texture texture
    )
    : _texture(std::move(texture))
{}

// ----------------------------------------------------------------------------
void Spritesheet::add_animation(
    Animation const& animation
    ) {

    _animations[animation.name] = animation;
}

// ----------------------------------------------------------------------------
void Spritesheet::set_animation(
    std::string const& name
    ) {


    if (_current_animation == name) {

        return;
    }

    auto it = _animations.find(name);
    if (it == _animations.end()) {

        LOG(Log_lvl::ERR) << "Animation not found: " << name;
        return;
    }

    _current_animation = name;
    _current_frame     = 0;
    _elapsed           = 0.0f;
    _finished          = false;
    _ping_pong_dir     = 1;
}

// ----------------------------------------------------------------------------
void Spritesheet::tick(float const dt) {

    if (_paused || _finished || _current_animation.empty()) {

        return;
    }

    auto it = _animations.find(_current_animation);
    if (it == _animations.end()) {

        return;
    }

    Animation const& anim = it->second;

    if (anim.frames.empty()) {

        return;
    }

    _elapsed += dt;

    float const frame_time = 1.0f / anim.fps;

    while (_elapsed >= frame_time) {

        _elapsed -= frame_time;

        int const frame_count = static_cast<int>(anim.frames.size());

        switch (anim.mode) {

            case Animation_mode::LOOP:
                _current_frame = (_current_frame + 1) % frame_count;
                break;

            case Animation_mode::ONE_SHOT:
                if (_current_frame < frame_count - 1) {

                    _current_frame++;
                } else {

                    _finished = true;
                    return;
                }
                break;

            case Animation_mode::HOLD:
                if (_current_frame < frame_count - 1) {

                    _current_frame++;
                } else {

                    _paused = true;
                    return;
                }
                break;

            case Animation_mode::PING_PONG: {
                _current_frame += _ping_pong_dir;
                if (_current_frame >= frame_count - 1) {

                    _ping_pong_dir = -1;
                } else if (_current_frame <= 0) {

                    _ping_pong_dir = 1;
                }
                break;
            }

            default:
                break;
        }
    }
}

// ----------------------------------------------------------------------------
void Spritesheet::pause() {

    _paused = true;
}

// ----------------------------------------------------------------------------
void Spritesheet::resume() {

    _paused = false;
}

// ----------------------------------------------------------------------------
void Spritesheet::reset() {

    _current_frame = 0;
    _elapsed       = 0.0f;
    _finished      = false;
    _ping_pong_dir = 1;
    _paused        = false;
}

// ----------------------------------------------------------------------------
bool Spritesheet::is_finished() const {

    return _finished;
}

// ----------------------------------------------------------------------------
sf::IntRect Spritesheet::current_frame() const {

    if (_current_animation.empty()) {

        return {};
    }

    auto it = _animations.find(_current_animation);
    if (it == _animations.end()) {

        return {};
    }

    Animation const& anim = it->second;

    if (anim.frames.empty()) {

        return {};
    }

    return anim.frames[static_cast<std::size_t>(_current_frame)];
}

// ----------------------------------------------------------------------------
sf::Texture const& Spritesheet::texture() const {

    return _texture;
}

// ----------------------------------------------------------------------------
std::optional<Animation> Spritesheet::get_animation(std::string const& name) const {

    auto it = _animations.find(name);
    if (it == _animations.end()) {

        return std::nullopt;
    }

    return it->second;
}

// ----------------------------------------------------------------------------
std::string const& Spritesheet::current_animation_name() const {

    return _current_animation;
}

// ----------------------------------------------------------------------------
int Spritesheet::current_frame_index() const {

    return _current_frame;
}

} // namespace titan::resources
