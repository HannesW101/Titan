// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/config/include/config.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Config
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Config::Config()
    : _root()
    , _sections()
{}

// ----------------------------------------------------------------------------
bool Config::has(
    std::string const& key
    ) const {

    return _root.find(key) != _root.cend();
}

// ----------------------------------------------------------------------------
bool Config::has(
    std::string const& section,
    std::string const& key
    ) const {

    auto const section_it = _sections.find(section);

    if (section_it == _sections.cend()) {

        return false;
    }

    return section_it->second.find(key) != section_it->second.cend();
}

// ----------------------------------------------------------------------------
bool Config::has_section(
    std::string const& section
    ) const {

    return _sections.find(section) != _sections.cend();
}

// ----------------------------------------------------------------------------
void Config::remove(
    std::string const& key
    ) {

    _root.erase(key);
}

// ----------------------------------------------------------------------------
void Config::remove(
    std::string const& section,
    std::string const& key
    ) {

    auto const section_it = _sections.find(section);

    if (section_it != _sections.cend()) {

        section_it->second.erase(key);
    }
}

// ----------------------------------------------------------------------------
void Config::remove_section(
    std::string const& section
    ) {

    _sections.erase(section);
}

// ----------------------------------------------------------------------------
std::vector<std::string> Config::keys() const {

    std::vector<std::string> result;
    result.reserve(_root.size());

    for (auto const& [key, value] : _root) {

        result.push_back(key);
    }

    return result;
}

// ----------------------------------------------------------------------------
std::vector<std::string> Config::keys(
    std::string const& section
    ) const {

    std::vector<std::string> result;
    auto const section_it = _sections.find(section);

    if (section_it == _sections.cend()) {

        return result;
    }

    result.reserve(section_it->second.size());

    for (auto const& [key, value] : section_it->second) {

        result.push_back(key);
    }

    return result;
}

// ----------------------------------------------------------------------------
std::vector<std::string> Config::sections() const {

    std::vector<std::string> result;
    result.reserve(_sections.size());

    for (auto const& [section, values] : _sections) {

        result.push_back(section);
    }

    return result;
}

// ----------------------------------------------------------------------------
void Config::clear() {

    _root.clear();
    _sections.clear();
}

// ----------------------------------------------------------------------------
void Config::merge(
    Config const& other
    ) {

    for (auto const& [key, value] : other._root) {

        _root[key] = value;
    }

    for (auto const& [section, values] : other._sections) {

        for (auto const& [key, value] : values) {

            _sections[section][key] = value;
        }
    }
}

} // namespace titan::core
