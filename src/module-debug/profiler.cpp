// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-debug/include/profiler.hpp"

#include "module-utils/include/math.hpp"

#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::debug {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Profiler
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Profiler::Profiler()
    : _frame_start(Clock::now())
{}

// ----------------------------------------------------------------------------
Profiler& Profiler::instance() {

    static Profiler s_instance;
    return s_instance;
}

// ============================================================================
// Frame timing
// ============================================================================

// ----------------------------------------------------------------------------
void Profiler::begin_frame() {

    if (!_enabled) { return; }

    _frame_start = Clock::now();
}

// ----------------------------------------------------------------------------
void Profiler::end_frame() {

    if (!_enabled) { return; }

    auto const now                                         = Clock::now();
    std::chrono::duration<float, std::milli> const elapsed = now - _frame_start;
    _last_frame_ms                                         = elapsed.count();

    // Exponential moving average for a stable on-screen reading.
    if (_avg_frame_ms <= 0.0f) {

        _avg_frame_ms = _last_frame_ms;
    } else {

        _avg_frame_ms = lerp(_avg_frame_ms, _last_frame_ms, _smoothing);
    }

    ++_frame_index;
}

// ----------------------------------------------------------------------------
float Profiler::fps() const {

    return (_avg_frame_ms > 0.0f) ? (1000.0f / _avg_frame_ms) : 0.0f;
}

// ----------------------------------------------------------------------------
float Profiler::average_frame_ms   () const { return _avg_frame_ms;  }
float Profiler::last_frame_ms      () const { return _last_frame_ms; }
std::uint64_t Profiler::frame_index() const { return _frame_index;   }

// ============================================================================
// Named scopes
// ============================================================================

// ----------------------------------------------------------------------------
void Profiler::begin_scope(
    std::string const& name
    ) {

    if (!_enabled) { return; }

    Scope_data& s = _scopes[name];
    s.start       = Clock::now();
    s.open        = true;
}

// ----------------------------------------------------------------------------
void Profiler::end_scope(
    std::string const& name
    ) {

    if (!_enabled) { return; }

    auto const it = _scopes.find(name);

    if (it == _scopes.cend() || !it->second.open) { return; }

    auto const now = Clock::now();
    std::chrono::duration<float, std::milli> const elapsed = now - it->second.start;
    it->second.last_ms                                     = elapsed.count();
    it->second.open                                        = false;
}

// ----------------------------------------------------------------------------
float Profiler::scope_ms(
    std::string const& name
    ) const {

    auto const it = _scopes.find(name);

    return (it != _scopes.cend()) ? it->second.last_ms : 0.0f;
}

// ----------------------------------------------------------------------------
std::vector<std::pair<std::string, float>> Profiler::scopes() const {

    std::vector<std::pair<std::string, float>> out;
    out.reserve(_scopes.size());

    for (auto const& [name, data] : _scopes) {

        out.emplace_back(name, data.last_ms);
    }

    return out;
}

// ============================================================================
// Config
// ============================================================================

// ----------------------------------------------------------------------------
void Profiler::set_smoothing(float const factor )       { _smoothing = clamp01(factor); }
void Profiler::set_enabled  (bool  const enabled)       { _enabled = enabled;           }
bool Profiler::is_enabled   (                   ) const { return _enabled;              }

// ----------------------------------------------------------------------------
void Profiler::reset() {

    _scopes.clear();

    _avg_frame_ms  = 0.0f;
    _last_frame_ms = 0.0f;
    _frame_index   = 0u;
}

// ============================================================================
// Profile_scope (RAII)
// ============================================================================

// ----------------------------------------------------------------------------
Profile_scope::Profile_scope(
    Profiler& profiler,
    std::string name
    )
    : _profiler(profiler       )
    , _name    (std::move(name))
    {

    _profiler.begin_scope(_name);
}

// ----------------------------------------------------------------------------
Profile_scope::~Profile_scope() {

    _profiler.end_scope(_name);
}

} // namespace titan::debug
