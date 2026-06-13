// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/config/include/config_format.hpp"
#include "module-utils/include/logger.hpp"

#include "nlohmann/json.hpp"
#include "toml++/toml.h"
#include "msgpack.hpp"

#include <fstream>
#include <sstream>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Helper functions
// ----------------------------------------------------------------------------

namespace {

// ----------------------------------------------------------------------------
nlohmann::json config_to_json(
    Config const& config
    ) {

    nlohmann::json j;

    // Save root values
    auto root_keys = config.keys();
    if (!root_keys.empty()) {

        nlohmann::json root_obj;
        for (auto const& key : root_keys) {

            if (auto val_bool = config.get<bool>(key)) {

                root_obj[key] = *val_bool;
            } else if (auto val_int = config.get<int>(key)) {

                root_obj[key] = *val_int;
            } else if (auto val_long = config.get<long>(key)) {

                root_obj[key] = *val_long;
            } else if (auto val_float = config.get<float>(key)) {

                root_obj[key] = *val_float;
            } else if (auto val_double = config.get<double>(key)) {

                root_obj[key] = *val_double;
            } else if (auto val_str = config.get<std::string>(key)) {

                root_obj[key] = *val_str;
            }
        }

        j["root"] = root_obj;
    }

    // Save sections
    auto section_names = config.sections();
    if (!section_names.empty()) {

        nlohmann::json sections_obj;
        for (auto const& section : section_names) {

            nlohmann::json section_obj;

            auto section_keys = config.keys(section);
            for (auto const& key : section_keys) {

                if (auto val_bool = config.get<bool>(section, key)) {

                    section_obj[key] = *val_bool;
                } else if (auto val_int = config.get<int>(section, key)) {

                    section_obj[key] = *val_int;
                } else if (auto val_long = config.get<long>(section, key)) {

                    section_obj[key] = *val_long;
                } else if (auto val_float = config.get<float>(section, key)) {

                    section_obj[key] = *val_float;
                } else if (auto val_double = config.get<double>(section, key)) {

                    section_obj[key] = *val_double;
                } else if (auto val_str = config.get<std::string>(section, key)) {

                    section_obj[key] = *val_str;
                }
            }
            sections_obj[section] = section_obj;
        }

        j["sections"] = sections_obj;
    }

    return j;
}

// ----------------------------------------------------------------------------
Config json_to_config(
    nlohmann::json const& j
    ) {

    Config config;

    // Load root values
    if (j.contains("root") && j["root"].is_object()) {

        for (auto const& [key, value] : j["root"].items()) {

            if (value.is_boolean()) {

                config.set(key, value.get<bool>());
            } else if (value.is_number_integer()) {

                config.set(key, value.get<int>());
            } else if (value.is_number_float()) {

                config.set(key, value.get<double>());
            } else if (value.is_string()) {

                config.set(key, value.get<std::string>());
            }
        }
    }

    // Load sections
    if (j.contains("sections") && j["sections"].is_object()) {

        for (auto const& [section, values] : j["sections"].items()) {

            if (values.is_object()) {

                for (auto const& [key, value] : values.items()) {

                    if (value.is_boolean()) {

                        config.set(section, key, value.get<bool>());
                    } else if (value.is_number_integer()) {

                        config.set(section, key, value.get<int>());
                    } else if (value.is_number_float()) {

                        config.set(section, key, value.get<double>());
                    } else if (value.is_string()) {

                        config.set(section, key, value.get<std::string>());
                    }
                }
            }
        }
    }

    return config;
}

} // anonymous namespace

// ============================================================================
// JSON serialization
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Config load_config_json(
    std::filesystem::path const& path
    ) {

    std::ifstream file(path);

    if (!file.is_open()) {

        LOG(Log_lvl::ERR) << "Failed to open config file: " << path.string();
        return {};
    }

    nlohmann::json j;
    file >> j;

    return json_to_config(j);
}

// ----------------------------------------------------------------------------
void save_config_json(
    Config                const& config,
    std::filesystem::path const& path
    ) {

    nlohmann::json j = config_to_json(config);

    std::ofstream file(path);

    if (!file.is_open()) {

        LOG(Log_lvl::ERR) << "Failed to open config file for writing: " << path.string();
        return;
    }

    file << j.dump(4);
}

