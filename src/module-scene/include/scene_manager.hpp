#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-scene/include/scene.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }
namespace titan::app    { class Application; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::scene {

// ============================================================================
// Class Scene_manager
// ----------------------------------------------------------------------------

/**
 * @brief Owns a stack of Scenes and drives their lifecycle.
 *
 * The top scene is "active". Scenes below it are paused; an overlay scene can
 * opt to let those below keep rendering/updating (see Scene::set_renders_below).
 *
 * Transition operations (push / pop / replace / clear) are **deferred**: they
 * are recorded when requested and applied at a safe point (start of the next
 * update), so a scene can safely request "replace me" from inside its own
 * update() without being destroyed mid-call.
 *
 * On any scene destruction the manager calls Resource_manager::flush_unused(),
 * so resources a scene was the sole owner of are released on transition
 * (scene-tier resource lifetime via reference counting).
 *
 * Wire it into the Application once:
 *   scene_mgr.attach(app);
 *   app.set_fixed_update([&](float dt){ scene_mgr.fixed_update(dt); });
 *   app.set_update      ([&](float dt){ scene_mgr.update(dt); });
 *   app.set_render      ([&](auto& r ){ scene_mgr.render(r); });
 */
class Scene_manager final {

public:
    /**
     * @brief Construct an empty scene manager.
     */
    Scene_manager() = default;

    Scene_manager           (Scene_manager const&) = delete;
    Scene_manager& operator=(Scene_manager const&) = delete;
    Scene_manager           (Scene_manager&&     ) = delete;
    Scene_manager& operator=(Scene_manager&&     ) = delete;

    /**
     * @brief Provide the owning Application so scenes can reach subsystems.
     */
    /**
     * @brief Provide the owning Application so scenes can reach subsystems.
     * @param application Application reference
     */
    void attach(app::Application& application);

    // ---- Transition requests (deferred) ------------------------------------

    /**
     * @brief Push a scene on top of the stack. Current top pauses.
     */
    /**
     * @brief Push a scene on top of the stack (deferred). Current top pauses.
     * @param scene Scene to push
     */
    void push(std::unique_ptr<Scene> scene);

    /**
     * @brief Pop the top scene. The one below resumes.
     */
    /**
     * @brief Pop the top scene (deferred). The one below resumes.
     */
    void pop();

    /**
     * @brief Replace the entire stack with a single scene (common for
     * main-menu -> gameplay transitions). All current scenes exit.
     */
    /**
     * @brief Replace the entire stack with a single scene (deferred).
     * @param scene Scene to become the only one on the stack
     */
    void replace(std::unique_ptr<Scene> scene);

    /**
     * @brief Pop every scene on the stack.
     */
    /**
     * @brief Pop every scene on the stack (deferred).
     */
    void clear();

    // ---- Per-frame (driven by Application) ----------------------------------

    /**
     * @brief Fixed-timestep update; walks scenes that allow updates below.
     * @param fixed_dt Fixed delta time in seconds
     */
    void fixed_update(float fixed_dt);

    /**
     * @brief Apply deferred transitions, then update the active scene(s).
     * @param dt Frame delta time in seconds
     */
    void update(float dt);

    /**
     * @brief Render the visible scene(s) bottom-up so overlays land on top.
     * @param renderer Renderer to submit drawables to
     */
    void render(render::Renderer& renderer);

    // ---- Queries ------------------------------------------------------------

    /**
     * @brief Get the top (active) scene.
     * @return Pointer to the top scene, or nullptr if empty
     */
    Scene* current();

    /**
     * @brief Get the number of scenes on the stack.
     * @return Stack depth
     */
    std::size_t depth() const;

    /**
     * @brief Check whether the stack is empty.
     * @return True if no scenes
     */
    bool empty() const;

private:
    enum class Op : std::uint8_t { PUSH, POP, REPLACE, CLEAR };

    /**
     * @brief A queued transition operation.
     */
    struct Pending {

        Op                     op; ///< Operation kind
        std::unique_ptr<Scene> scene; ///< Scene for PUSH / REPLACE
    };

    /**
     * @brief Apply all queued transition operations.
     */
    void _apply_pending();

    /**
     * @brief Inject Application/Scene_manager context into a scene.
     * @param scene Scene to attach context to
     */
    void _attach_scene(Scene& scene);

    /**
     * @brief Release resources no longer referenced after a teardown.
     */
    void _flush_resources();

    app::Application*                   _app = nullptr;
    std::vector<std::unique_ptr<Scene>> _stack;
    std::vector<Pending>                _pending;
};

} // namespace titan::scene
