#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"
#include "module-game/components/include/transform.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// ============================================================================
// Forward declarations (1/2)
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Forward declarations (2/2)
// ----------------------------------------------------------------------------

class World;

// ============================================================================
// Class Game_object
// ----------------------------------------------------------------------------

/**
 * @brief An entity in the game World: a name, a unique id, a built-in Transform,
 * and a set of components that give it behavior and appearance.
 *
 * Composition over inheritance: instead of subclassing per entity type, you
 * assemble objects from components:
 *
 *   auto* enemy = world.spawn("enemy");
 *   enemy->transform().set_position(100.f, 50.f);
 *   enemy->add_component<Sprite_renderer>()->set_texture("orc");
 *   enemy->add_component<Health>(30);
 *   enemy->add_component<Script>()->set_update_fn(...);
 *
 * Typed access: add_component<T>(args...) constructs and returns T*;
 * get_component<T>() returns the first T* or nullptr. has_component<T>() tests.
 *
 * Lifecycle is driven by World, which forwards fixed_update/update/render to all
 * enabled components and fires on_start() the first active frame.
 *
 * Destruction is deferred: destroy() marks the object, World removes it at a
 * safe point so a component can destroy its own object mid-update.
 */
class Game_object final {

public:
    /**
     * @brief Construct a game object with a name and unique id.
     * @param name Display/lookup name
     * @param id Unique identifier assigned by the World
     */
    explicit Game_object(std::string name, std::uint64_t const id);

    /**
     * @brief Destroy the object, detaching all components in reverse order.
     */
    ~Game_object();

    Game_object           (Game_object const&) = delete;
    Game_object& operator=(Game_object const&) = delete;
    Game_object           (Game_object&&     ) = delete;
    Game_object& operator=(Game_object&&     ) = delete;

    // ---- Identity ----------------------------------------------------------

    /**
     * @brief Get the object's name.
     * @return Reference to the name string
     */
    std::string const& name() const;

    /**
     * @brief Set the object's name.
     * @param name New name
     */
    void set_name(std::string name);

    /**
     * @brief Get the object's unique id.
     * @return Unique identifier
     */
    std::uint64_t id() const;

    // ---- Active / destroy --------------------------------------------------

    /**
     * @brief Check whether the object is active.
     * @return True if active (receives updates/renders)
     */
    bool is_active() const;

    /**
     * @brief Activate or deactivate the object.
     * @param active True to activate, false to skip its updates/renders
     */
    void set_active(bool const active);

    /**
     * @brief Mark the object for destruction (deferred; World reaps it).
     */
    void destroy();

    /**
     * @brief Check whether the object is marked for destruction.
     * @return True if destroy() was called
     */
    bool is_destroyed() const;

    // ---- Transform (built in) ----------------------------------------------

    /**
     * @brief Get the built-in Transform component.
     * @return Reference to the object's Transform
     */
    Transform& transform();

    /**
     * @brief Get the built-in Transform component (const).
     * @return Const reference to the object's Transform
     */
    Transform const& transform() const;

    // ---- Components --------------------------------------------------------

    /**
     * @brief Construct and attach a component of type T.
     * @tparam T Component type (must derive from Game_component)
     * @tparam Args Constructor argument types for T
     * @param args Arguments forwarded to T's constructor
     * @return Raw pointer to the new component (owned by this object)
     */
    template<typename T, typename... Args>
    T* add_component(Args&&... args) {

        static_assert(std::is_base_of_v<Game_component, T>, "T must derive from Game_component");
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = comp.get();
        comp->_owner = this;
        _components.push_back(std::move(comp));
        raw->on_attach();
        return raw;
    }

    /**
     * @brief Get the first component of type T.
     * @tparam T Component type to search for
     * @return Pointer to the component, or nullptr if not present
     */
    template<typename T>
    T* get_component() {

        for (auto& c : _components) {

            if (auto* p = dynamic_cast<T*>(c.get())) { return p; }
        }

        return nullptr;
    }

    /**
     * @brief Check whether a component of type T is attached.
     * @tparam T Component type to test for
     * @return True if at least one T is attached
     */
    template<typename T>
    bool has_component() { return get_component<T>() != nullptr; }

    /**
     * @brief Remove the first component of type T, if present.
     * @tparam T Component type to remove
     */
    template<typename T>
    void remove_component() {

        for (auto it = _components.begin(); it != _components.end(); ++it) {

            if (dynamic_cast<T*>(it->get())) {

                (*it)->on_detach();
                _components.erase(it);
                return;
            }
        }
    }

    // ---- World context -----------------------------------------------------

    /**
     * @brief Get the owning World.
     * @return Reference to the World
     * @throws std::runtime_error if the object has no World
     */
    World& world();

    /**
     * @brief Get the owning World (const).
     * @return Const reference to the World
     * @throws std::runtime_error if the object has no World
     */
    World const& world() const;

    /**
     * @brief Check whether the object belongs to a World.
     * @return True if a World is set
     */
    bool has_world() const;

    // ---- Per-frame (driven by World) ---------------------------------------

    /**
     * @brief Fixed-timestep update; forwards to all enabled components.
     * @param fixed_dt Fixed delta time in seconds
     */
    void fixed_update(float const fixed_dt);

    /**
     * @brief Per-frame update; forwards to all enabled components.
     * @param dt Frame delta time in seconds
     */
    void update(float const dt);

    /**
     * @brief Per-frame render; forwards to all enabled components.
     * @param renderer Renderer to submit drawables to
     */
    void render(render::Renderer& renderer);

private:
    friend class World;

    std::string   _name; ///< Display/lookup name
    std::uint64_t _id; ///< Unique identifier
    bool          _active    = true; ///< Whether updates/renders run
    bool          _destroyed = false; ///< Marked for deferred destruction
    bool          _started   = false; ///< Whether on_start has fired

    Transform* _transform = nullptr; ///< Convenience pointer into _components

    std::vector<std::unique_ptr<Game_component>> _components;  ///< Owned components

    World* _world = nullptr; ///< Owning world (set by World)

    /**
     * @brief Fire on_start on components the first active frame.
     */
    void _start_if_needed();
};

} // namespace titan::game