// ============================================================================
// TOML serialization
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Config load_config_toml(
    std::filesystem::path const& path
    ) {

    toml::table tbl;

    try {

        tbl = toml::parse_file(path.string());
    } catch (toml::parse_error const& err) {

        LOG(Log_lvl::ERR) << "Failed to parse TOML file: " << std::string(err.what());
        return {};
    }

    Config config;

    // Load all sections
    for (auto const& [section_key, section_value] : tbl) {

        std::string section_name = std::string(section_key.str());

        if (section_value.is_table()) {

            auto const* table = section_value.as_table();
            for (auto const& [key, value] : *table) {

                std::string key_name = std::string(key.str());

                if (value.is_boolean()) {

                    config.set(section_name, key_name, **value.as_boolean());
                } else if (value.is_integer()) {

                    config.set(section_name, key_name, static_cast<int>(**value.as_integer()));
                } else if (value.is_floating_point()) {

                    config.set(section_name, key_name, **value.as_floating_point());
                } else if (value.is_string()) {

                    config.set(section_name, key_name, std::string(**value.as_string()));
                }
            }
        }
    }

    return config;
}

// ----------------------------------------------------------------------------
void save_config_toml(
    Config                const& config,
    std::filesystem::path const& path
    ) {

    toml::table tbl;

    // Save sections
    auto section_names = config.sections();
    for (auto const& section : section_names) {

        toml::table section_tbl;
        auto section_keys = config.keys(section);

        for (auto const& key : section_keys) {

            if (auto val_bool = config.get<bool>(section, key)) {

                section_tbl.insert_or_assign(key, *val_bool);
            } else if (auto val_int = config.get<int>(section, key)) {

                section_tbl.insert_or_assign(key, *val_int);
            } else if (auto val_long = config.get<long>(section, key)) {

                section_tbl.insert_or_assign(key, *val_long);
            } else if (auto val_float = config.get<float>(section, key)) {

                section_tbl.insert_or_assign(key, static_cast<double>(*val_float));
            } else if (auto val_double = config.get<double>(section, key)) {

                section_tbl.insert_or_assign(key, *val_double);
            } else if (auto val_str = config.get<std::string>(section, key)) {

                section_tbl.insert_or_assign(key, *val_str);
            }
        }

        tbl.insert_or_assign(section, section_tbl);
    }

    std::ofstream file(path);
    if (!file.is_open()) {

        LOG(Log_lvl::ERR) << "Failed to open TOML file for writing: " << path.string();
        return;
    }

    file << tbl;
}

// ============================================================================
// Binary serialization (MessagePack)
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Config load_config_binary(
    std::filesystem::path const& path
    ) {

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {

        LOG(Log_lvl::ERR) << "Failed to open binary config file: " << path.string();
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string const data = buffer.str();

    msgpack::object_handle oh = msgpack::unpack(data.data(), data.size());
    msgpack::object const obj = oh.get();

    nlohmann::json j = nlohmann::json::parse(obj.as<std::string>());
    return json_to_config(j);
}

// ----------------------------------------------------------------------------
void save_config_binary(
    Config                const& config,
    std::filesystem::path const& path
    ) {

    nlohmann::json j = config_to_json(config);
    std::string const json_str = j.dump();

    msgpack::sbuffer buffer;
    msgpack::pack(buffer, json_str);

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {

        LOG(Log_lvl::ERR) << "Failed to open binary config file for writing: " << path.string();
        return;
    }

    file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
}

// ============================================================================
// Public interface
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Config load_config(
    std::filesystem::path const& path,
    Config_format         const  format
    ) {

    switch (format) {

        case Config_format::JSON:   return load_config_json  (path);
        case Config_format::TOML:   return load_config_toml  (path);
        case Config_format::BINARY: return load_config_binary(path);

        default: LOG(Log_lvl::ERR) << "Unknown config format"; return {};
    }
}

// ----------------------------------------------------------------------------
void save_config(
    Config                const& config,
    std::filesystem::path const& path,
    Config_format         const  format
    ) {

    switch (format) {

        case Config_format::JSON:   save_config_json  (config, path); break;
        case Config_format::TOML:   save_config_toml  (config, path); break;
        case Config_format::BINARY: save_config_binary(config, path); break;

        default: LOG(Log_lvl::ERR) << "Unknown config format";
    }
}

// ----------------------------------------------------------------------------
Config load_config_auto(
    std::filesystem::path const& path
    ) {

    auto const ext = path.extension().string();

    if (ext == ".json") {

        return load_config(path, Config_format::JSON);
    } else if (ext == ".toml") {

        return load_config(path, Config_format::TOML);
    } else if (ext == ".bin") {

        return load_config(path, Config_format::BINARY);
    } else {

        LOG(Log_lvl::ERR) << "Unknown config file extension: " << ext;
        return {};
    }
}

// ----------------------------------------------------------------------------
void save_config_auto(
    Config                const& config,
    std::filesystem::path const& path
    ) {

    auto const ext = path.extension().string();

    if (ext == ".json") {

        save_config(config, path, Config_format::JSON);
    } else if (ext == ".toml") {

        save_config(config, path, Config_format::TOML);
    } else if (ext == ".bin") {

        save_config(config, path, Config_format::BINARY);
    } else {

        LOG(Log_lvl::ERR) << "Unknown config file extension: " << ext;
    }
}

} // namespace titan::core
