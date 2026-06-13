// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/animator.hpp"
#include "module-game/components/include/sprite_renderer.hpp"
#include "module-game/include/game_object.hpp"

#include "module-resources/include/resource_manager.hpp"
#include "module-resources/include/spritesheet.hpp"

#include <exception>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Animator
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void Animator::set_spritesheet(
    std::string const& name
    ) {

    _sheet_name = name;
    _sheet.reset();

    if (has_owner()) { _acquire(); }
}

// ----------------------------------------------------------------------------
void Animator::add_animation(
    resources::Animation const& animation
    ) {

    if (!_sheet) { _acquire(); }

    if (_sheet) { _sheet->add_animation(animation); }
}

// ----------------------------------------------------------------------------
void Animator::play(
    std::string const& name
    ) {

    _current = name;

    if (_sheet) {

        _sheet->set_animation(name);
    } else {

        _pending_play = name;
    }
}

// ----------------------------------------------------------------------------
void Animator::pause () { if (_sheet) { _sheet->pause();  } }
void Animator::resume() { if (_sheet) { _sheet->resume(); } }
void Animator::reset () { if (_sheet) { _sheet->reset();  } }

// ----------------------------------------------------------------------------
bool               Animator::is_finished      () const { return _sheet ? _sheet->is_finished() : false; }
std::string const& Animator::current_animation() const { return _current;                               }

// ----------------------------------------------------------------------------
void Animator::_acquire() {

    if (_sheet_name.empty()) { return; }

    try {

        _sheet = resources::Resource_manager::instance().get<resources::Spritesheet>(_sheet_name);
    } catch (std::exception const&) {

        _sheet = nullptr;
    }

    if (_sheet && !_pending_play.empty()) {

        _sheet->set_animation(_pending_play);
        _pending_play.clear();
    }
}

// ----------------------------------------------------------------------------
void Animator::on_start() {

    _acquire();
    _renderer = owner().get_component<Sprite_renderer>();
}

// ----------------------------------------------------------------------------
void Animator::update(
    float const dt
    ) {

    if (!_sheet) {

        _acquire();

        if (!_sheet) { return; }
    }

    if (!_renderer) { _renderer = owner().get_component<Sprite_renderer>(); }

    _sheet->tick(dt);

    if (_renderer) {

        _renderer->set_texture_rect(_sheet->current_frame());
    }
}

} // namespace titan::game
