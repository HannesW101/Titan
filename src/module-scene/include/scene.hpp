#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <string>

// ============================================================================
// Forward declarations (1/2)
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }
namespace titan::app    { class Application; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::scene {

// ============================================================================
// Forward declarations (2/2)
// ----------------------------------------------------------------------------

class Scene_manager;

// ============================================================================
// Class Scene
// ----------------------------------------------------------------------------

/**
 * @brief Base class for a game screen: main menu, gameplay level, pause overlay,
 * settings, etc. A Scene owns whatever it needs (UI, game world, scene-tier
 * resources) and exposes lifecycle hooks the Scene_manager drives.
 *
 * Lifecycle:
 *   on_enter()        once, when the scene is first pushed/activated
 *   on_resume()       when it becomes the top scene again (e.g. overlay closed)
 *   on_pause()        when another scene is pushed on top of it
 *   fixed_update(dt)  zero+ times per frame, fixed timestep (gameplay/physics)
 *   update(dt)        once per frame (animations, UI, input-driven logic)
 *   render(renderer)  once per frame
 *   on_exit()         once, right before the scene is destroyed/popped
 *
 * Transparency flags let a pushed scene (e.g. a pause menu) request that the
 * scene beneath it keep rendering and/or updating, enabling overlays.
 *
 * Scenes get access to the owning Application (window, renderer, resources,
 * audio) and the Scene_manager (to request transitions) once attached.
 */
class Scene {

public:
    /**
     * @brief Construct a scene with a name.
     * @param name Scene name (for lookup/logging)
     */
    explicit Scene(std::string name);

    /**
     * @brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~Scene() = default;

    Scene           (Scene const&) = delete;
    Scene& operator=(Scene const&) = delete;
    Scene           (Scene&&     ) = delete;
    Scene& operator=(Scene&&     ) = delete;

    // ---- Lifecycle hooks (override as needed) ------------------------------

    /**
     * @brief Called once when the scene is first pushed/activated.
     */
    virtual void on_enter() {}

    /**
     * @brief Called once right before the scene is popped/destroyed.
     */
    virtual void on_exit() {}

    /**
     * @brief Called when another scene is pushed on top of this one.
     */
    virtual void on_pause() {}

    /**
     * @brief Called when this scene becomes the top scene again.
     */
    virtual void on_resume() {}

    /**
     * @brief Fixed-timestep update for gameplay/physics.
     * @param fixed_dt Fixed delta time in seconds
     */
    virtual void fixed_update(float fixed_dt) { static_cast<void>(fixed_dt); }

    /**
     * @brief Per-frame update for animation, UI, and input logic.
     * @param dt Frame delta time in seconds
     */
    virtual void update(float dt) { static_cast<void>(dt); }

    /**
     * @brief Per-frame render.
     * @param renderer Renderer to submit drawables to
     */
    virtual void render(render::Renderer& renderer) { static_cast<void>(renderer); }

    // ---- Overlay behavior --------------------------------------------------

    /**
     * @brief If true, the scene directly below this one still renders.
     * Use for pause menus / popups drawn over the frozen game.
     */
    /**
     * @brief Whether the scene directly below this one still renders.
     * @return True if the scene below renders (for overlays)
     */
    bool renders_below() const;

    /**
     * @brief Set whether the scene directly below this one still renders.
     * @param value True to render the scene below (for overlays)
     */
    void set_renders_below(bool value);

    /**
     * @brief If true, the scene directly below this one still updates.
     * Usually false for a pause menu (game should freeze).
     */
    /**
     * @brief Whether the scene directly below this one still updates.
     * @return True if the scene below updates
     */
    bool updates_below() const;

    /**
     * @brief Set whether the scene directly below this one still updates.
     * @param value True to update the scene below
     */
    void set_updates_below(bool value);

    // ---- Identity / context ------------------------------------------------

    /**
     * @brief Get the scene name.
     * @return Reference to the name string
     */
    std::string const& name() const;

    /**
     * @brief Get the owning Application.
     * @return Reference to the Application
     * @throws std::runtime_error if no context is attached yet
     */
    app::Application& application();

    /**
     * @brief Get the owning Scene_manager.
     * @return Reference to the Scene_manager
     * @throws std::runtime_error if no context is attached yet
     */
    Scene_manager& scenes();

protected:
    /**
     * @brief Check whether Application/Scene_manager context is attached.
     * @return True if context is available
     */
    bool _has_context() const { return _app != nullptr; }

private:
    friend class Scene_manager;

    std::string _name;
    bool        _renders_below = false;
    bool        _updates_below = false;

    app::Application* _app     = nullptr;
    Scene_manager*    _manager = nullptr;
};

} // namespace titan::scene
