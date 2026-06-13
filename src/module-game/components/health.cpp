// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/health.hpp"
#include "module-game/include/game_object.hpp"

#include <algorithm>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// CLass Health
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Health::Health(
    int const max_hp
    )
    : _max    (std::max(1, max_hp))
    , _current(_max               )
{}

// ----------------------------------------------------------------------------
int  Health::current () const { return _current; }
int  Health::max     () const { return _max;     }
bool Health::is_alive() const { return !_dead;   }
bool Health::is_dead () const { return _dead;    }

// ----------------------------------------------------------------------------
void Health::set_max(
    int  const max_hp,
    bool const refill
    ) {

    _max = std::max(1, max_hp);

    if (refill) {

        _current = _max;
        _dead    = false;
    } else {

        _current = std::min(_current, _max);
    }
}

// ----------------------------------------------------------------------------
void Health::damage(
    int const amount
    ) {

    if (_dead || amount <= 0) { return; }

    _current = std::max(0, _current - amount);

    if (_on_damage) { _on_damage(owner(), amount); }

    _check_death();
}

// ----------------------------------------------------------------------------
void Health::heal(
    int const amount
    )
     {
    if (_dead || amount <= 0) { return; }

    _current = std::min(_max, _current + amount);

    if (_on_heal) { _on_heal(owner(), amount); }
}

// ----------------------------------------------------------------------------
void Health::revive(
    int const to_hp
    ) {

    _dead    = false;
    _current = (to_hp < 0) ? _max : std::clamp(to_hp, 1, _max);
}

// ----------------------------------------------------------------------------
void Health::set_destroy_on_death(bool const value)       { _destroy_on_death = value; }
bool Health::destroy_on_death    (                ) const { return _destroy_on_death;  }

// ----------------------------------------------------------------------------
void Health::set_on_damage(Amount_fn fn) { _on_damage = std::move(fn); }
void Health::set_on_heal  (Amount_fn fn) { _on_heal   = std::move(fn); }
void Health::set_on_death (Health_fn fn) { _on_death  = std::move(fn); }

// ----------------------------------------------------------------------------
void Health::_check_death() {

    if (_current > 0 || _dead) { return; }

    _dead = true;

    if (_on_death) { _on_death(owner()); }

    if (_destroy_on_death) { owner().destroy(); }
}

} // namespace titan::game
