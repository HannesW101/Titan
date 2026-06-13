// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-resources/include/resource_manager.hpp"
#include "module-utils/include/logger.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::resources {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Resource_manager
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Resource_manager& Resource_manager::instance() {

    static Resource_manager instance;
    return instance;
}

// ----------------------------------------------------------------------------
void Resource_manager::unload(
    std::string const& name
    ) {

    _resources.erase(name);
    _music.erase(name);
}

// ----------------------------------------------------------------------------
void Resource_manager::flush_unused() {

    for (auto it = _resources.begin(); it != _resources.end();) {

        // use_count == 1 means only the manager holds a reference
        if (it->second.use_count() == 1) {

            it = _resources.erase(it);
        } else {

            ++it;
        }
    }
}

// ----------------------------------------------------------------------------
void Resource_manager::clear() {

    stop_all_music();
    _resources.clear();
    _music.clear();
}

// ----------------------------------------------------------------------------
void Resource_manager::play_music(
    std::string const& name,
    bool const loop
    ) {

    auto it = _music.find(name);
    if (it == _music.end()) {

        LOG(Log_lvl::ERR) << "Music not found: " + name;
    }

    it->second->setLooping(loop);
    it->second->play();
}

// ----------------------------------------------------------------------------
void Resource_manager::stop_music(
    std::string const& name
    ) {

    auto it = _music.find(name);
    if (it == _music.end()) {

        LOG(Log_lvl::ERR) << "Music not found: " + name;
    }

    it->second->stop();
}

// ----------------------------------------------------------------------------
void Resource_manager::pause_music(
    std::string const& name
    ) {

    auto it = _music.find(name);
    if (it == _music.end()) {

        LOG(Log_lvl::ERR) << "Music not found: " + name;
    }

    it->second->pause();
}

// ----------------------------------------------------------------------------
void Resource_manager::resume_music(
    std::string const& name
    ) {

    auto it = _music.find(name);
    if (it == _music.end()) {

        LOG(Log_lvl::ERR) << "Music not found: " + name;
    }

    it->second->play();
}

// ----------------------------------------------------------------------------
void Resource_manager::set_music_volume(
    std::string const& name,
    float const volume
    ) {

    auto it = _music.find(name);
    if (it == _music.end()) {

        LOG(Log_lvl::ERR) << "Music not found: " + name;
    }

    it->second->setVolume(volume);
}

// ----------------------------------------------------------------------------
void Resource_manager::stop_all_music() {

    for (auto& [name, music] : _music) {

        music->stop();
    }
}

} // namespace titan::resources
