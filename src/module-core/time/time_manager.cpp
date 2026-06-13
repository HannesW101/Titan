// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/time/include/time_manager.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Time_manager
// ----------------------------------------------------------------------------

Time_manager::Time_manager()
    : _timers()
    {

    _timers.emplace("main", Timer{});
}

// ----------------------------------------------------------------------------
Time_manager& Time_manager::instance() {

    static Time_manager instance;
    return instance;
}

// ----------------------------------------------------------------------------
Timer& Time_manager::get_timer(
    std::string_view const name
    ) {

    auto const it = _timers.find(std::string(name));

    if (it != _timers.cend()) {

        return it->second;
    }

    return _timers.emplace(name, Timer{}).first->second;
}

// ----------------------------------------------------------------------------
std::optional<std::reference_wrapper<Timer>> Time_manager::try_get_timer(
    std::string_view const name
    ) {

    auto const it = _timers.find(std::string(name));

    if (it != _timers.cend()) {

        return std::ref(it->second);
    }

    return std::nullopt;
}

// ----------------------------------------------------------------------------
bool Time_manager::has_timer(
    std::string_view const name
    ) const {

    return _timers.find(std::string(name)) != _timers.cend();
}

// ----------------------------------------------------------------------------
void Time_manager::remove_timer(
    std::string_view const name
    ) {

    _timers.erase(std::string(name));
}

// ----------------------------------------------------------------------------
void Time_manager::tick_all() {

    for (auto& [name, timer] : _timers) {

        timer.tick();
    }
}

// ----------------------------------------------------------------------------
void Time_manager::pause_all() {

    for (auto& [name, timer] : _timers) {

        timer.pause();
    }
}

// ----------------------------------------------------------------------------
void Time_manager::resume_all() {

    for (auto& [name, timer] : _timers) {

        timer.resume();
    }
}

// ----------------------------------------------------------------------------
Timer& Time_manager::main() {

    return _timers.at("main");
}

} // namespace titan::core
