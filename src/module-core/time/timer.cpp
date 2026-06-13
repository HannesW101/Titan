// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/time/include/timer.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Timer
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Timer::Timer()
    : _real_clock       (            )
    , _delta            (0.0f        )
    , _unscaled_delta   (0.0f        )
    , _total            (0.0         )
    , _unscaled_total   (0.0         )
    , _frame_count      (0u          )
    , _time_scale       (1.0f        )
    , _max_delta        (0.25f       )
    , _fixed_dt         (1.0f / 60.0f)
    , _fixed_accumulator(0.0f        )
    , _paused           (false       )
{}

// ----------------------------------------------------------------------------
void Timer::tick() {

    float const raw_delta = _real_clock.restart().asSeconds();
    _unscaled_delta       = std::min(raw_delta, _max_delta);

    if (_paused) {

        _delta = 0.0f;
    } else {

        _delta             =  _unscaled_delta * _time_scale;
        _total             += static_cast<double>(_delta);
        _fixed_accumulator += _delta;
    }

    _unscaled_total += static_cast<double>(_unscaled_delta);
    ++_frame_count;
}

// ----------------------------------------------------------------------------
void Timer::reset() {

    _real_clock.restart();

    _delta             = 0.0f;
    _unscaled_delta    = 0.0f;
    _total             = 0.0;
    _unscaled_total    = 0.0;
    _frame_count       = 0u;
    _fixed_accumulator = 0.0f;
}

// ----------------------------------------------------------------------------
void Timer::pause() {

    _paused = true;
}

// ----------------------------------------------------------------------------
void Timer::resume() {

    _paused = false;
    _real_clock.restart();
}

// ----------------------------------------------------------------------------
bool Timer::is_paused() const {

    return _paused;
}

// ----------------------------------------------------------------------------
float Timer::delta_time() const {

    return _delta;
}

// ----------------------------------------------------------------------------
float Timer::unscaled_delta_time() const {

    return _unscaled_delta;
}

// ----------------------------------------------------------------------------
double Timer::total_time() const {

    return _total;
}

// ----------------------------------------------------------------------------
double Timer::unscaled_total_time() const {

    return _unscaled_total;
}

// ----------------------------------------------------------------------------
std::uint64_t Timer::frame_count() const {

    return _frame_count;
}

// ----------------------------------------------------------------------------
void Timer::set_time_scale(
    float const scale
    ) {

    _time_scale = std::max(0.0f, scale);
}

// ----------------------------------------------------------------------------
float Timer::time_scale() const {

    return _time_scale;
}

// ----------------------------------------------------------------------------
void Timer::set_max_delta(
    float const seconds
    ) {

    _max_delta = std::max(0.0f, seconds);
}

// ----------------------------------------------------------------------------
float Timer::max_delta() const {

    return _max_delta;
}

// ----------------------------------------------------------------------------
void Timer::set_fixed_dt(
    float const seconds
    ) {

    _fixed_dt = std::max(0.0f, seconds);
}

// ----------------------------------------------------------------------------
float Timer::fixed_dt() const {

    return _fixed_dt;
}

// ----------------------------------------------------------------------------
bool Timer::consume_fixed_step() {

    if (_fixed_accumulator >= _fixed_dt) {

        _fixed_accumulator -= _fixed_dt;
        return true;
    }

    return false;
}

} // namespace titan::core
