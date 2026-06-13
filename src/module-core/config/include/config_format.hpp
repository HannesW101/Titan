#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include "module-core/config/include/config.hpp"

#include <string>
#include <filesystem>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Enums
// ----------------------------------------------------------------------------

/**
 * @brief Supported configuration file formats.
 */
enum class Config_format : std::uint8_t {

    JSON, ///< JSON format (human-readable, structured)
    TOML, ///< TOML format (human-readable, modern INI)
    BINARY, ///< Binary format (compact, fast, non-human-readable)
};

// ============================================================================
// Functions
// ----------------------------------------------------------------------------

/**
 * @brief Load configuration from file.
 * @param path File path
 * @param format File format
 * @return Loaded config
 */
Config load_config(
    std::filesystem::path const& path,
    Config_format         const  format
    );

/**
 * @brief Save configuration to file.
 * @param config Config to save
 * @param path File path
 * @param format File format
 */
void save_config(
    Config                const& config,
    std::filesystem::path const& path,
    Config_format         const  format
    );

/**
 * @brief Load configuration with format auto-detected from extension.
 * @param path File path (.json, .ini, .bin)
 * @return Loaded config
 */
Config load_config_auto(
    std::filesystem::path const& path
    );

/**
 * @brief Save configuration with format auto-detected from extension.
 * @param config Config to save
 * @param path File path (.json, .ini, .bin)
 */
void save_config_auto(
    Config                const& config,
    std::filesystem::path const& path
    );

} // namespace titan::core
