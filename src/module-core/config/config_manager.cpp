// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/config/include/config_manager.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Config_manager
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Config_manager::Config_manager()
    : _configs()
{}

// ----------------------------------------------------------------------------
Config_manager& Config_manager::instance() {

    static Config_manager instance;
    return instance;
}

// ----------------------------------------------------------------------------
Config& Config_manager::get(
    std::string const& name
    ) {

    auto const it = _configs.find(name);

    if (it != _configs.end()) {

        return it->second;
    }

    return _configs.emplace(name, Config{}).first->second;
}

// ----------------------------------------------------------------------------
bool Config_manager::has(
    std::string const& name
    ) const {

    return _configs.find(name) != _configs.cend();
}

// ----------------------------------------------------------------------------
void Config_manager::remove(
    std::string const& name
    ) {

    _configs.erase(name);
}

// ----------------------------------------------------------------------------
void Config_manager::load(
    std::string           const& name,
    std::filesystem::path const& path,
    Config_format         const  format
    ) {

    _configs[name] = load_config(path, format);
}

// ----------------------------------------------------------------------------
void Config_manager::load_auto(
    std::string           const& name,
    std::filesystem::path const& path
    ) {

    _configs[name] = load_config_auto(path);
}

// ----------------------------------------------------------------------------
void Config_manager::save(
    std::string           const& name,
    std::filesystem::path const& path,
    Config_format         const  format
    ) {

    auto const it = _configs.find(name);

    if (it != _configs.cend()) {

        save_config(it->second, path, format);
    }
}

// ----------------------------------------------------------------------------
void Config_manager::save_auto(
    std::string           const& name,
    std::filesystem::path const& path
    ) {

    auto const it = _configs.find(name);

    if (it != _configs.cend()) {

        save_config_auto(it->second, path);
    }
}

// ----------------------------------------------------------------------------
void Config_manager::clear() {

    _configs.clear();
}

} // namespace titan::core
