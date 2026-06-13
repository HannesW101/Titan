#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>
#include <any>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

/**
 * @brief Variant type holding configuration values.
 *
 * Supports common data types used in configuration files.
 */
using Config_value = std::variant<
    bool,
    int,
    long,
    float,
    double,
    std::string
    >;

// ============================================================================
// Concepts
// ----------------------------------------------------------------------------

/**
 * @brief Concept that matches if T is one of the allowed config value types.
 *
 * Automatically strips cv-qualifiers and references, so "const int&" works.
 */
template<typename T>
concept ConfigValueType =
    (std::is_same_v<std::remove_cvref_t<T>, bool>   ||
     std::is_same_v<std::remove_cvref_t<T>, int>    ||
     std::is_same_v<std::remove_cvref_t<T>, long>   ||
     std::is_same_v<std::remove_cvref_t<T>, float>  ||
     std::is_same_v<std::remove_cvref_t<T>, double> ||
     std::is_same_v<std::remove_cvref_t<T>, std::string>
    );

// ============================================================================
// Class Config
// ----------------------------------------------------------------------------

/**
 * @brief Hierarchical configuration with sections and key-value pairs.
 *
 * Supports nested sections for organizing related settings.
 * Values can be primitives or strings.
 */
class Config final {

public:
    /**
     * @brief Construct empty configuration.
     */
    Config();

    /**
     * @brief Set a value in the root section.
     * @tparam T Value type
     * @param key Parameter name
     * @param value Parameter value
     */
    template<ConfigValueType T>
    void set(std::string const& key, T const& value);

    /**
     * @brief Set a value in a specific section path.
     * @tparam T Value type
     * @param section Section path (e.g., "graphics.display")
     * @param key Parameter name
     * @param value Parameter value
     */
    template<ConfigValueType T>
    void set(std::string const& section, std::string const& key, T const& value);

    /**
     * @brief Get a value from root section.
     * @tparam T Expected value type
     * @param key Parameter name
     * @return Optional value if found and correct type
     */
    template<ConfigValueType T>
    std::optional<T> get(std::string const& key) const;

    /**
     * @brief Get a value from specific section.
     * @tparam T Expected value type
     * @param section Section path
     * @param key Parameter name
     * @return Optional value if found and correct type
     */
    template<ConfigValueType T>
    std::optional<T> get(std::string const& section, std::string const& key) const;

    /**
     * @brief Get value with default fallback.
     * @tparam T Value type
     * @param key Parameter name
     * @param default_value Value returned if key not found
     * @return Value or default
     */
    template<ConfigValueType T>
    T get_or(std::string const& key, T const& default_value) const;

    /**
     * @brief Get value with default fallback from section.
     * @tparam T Value type
     * @param section Section path
     * @param key Parameter name
     * @param default_value Value returned if key not found
     * @return Value or default
     */
    template<ConfigValueType T>
    T get_or(std::string const& section, std::string const& key, T const& default_value) const;

    /**
     * @brief Check if key exists in root section.
     * @param key Parameter name
     * @return True if key exists
     */
    bool has(std::string const& key) const;

    /**
     * @brief Check if key exists in section.
     * @param section Section path
     * @param key Parameter name
     * @return True if key exists
     */
    bool has(std::string const& section, std::string const& key) const;

    /**
     * @brief Check if section exists.
     * @param section Section path
     * @return True if section exists
     */
    bool has_section(std::string const& section) const;

    /**
     * @brief Remove key from root section.
     * @param key Parameter name
     */
    void remove(std::string const& key);

    /**
     * @brief Remove key from section.
     * @param section Section path
     * @param key Parameter name
     */
    void remove(std::string const& section, std::string const& key);

    /**
     * @brief Remove entire section.
     * @param section Section path
     */
    void remove_section(std::string const& section);

    /**
     * @brief Get all keys in root section.
     * @return Vector of key names
     */
    std::vector<std::string> keys() const;

    /**
     * @brief Get all keys in specific section.
     * @param section Section path
     * @return Vector of key names
     */
    std::vector<std::string> keys(std::string const& section) const;

    /**
     * @brief Get all section names.
     * @return Vector of section paths
     */
    std::vector<std::string> sections() const;

    /**
     * @brief Clear all data.
     */
    void clear();

    /**
     * @brief Merge another config into this one.
     * @param other Config to merge (overwrites existing keys)
     */
    void merge(Config const& other);

private:
    std::unordered_map<std::string, Config_value>                                  _root; ///< Root section values
    std::unordered_map<std::string, std::unordered_map<std::string, Config_value>> _sections; ///< Nested sections
};

// ============================================================================
// Template implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<ConfigValueType T>
void Config::set(
    std::string const& key,
    T           const& value
    ) {

    _root[key] = value;
}

// ----------------------------------------------------------------------------
template<ConfigValueType T>
void Config::set(
    std::string const& section,
    std::string const& key,
    T           const& value
    ) {

    _sections[section][key] = value;
}

// ----------------------------------------------------------------------------
template<ConfigValueType T>
std::optional<T> Config::get(
    std::string const& key
    ) const {

    auto const it = _root.find(key);

    if (it == _root.cend()) {

        return std::nullopt;
    }

    if (auto const* val = std::get_if<T>(&it->second)) {

        return *val;
    }

    return std::nullopt;
}

// ----------------------------------------------------------------------------
template<ConfigValueType T>
std::optional<T> Config::get(
    std::string const& section,
    std::string const& key
    ) const {

    auto const section_it = _sections.find(section);

    if (section_it == _sections.cend()) {

        return std::nullopt;
    }

    auto const key_it = section_it->second.find(key);

    if (key_it == section_it->second.cend()) {

        return std::nullopt;
    }

    if (auto const* val = std::get_if<T>(&key_it->second)) {

        return *val;
    }

    return std::nullopt;
}

// ----------------------------------------------------------------------------
template<ConfigValueType T>
T Config::get_or(
    std::string const& key,
    T           const& default_value
    ) const {

    return get<T>(key).value_or(default_value);
}

// ----------------------------------------------------------------------------
template<ConfigValueType T>
T Config::get_or(
    std::string const& section,
    std::string const& key,
    T           const& default_value
    ) const {

    return get<T>(section, key).value_or(default_value);
}

} // namespace titan::core
