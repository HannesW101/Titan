#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/config/include/config.hpp"
#include "module-core/config/include/config_format.hpp"

#include <string>
#include <unordered_map>
#include <filesystem>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Config_manager
// ----------------------------------------------------------------------------

/**
 * @brief Singleton manager for named configuration objects.
 *
 * Manages multiple configs (e.g., "settings", "keybinds", "save_data").
 * Handles loading/saving with multiple format support.
 */
class Config_manager final {

public:
    Config_manager           (Config_manager const&) = delete;
    Config_manager& operator=(Config_manager const&) = delete;
    Config_manager           (Config_manager&&     ) = delete;
    Config_manager& operator=(Config_manager&&     ) = delete;

    /**
     * @brief Get singleton instance.
     * @return Reference to Config_manager instance
     */
    static Config_manager& instance();

    /**
     * @brief Create or get a named config.
     * @param name Config identifier
     * @return Reference to config
     */
    Config& get(std::string const& name);

    /**
     * @brief Check if named config exists.
     * @param name Config identifier
     * @return True if config exists
     */
    bool has(std::string const& name) const;

    /**
     * @brief Remove a named config.
     * @param name Config identifier
     */
    void remove(std::string const& name);

    /**
     * @brief Load config from file.
     * @param name Config identifier to store under
     * @param path File path
     * @param format File format
     */
    void load(
        std::string           const& name,
        std::filesystem::path const& path,
        Config_format         const  format
        );

    /**
     * @brief Load config with auto-detected format.
     * @param name Config identifier to store under
     * @param path File path
     */
    void load_auto(
        std::string           const& name,
        std::filesystem::path const& path
        );

    /**
     * @brief Save config to file.
     * @param name Config identifier
     * @param path File path
     * @param format File format
     */
    void save(
        std::string           const& name,
        std::filesystem::path const& path,
        Config_format         const  format
        );

    /**
     * @brief Save config with auto-detected format.
     * @param name Config identifier
     * @param path File path
     */
    void save_auto(
        std::string           const& name,
        std::filesystem::path const& path
        );

    /**
     * @brief Clear all configs.
     */
    void clear();

private:
    Config_manager();

    ~Config_manager() = default;

    std::unordered_map<std::string, Config> _configs; ///< Named config storage
};

} // namespace titan::core
