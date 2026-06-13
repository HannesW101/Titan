#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-resources/include/spritesheet.hpp"
#include "module-utils/include/logger.hpp"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <stdexcept>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::resources {

// ============================================================================
// Class Resource_manager
// ----------------------------------------------------------------------------

/**
 * @brief Manages all SFML resources with reference counting and automatic unloading.
 *
 * Resources are identified by string keys and loaded from file paths.
 * Reference counting is handled via shared_ptr - when no outside handles remain
 * the resource is queued for unloading at end of frame via flush_unused().
 *
 * Usage:
 * @code
 * auto tex   = res.load<sf::Texture>("player", "assets/player.png");
 * auto sheet = res.load<Spritesheet>("player_sheet", "assets/player.png");
 * sheet->add_animation({...});
 * auto music = res.load<sf::Music>("battle", "assets/battle.ogg");
 * res.play_music("battle");
 * @endcode
 */
class Resource_manager final {

public:
    Resource_manager           (Resource_manager const&) = delete;
    Resource_manager& operator=(Resource_manager const&) = delete;
    Resource_manager           (Resource_manager&&     ) = delete;
    Resource_manager& operator=(Resource_manager&&     ) = delete;

    /**
     * @brief Get singleton instance.
     * @return Reference to Resource_manager
     */
    static Resource_manager& instance();

    /**
     * @brief Load a resource from file. Returns existing if already loaded.
     * @tparam T Resource type (sf::Texture, sf::Font, sf::Image, sf::SoundBuffer, sf::Shader, sf::Music, Spritesheet)
     * @param name String key to identify the resource
     * @param path Path to resource file
     * @return Shared pointer to resource
     */
    template<typename T>
    std::shared_ptr<T> load(
        std::string const& name,
        std::filesystem::path const& path
        );

    /**
     * @brief Get a previously loaded resource by name.
     * @tparam T Resource type
     * @param name String key
     * @return Shared pointer to resource
     * @throws std::runtime_error if not found
     */
    template<typename T>
    std::shared_ptr<T> get(std::string const& name);

    /**
     * @brief Manually unload a resource immediately.
     * @param name String key
     */
    void unload(std::string const& name);

    /**
     * @brief Unload all resources with no outside references.
     * Call once per frame at the END of your game loop.
     */
    void flush_unused();

    /**
     * @brief Unload all resources immediately.
     */
    void clear();

    /**
     * @brief Play a loaded music track.
     * @param name String key
     * @param loop Whether to loop
     */
    void play_music(std::string const& name, bool loop = true);

    /**
     * @brief Stop a loaded music track.
     * @param name String key
     */
    void stop_music(std::string const& name);

    /**
     * @brief Pause a loaded music track.
     * @param name String key
     */
    void pause_music(std::string const& name);

    /**
     * @brief Resume a paused music track.
     * @param name String key
     */
    void resume_music(std::string const& name);

    /**
     * @brief Set volume of a music track.
     * @param name String key
     * @param volume Volume 0.0 - 100.0
     */
    void set_music_volume(std::string const& name, float volume);

    /**
     * @brief Stop all currently playing music tracks.
     */
    void stop_all_music();

private:
    Resource_manager()  = default;

    ~Resource_manager() = default;

    // Type-erased shared_ptr storage for all non-music resources
    std::unordered_map<std::string, std::shared_ptr<void>> _resources;

    // Music stored separately - streams from disk, non-copyable
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> _music;
};

// ============================================================================
// Template implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
std::shared_ptr<T> Resource_manager::load(
    std::string const& name,
    std::filesystem::path const& path
    ) {

    using namespace titan::utils;

    // Return existing if already loaded
    auto it = _resources.find(name);
    if (it != _resources.end()) {

        return std::static_pointer_cast<T>(it->second);
    }

    auto resource = std::make_shared<T>();

    if (!resource->loadFromFile(path.string())) {

        LOG(Log_lvl::ERR) << "Failed to load resource: " + path.string();
        return {};
    }

    _resources[name] = resource;
    return resource;
}

// ----------------------------------------------------------------------------
// sf::Font is opened, not loaded, in SFML 3 (openFromFile), so it needs its own
// specialization rather than the generic loadFromFile path above.
template<>
inline std::shared_ptr<sf::Font> Resource_manager::load<sf::Font>(
    std::string const& name,
    std::filesystem::path const& path
    ) {

    using namespace titan::utils;

    auto it = _resources.find(name);
    if (it != _resources.end()) {

        return std::static_pointer_cast<sf::Font>(it->second);
    }

    auto resource = std::make_shared<sf::Font>();

    if (!resource->openFromFile(path.string())) {

        LOG(Log_lvl::ERR) << "Failed to open font: " + path.string();
        return {};
    }

    _resources[name] = resource;
    return resource;
}

// ----------------------------------------------------------------------------
template<>
inline std::shared_ptr<Spritesheet> Resource_manager::load<Spritesheet>(
    std::string const& name,
    std::filesystem::path const& path
    ) {

    using namespace titan::utils;

    auto it = _resources.find(name);
    if (it != _resources.end()) {

        return std::static_pointer_cast<Spritesheet>(it->second);
    }

    sf::Texture texture;
    if (!texture.loadFromFile(path.string())) {

        LOG(Log_lvl::ERR) << "Failed to load spritesheet texture: " + path.string();
        return {};
    }

    auto spritesheet = std::make_shared<Spritesheet>(std::move(texture));
    _resources[name] = spritesheet;
    return spritesheet;
}

// ----------------------------------------------------------------------------
template<>
inline std::shared_ptr<sf::Music> Resource_manager::load<sf::Music>(
    std::string const& name,
    std::filesystem::path const& path
    ) {

    using namespace titan::utils;

    auto it = _music.find(name);
    if (it != _music.end()) {

        return std::shared_ptr<sf::Music>(it->second.get(), [](sf::Music*){});
    }

    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(path.string())) {

        LOG(Log_lvl::ERR) << "Failed to open music: " + path.string();
        return {};
    }

    auto* raw_ptr = music.get();
    _music[name] = std::move(music);

    // Non-owning shared_ptr - lifetime managed by _music map
    return std::shared_ptr<sf::Music>(raw_ptr, [](sf::Music*){});
}

// ----------------------------------------------------------------------------
template<typename T>
std::shared_ptr<T> Resource_manager::get(std::string const& name) {

    using namespace titan::utils;

    auto it = _resources.find(name);
    if (it != _resources.end()) {

        return std::static_pointer_cast<T>(it->second);
    }

    LOG(Log_lvl::ERR) << "Resource not found: " + name;
    return {};
}

// ----------------------------------------------------------------------------
template<>
inline std::shared_ptr<sf::Music> Resource_manager::get<sf::Music>(
    std::string const& name
    ) {

    using namespace titan::utils;

    auto it = _music.find(name);
    if (it == _music.end()) {

        LOG(Log_lvl::ERR) << "Music not found: " + name;
    }

    return std::shared_ptr<sf::Music>(it->second.get(), [](sf::Music*){});
}

} // namespace titan::resources
