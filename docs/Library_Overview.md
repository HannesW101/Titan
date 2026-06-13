# Titan Engine — Library Overview

The complete API reference for Titan, generated from the engine headers. Every class and enum is listed with its description and full method signatures (parameters and return values included).

## Contents

- [Core Application](#core-application)
- [Scenes](#scenes)
- [Game Objects & Components](#game-objects-components)
- [User Interface](#user-interface)
- [Rendering](#rendering)
- [Resources](#resources)
- [Audio](#audio)
- [Core Services](#core-services)
- [Utilities](#utilities)
- [Debug](#debug)


## Core Application

### `class Application : public events::Event_listener`

The engine's runtime shell.

Owns the window and every core subsystem (renderer, resources, audio, the frame timer) and runs the main loop:

```cpp
while running:
  poll events                       (SFML_event_manager)
  accumulate dt
  while accumulator >= fixed_dt:     fixed_update(fixed_dt)   // gameplay/physics
  update(dt)                         // per-frame logic, animations, UI
  render(renderer)                   // submit drawables
```

Game code does not subclass Application. Instead it installs callbacks (set_update / set_fixed_update / set_render) or, more commonly, hands control to a Scene_manager (next module) whose hooks Application calls. This keeps the shell decoupled from any particular game structure.

Lifetime: construct with an Application_config, optionally wire callbacks, then call run(). run() returns when the window closes or quit() is called.

**Methods**

```cpp
int run();
```
Run the main loop until the window closes or quit() is called.
- *returns* — Process exit code (0 = clean).

```cpp
void quit();
```
Request the loop to stop after the current frame.

```cpp
bool is_running() const;
```
Whether the loop is currently running.

```cpp
void set_update(Update_fn fn);
```
Set the per-frame update callback.
- `fn` — Callback receiving the frame delta

```cpp
void set_fixed_update(Fixed_update_fn fn);
```
Set the fixed-step update callback.
- `fn` — Callback receiving the fixed delta

```cpp
void set_render(Render_fn fn);
```
Set the render callback.
- `fn` — Callback receiving the renderer

```cpp
void set_on_start(Event_hook_fn fn);
```
Called once after subsystems are up but before the first frame. Good place to load the initial scene / build initial UI.

```cpp
void set_on_stop(Event_hook_fn fn);
```
Called once after the loop ends, before subsystems tear down.

```cpp
sf::RenderWindow& window();
```
Get the render window.
- *returns* — Reference to the window

```cpp
render::Renderer& renderer();
```
Get the renderer.
- *returns* — Reference to the renderer

```cpp
resources::Resource_manager& resources();
```
Get the resource manager.
- *returns* — Reference to the resource manager

```cpp
audio::Audio_system& audio();
```
Get the audio system.
- *returns* — Reference to the audio system

```cpp
core::Timer& frame_timer();
```
Get the frame timer.
- *returns* — Reference to the frame timer

```cpp
Application_config const& config() const;
```
Get the active configuration.
- *returns* — Const reference to the config

```cpp
void apply_config(Application_config const& config);
```
Rebuild the window from a (possibly changed) config. Use after the user changes resolution / fullscreen in a settings menu.


### `struct Application_config`

Settings that define how the Application creates its window and runs its main loop. Construct one, tweak fields, and pass to Application.

Can be loaded from / saved to a titan::core::Config so the window size, fullscreen flag, vsync, and frame caps live in the user's settings file.

**Methods**

```cpp
void load_from_config(core::Config const& config);
```
Populate this config from a Config section.
- `config` — Config to read from

```cpp
void save_to_config(core::Config& config) const;
```
Write current settings into a Config under [application].
- `config` — Config to write into



## Scenes

### `class Scene`

Base class for a game screen: main menu, gameplay level, pause overlay, settings, etc. A Scene owns whatever it needs (UI, game world, scene-tier resources) and exposes lifecycle hooks the Scene_manager drives.

Lifecycle:

```cpp
on_enter()        once, when the scene is first pushed/activated
on_resume()       when it becomes the top scene again (e.g. overlay closed)
on_pause()        when another scene is pushed on top of it
```

fixed_update(dt) zero+ times per frame, fixed timestep (gameplay/physics) update(dt) once per frame (animations, UI, input-driven logic) render(renderer) once per frame

```cpp
on_exit()         once, right before the scene is destroyed/popped
```

Transparency flags let a pushed scene (e.g. a pause menu) request that the scene beneath it keep rendering and/or updating, enabling overlays.

Scenes get access to the owning Application (window, renderer, resources, audio) and the Scene_manager (to request transitions) once attached.

**Methods**

```cpp
explicit Scene(std::string name);
```
Construct a scene with a name.
- `name` — Scene name (for lookup/logging)

```cpp
virtual void on_enter();
```
Called once when the scene is first pushed/activated.

```cpp
virtual void on_exit();
```
Called once right before the scene is popped/destroyed.

```cpp
virtual void on_pause();
```
Called when another scene is pushed on top of this one.

```cpp
virtual void on_resume();
```
Called when this scene becomes the top scene again.

```cpp
virtual void fixed_update(float fixed_dt);
```
Fixed-timestep update for gameplay/physics.
- `fixed_dt` — Fixed delta time in seconds

```cpp
virtual void update(float dt);
```
Per-frame update for animation, UI, and input logic.
- `dt` — Frame delta time in seconds

```cpp
virtual void render(render::Renderer& renderer);
```
Per-frame render.
- `renderer` — Renderer to submit drawables to

```cpp
bool renders_below() const;
```
Whether the scene directly below this one still renders.
- *returns* — True if the scene below renders (for overlays)

```cpp
void set_renders_below(bool value);
```
Set whether the scene directly below this one still renders.
- `value` — True to render the scene below (for overlays)

```cpp
bool updates_below() const;
```
Whether the scene directly below this one still updates.
- *returns* — True if the scene below updates

```cpp
void set_updates_below(bool value);
```
Set whether the scene directly below this one still updates.
- `value` — True to update the scene below

```cpp
std::string const& name() const;
```
Get the scene name.
- *returns* — Reference to the name string

```cpp
app::Application& application();
```
Get the owning Application.
- *returns* — Reference to the Application

```cpp
Scene_manager& scenes();
```
Get the owning Scene_manager.
- *returns* — Reference to the Scene_manager

```cpp
bool _has_context() const;
```
Check whether Application/Scene_manager context is attached. _(protected)_
- *returns* — True if context is available


### `class Scene_manager`

Owns a stack of Scenes and drives their lifecycle.

The top scene is "active". Scenes below it are paused; an overlay scene can opt to let those below keep rendering/updating (see Scene::set_renders_below).

Transition operations (push / pop / replace / clear) are **deferred**: they are recorded when requested and applied at a safe point (start of the next update), so a scene can safely request "replace me" from inside its own update() without being destroyed mid-call.

On any scene destruction the manager calls Resource_manager::flush_unused(), so resources a scene was the sole owner of are released on transition (scene-tier resource lifetime via reference counting).

Wire it into the Application once:

```cpp
scene_mgr.attach(app);
app.set_fixed_update([&](float dt){ scene_mgr.fixed_update(dt); });
app.set_update      ([&](float dt){ scene_mgr.update(dt); });
app.set_render      ([&](auto& r ){ scene_mgr.render(r); });
```

**Methods**

```cpp
void attach(app::Application& application);
```
Provide the owning Application so scenes can reach subsystems.
- `application` — Application reference

```cpp
void push(std::unique_ptr<Scene> scene);
```
Push a scene on top of the stack (deferred). Current top pauses.
- `scene` — Scene to push

```cpp
void pop();
```
Pop the top scene (deferred). The one below resumes.

```cpp
void replace(std::unique_ptr<Scene> scene);
```
Replace the entire stack with a single scene (deferred).
- `scene` — Scene to become the only one on the stack

```cpp
void clear();
```
Pop every scene on the stack (deferred).

```cpp
void fixed_update(float fixed_dt);
```
Fixed-timestep update; walks scenes that allow updates below.
- `fixed_dt` — Fixed delta time in seconds

```cpp
void update(float dt);
```
Apply deferred transitions, then update the active scene(s).
- `dt` — Frame delta time in seconds

```cpp
void render(render::Renderer& renderer);
```
Render the visible scene(s) bottom-up so overlays land on top.
- `renderer` — Renderer to submit drawables to

```cpp
Scene* current();
```
Get the top (active) scene.
- *returns* — Pointer to the top scene, or nullptr if empty

```cpp
std::size_t depth() const;
```
Get the number of scenes on the stack.
- *returns* — Stack depth

```cpp
bool empty() const;
```
Check whether the stack is empty.
- *returns* — True if no scenes


### `class Settings_scene : public Scene`

A ready-made settings overlay: volume sliders bound to the audio buses (Master / Music / SFX / UI), live-updating the mix and persisting it to a named Config on close.

Demonstrates the full UI integration path: Scene + UI_system + Audio_system + Config working together. Push it over gameplay as an overlay (it renders the scene below but freezes its updates):

```cpp
scenes().push(std::make_unique<Settings_scene>("settings"));
```

On "Back", it saves the audio mix to the config (via Audio_system::save_mix) and pops itself.

**Methods**

```cpp
explicit Settings_scene(std::string name = "settings");
```
Construct the settings overlay scene.
- `name` — Scene name (defaults to "settings")

```cpp
void set_config_name(std::string config_name);
```
Set the Config name to persist the audio mix into.
- `config_name` — Config identifier (defaults to "settings")

```cpp
void on_enter() override;
```
Build the UI when the scene is entered.

```cpp
void update(float dt) override;
```
Update the UI system.
- `dt` — Frame delta time in seconds

```cpp
void render(render::Renderer& renderer) override;
```
Render the UI.
- `renderer` — Renderer (UI draws via the shared renderer)



## Game Objects & Components

### enum `Collider_shape`

The geometric shape a Collider tests with.

| Value | Meaning |
| --- | --- |
| `AABB` | Axis-aligned box, sized by half_extents |
| `CIRCLE` | Circle, sized by radius |

### `class Animator : public Game_component`

Plays named animations from a Spritesheet and feeds the current frame rect into the owner's Sprite_renderer each frame.

Pairs with a Sprite_renderer on the same object: the Animator owns timing and frame selection, the Sprite_renderer owns drawing. The Spritesheet resource is loaded by name from the Resource_manager.

```cpp
auto* sr  = obj->add_component<Sprite_renderer>();
sr->set_texture("hero_sheet");
auto* anim = obj->add_component<Animator>();
anim->set_spritesheet("hero_sheet");
anim->add_animation({ "run", { rect0, rect1, rect2 }, 12.f });
anim->play("run");
```

**Methods**

```cpp
void set_spritesheet(std::string const& resource_name);
```
Set the spritesheet by resource name.
- `resource_name` — Spritesheet key in the Resource_manager

```cpp
void add_animation(resources::Animation const& animation);
```
Add an animation clip to the spritesheet.
- `animation` — Animation definition (name, frames, fps)

```cpp
void play(std::string const& name);
```
Play a named animation (deferred if sheet not yet loaded).
- `name` — Animation name

```cpp
void pause();
```
Pause playback.

```cpp
void resume();
```
Resume playback.

```cpp
void reset();
```
Reset the current animation to its first frame.

```cpp
bool is_finished() const;
```
Check whether a non-looping animation has finished.
- *returns* — True if finished

```cpp
std::string const& current_animation() const;
```
Get the current animation name.
- *returns* — Reference to the current animation name

```cpp
void on_start() override;
```
Resolve the spritesheet and cache the Sprite_renderer.

```cpp
void update(float dt) override;
```
Advance the animation and push the frame to the Sprite_renderer.
- `dt` — Frame delta time in seconds


### `class Audio_emitter : public Game_component`

Plays sound effects positioned at the owner's world location.

Components don't own the Audio_system (the Application does), so the scene wires a reference in via set_audio_system(). play() then emits the named SFX with spatial attenuation centered on the owner's transform, so distant emitters are quieter.

```cpp
auto* em = obj->add_component<Audio_emitter>();
em->set_audio_system(app.audio());
em->set_falloff_radius(600.f);
em->play("footstep");
```

**Methods**

```cpp
void set_audio_system(audio::Audio_system& system);
```
Bind the audio system used to play sounds.
- `system` — Audio system reference

```cpp
void set_bus(audio::Audio_bus bus);
```
Set the audio bus to route sounds through.
- `bus` — Target audio bus

```cpp
audio::Audio_bus bus() const;
```
Get the audio bus.
- *returns* — Current bus

```cpp
void set_falloff_radius(float radius);
```
Set the spatial falloff radius (distance to silence).
- `radius` — Falloff radius in pixels

```cpp
float falloff_radius() const;
```
Get the spatial falloff radius.
- *returns* — Falloff radius in pixels

```cpp
void set_spatial(bool spatial);
```
Enable or disable spatial positioning.
- `spatial` — True for positioned audio, false for non-spatial

```cpp
bool is_spatial() const;
```
Check whether spatial positioning is enabled.
- *returns* — True if spatial

```cpp
int play(std::string const& resource_name);
```
Play a one-shot SFX by resource name.
- `resource_name` — Sound buffer key in the Resource_manager
- *returns* — Play id, or -1 if not played

```cpp
int play(std::string const& resource_name, float volume, float pitch);
```
Play a one-shot SFX with volume/pitch overrides.
- `resource_name` — Sound buffer key in the Resource_manager
- `volume` — Volume scalar [0, 1] before bus scaling
- `pitch` — Pitch multiplier (1.0 = normal)
- *returns* — Play id, or -1 if not played


### `class Collider : public Game_component`

A collision shape attached to a Game_object, positioned at the owner's world position plus a local offset.

Two shapes are supported: AABB (set via half-extents) and circle (radius). The Collision_system tests every enabled collider pair each fixed step and fires on_enter / on_stay / on_exit callbacks. A collider may be a trigger (overlap reported but no implied physical response).

```cpp
auto* c = obj->add_component<Collider>();
c->set_box({ 16.f, 16.f });          // 32x32 box
c->set_on_enter([](Collider& self, Collider& other){ ... });
```

**Methods**

```cpp
void set_box(sf::Vector2f half_extents);
```
Configure as an axis-aligned box.
- `half_extents` — Half width/height (full box is twice this)

```cpp
void set_circle(float radius);
```
Configure as a circle.
- `radius` — Circle radius

```cpp
Collider_shape shape() const;
```
Get the collider shape.
- *returns* — Current shape enum

```cpp
sf::Vector2f half_extents() const;
```
Get the box half-extents.
- *returns* — Half width/height

```cpp
float radius() const;
```
Get the circle radius.
- *returns* — Radius

```cpp
void set_offset(sf::Vector2f offset);
```
Set the local offset from the owner's position.
- `offset` — Offset in pixels

```cpp
sf::Vector2f offset() const;
```
Get the local offset.
- *returns* — Offset in pixels

```cpp
void set_trigger(bool trigger);
```
Mark the collider as a trigger (overlap-only, no response).
- `trigger` — True for a trigger

```cpp
bool is_trigger() const;
```
Check whether the collider is a trigger.
- *returns* — True if a trigger

```cpp
sf::Vector2f world_center() const;
```
Get the world-space center (owner position plus offset).
- *returns* — World center in pixels

```cpp
sf::FloatRect world_aabb() const;
```
Get the world-space bounding box (also for circles).
- *returns* — Bounding rectangle

```cpp
bool overlaps(Collider const& other) const;
```
Test overlap against another collider.
- `other` — Other collider
- *returns* — True if the shapes overlap

```cpp
void set_on_enter(Collision_fn fn);
```
Set the callback fired when a new overlap begins.
- `fn` — Callback receiving self and other

```cpp
void set_on_stay(Collision_fn fn);
```
Set the callback fired while an overlap continues.
- `fn` — Callback receiving self and other

```cpp
void set_on_exit(Collision_fn fn);
```
Set the callback fired when an overlap ends.
- `fn` — Callback receiving self and other

```cpp
void fire_enter(Collider& other);
```
Fire the enter callback (called by Collision_system).
- `other` — The other collider

```cpp
void fire_stay(Collider& other);
```
Fire the stay callback (called by Collision_system).
- `other` — The other collider

```cpp
void fire_exit(Collider& other);
```
Fire the exit callback (called by Collision_system).
- `other` — The other collider


### `class Collision_system`

Detects overlaps between all active Colliders in a World and fires enter / stay / exit callbacks.

Call update(world) once per fixed step (after movement). It gathers every enabled Collider on active, non-destroyed objects, tests each unique pair, and tracks which pairs were touching last step so it can distinguish enter (new), stay (still), and exit (no longer) and fire the matching callbacks on both colliders.

```cpp
collision_system.update(world);   // in Scene::fixed_update after world step
```

The current broad test is O(n^2) pairwise, which is fine for the hundreds of colliders a typical 2D game has; a spatial grid can replace _broad_phase later without changing the callback contract.

**Methods**

```cpp
void update(World& world);
```
Test all collider pairs and fire enter/stay/exit callbacks.
- `world` — World whose colliders to test

```cpp
void clear();
```
Forget all tracked contacts (e.g. on scene reset).


### `class Game_component`

Base class for all behavior/data attached to a Game_object.

Components are the unit of composition: a Game_object is little more than a named bag of components plus a Transform. Each component gets lifecycle hooks mirroring the engine loop and a back-pointer to its owner.

Lifecycle:

```cpp
on_attach()        once, when added to an object that's in the world
on_start()         once, on the first frame the object is active
```

fixed_update(dt) zero+ times/frame (physics, gameplay) update(dt) once/frame (animation, input-driven logic) render(renderer) once/frame (visual components)

```cpp
on_detach()        once, when removed or the object is destroyed
```

Components reach siblings through owner(): e.g. a Sprite_renderer reads the Transform via owner().transform().

**Methods**

```cpp
virtual void on_attach();
```
Called once when the component is attached to its owner.

```cpp
virtual void on_start();
```
Called once on the first frame the owning object is active.

```cpp
virtual void on_detach();
```
Called once when the component is removed or its owner destroyed.

```cpp
virtual void fixed_update(float fixed_dt);
```
Fixed-timestep update for physics/gameplay logic.
- `fixed_dt` — Fixed delta time in seconds

```cpp
virtual void update(float dt);
```
Per-frame update for animation and input-driven logic.
- `dt` — Frame delta time in seconds

```cpp
virtual void render(render::Renderer& renderer);
```
Per-frame render hook for visual components.
- `renderer` — Renderer to submit drawables to

```cpp
bool is_enabled() const;
```
Check whether the component is enabled.
- *returns* — True if enabled (receives updates/renders)

```cpp
void set_enabled(bool enabled);
```
Enable or disable the component.
- `enabled` — True to enable, false to skip its updates/renders

```cpp
Game_object& owner();
```
Get the owning Game_object.
- *returns* — Reference to the owner

```cpp
Game_object const& owner() const;
```
Get the owning Game_object (const).
- *returns* — Const reference to the owner

```cpp
bool has_owner() const;
```
Check whether the component is attached to an owner.
- *returns* — True if an owner is set


### `class Game_object`

An entity in the game World: a name, a unique id, a built-in Transform, and a set of components that give it behavior and appearance.

Composition over inheritance: instead of subclassing per entity type, you assemble objects from components:

```cpp
auto* enemy = world.spawn("enemy");
enemy->transform().set_position(100.f, 50.f);
enemy->add_component<Sprite_renderer>()->set_texture("orc");
enemy->add_component<Health>(30);
enemy->add_component<Script>()->set_update_fn(...);
```

Typed access: `add_component<T>`(args...) constructs and returns T*; `get_component<T>`() returns the first T* or nullptr; `has_component<T>`() tests.

Lifecycle is driven by World, which forwards fixed_update/update/render to all enabled components and fires on_start() the first active frame.

Destruction is deferred: destroy() marks the object; World removes it at a safe point so a component can destroy its own object mid-update.

**Methods**

```cpp
explicit Game_object(std::string name, std::uint64_t id);
```
Construct a game object with a name and unique id.
- `name` — Display/lookup name
- `id` — Unique identifier assigned by the World

```cpp
std::string const& name() const;
```
Get the object's name.
- *returns* — Reference to the name string

```cpp
void set_name(std::string name);
```
Set the object's name.
- `name` — New name

```cpp
std::uint64_t id() const;
```
Get the object's unique id.
- *returns* — Unique identifier

```cpp
bool is_active() const;
```
Check whether the object is active.
- *returns* — True if active (receives updates/renders)

```cpp
void set_active(bool active);
```
Activate or deactivate the object.
- `active` — True to activate, false to skip its updates/renders

```cpp
void destroy();
```
Mark the object for destruction (deferred; World reaps it).

```cpp
bool is_destroyed() const;
```
Check whether the object is marked for destruction.
- *returns* — True if destroy() was called

```cpp
Transform& transform();
```
Get the built-in Transform component.
- *returns* — Reference to the object's Transform

```cpp
Transform const& transform() const;
```
Get the built-in Transform component (const).
- *returns* — Const reference to the object's Transform

```cpp
T* add_component(Args&&... args);
```

```cpp
T* get_component();
```

```cpp
bool has_component();
```

```cpp
void remove_component();
```

```cpp
World& world();
```
Get the owning World.
- *returns* — Reference to the World

```cpp
World const& world() const;
```
Get the owning World (const).
- *returns* — Const reference to the World

```cpp
bool has_world() const;
```
Check whether the object belongs to a World.
- *returns* — True if a World is set

```cpp
void fixed_update(float fixed_dt);
```
Fixed-timestep update; forwards to all enabled components.
- `fixed_dt` — Fixed delta time in seconds

```cpp
void update(float dt);
```
Per-frame update; forwards to all enabled components.
- `dt` — Frame delta time in seconds

```cpp
void render(render::Renderer& renderer);
```
Per-frame render; forwards to all enabled components.
- `renderer` — Renderer to submit drawables to


### `class Health : public Game_component`

Hit points with damage / heal helpers and death handling.

Tracks current and max HP, fires callbacks on damage, heal, and death, and (by default) destroys the owning object when HP reaches zero. The death callback runs before destruction so you can spawn loot, play an animation, or cancel the auto-destroy by calling set_destroy_on_death(false).

```cpp
auto* hp = obj->add_component<Health>(100);
hp->set_on_death([](Game_object& self){ spawn_explosion(self); });
hp->damage(30);
```

**Methods**

```cpp
explicit Health(int max_hp);
```
Construct with a specified max HP (full).
- `max_hp` — Maximum hit points (clamped to >= 1)

```cpp
int current() const;
```
Get current hit points.
- *returns* — Current HP

```cpp
int max() const;
```
Get maximum hit points.
- *returns* — Max HP

```cpp
bool is_alive() const;
```
Check whether the entity is alive.
- *returns* — True if not dead

```cpp
bool is_dead() const;
```
Check whether the entity is dead.
- *returns* — True if dead

```cpp
void set_max(int max_hp, bool refill = true);
```
Set the maximum HP.
- `max_hp` — New maximum (clamped to >= 1)
- `refill` — If true, refill to full and clear dead state

```cpp
void damage(int amount);
```
Apply damage; may trigger death and auto-destroy.
- `amount` — Damage amount (ignored if <= 0)

```cpp
void heal(int amount);
```
Heal up to max (no effect if dead).
- `amount` — Heal amount (ignored if <= 0)

```cpp
void revive(int to_hp = -1);
```
Revive and set HP, clearing the dead state.
- `to_hp` — Target HP, or -1 for full

```cpp
void set_destroy_on_death(bool value);
```
Set whether reaching 0 HP destroys the owner.
- `value` — True to auto-destroy on death

```cpp
bool destroy_on_death() const;
```
Check whether death auto-destroys the owner.
- *returns* — True if auto-destroy is enabled

```cpp
void set_on_damage(Amount_fn fn);
```
Set the callback fired on damage.
- `fn` — Callback receiving the owner and damage amount

```cpp
void set_on_heal(Amount_fn fn);
```
Set the callback fired on heal.
- `fn` — Callback receiving the owner and heal amount

```cpp
void set_on_death(Health_fn fn);
```
Set the callback fired on death.
- `fn` — Callback receiving the owner


### `class Rigidbody : public Game_component`

Simple kinematic motion: velocity, acceleration, forces, drag and gravity, integrated into the owner's Transform each fixed step.

This is deliberately lightweight (not a full physics engine). It moves the Transform by integrating velocity semi-implicitly, which is stable for top-down movement, platformer-ish gravity, projectiles, knockback, etc. Pair with a Collider + Collision_system for response logic in callbacks.

```cpp
auto* rb = obj->add_component<Rigidbody>();
rb->set_gravity({ 0.f, 980.f });
rb->add_force({ 200.f, 0.f });          // one-shot impulse-ish push
rb->set_velocity({ 50.f, 0.f });
```

**Methods**

```cpp
void set_velocity(sf::Vector2f v);
```
Set the velocity.
- `v` — Velocity in pixels/second

```cpp
sf::Vector2f velocity() const;
```
Get the velocity.
- *returns* — Velocity in pixels/second

```cpp
void set_acceleration(sf::Vector2f a);
```
Set the constant base acceleration.
- `a` — Acceleration in pixels/second^2

```cpp
sf::Vector2f acceleration() const;
```
Get the constant base acceleration.
- *returns* — Acceleration in pixels/second^2

```cpp
void add_force(sf::Vector2f force);
```
Accumulate a force for the next fixed step (scaled by 1/mass).
- `force` — Force vector; cleared after the next integration

```cpp
void set_mass(float mass);
```
Set the mass (clamped to a small positive minimum).
- `mass` — Mass value

```cpp
float mass() const;
```
Get the mass.
- *returns* — Mass value

```cpp
void set_gravity(sf::Vector2f g);
```
Set the gravity acceleration.
- `g` — Gravity in pixels/second^2

```cpp
sf::Vector2f gravity() const;
```
Get the gravity acceleration.
- *returns* — Gravity in pixels/second^2

```cpp
void set_linear_damping(float damping);
```
Set linear damping in [0,1): fraction of velocity lost per second.
- `damping` — Damping factor (0 = none)

```cpp
float linear_damping() const;
```
Get the linear damping factor.
- *returns* — Damping factor

```cpp
void fixed_update(float fixed_dt) override;
```
Integrate motion and move the owner's Transform.
- `fixed_dt` — Fixed delta time in seconds


### `class Script : public Game_component`

A component that runs user-provided callbacks for the lifecycle hooks, so simple behaviors don't need a whole new component subclass.

```cpp
auto* s = obj->add_component<Script>();
s->set_start_fn ([](Game_object& self){ ... });
s->set_update_fn([](Game_object& self, float dt){
    self.transform().move({ 10.f * dt, 0.f });
});
```

Each callback receives the owning Game_object by reference, so behavior can reach the transform, siblings, and the world.

**Methods**

```cpp
void set_start_fn(Start_fn fn);
```
Set the callback run once on start.
- `fn` — Callback receiving the owner

```cpp
void set_update_fn(Update_fn fn);
```
Set the callback run each frame.
- `fn` — Callback receiving the owner and frame delta

```cpp
void set_fixed_update_fn(Fixed_update_fn fn);
```
Set the callback run each fixed step.
- `fn` — Callback receiving the owner and fixed delta

```cpp
void set_detach_fn(Detach_fn fn);
```
Set the callback run on detach.
- `fn` — Callback receiving the owner

```cpp
void on_start() override;
```
Invoke the start callback, if set.

```cpp
void on_detach() override;
```
Invoke the detach callback, if set.

```cpp
void update(float dt) override;
```
Invoke the update callback, if set.
- `dt` — Frame delta time in seconds

```cpp
void fixed_update(float fixed_dt) override;
```
Invoke the fixed-update callback, if set.
- `fixed_dt` — Fixed delta time in seconds


### `class Sprite_renderer : public Game_component`

Draws a textured sprite at the owner's world transform.

Pulls the texture from the Resource_manager by name, builds an sf::Sprite each frame positioned/rotated/scaled by the owner's Transform, and submits it to the renderer on a chosen layer (default ENTITIES). A keepalive sprite is stored so it outlives end_frame() per the renderer's contract.

```cpp
auto* sr = obj->add_component<Sprite_renderer>();
sr->set_texture("player");
sr->set_origin_centered(true);
```

**Methods**

```cpp
void set_texture(std::string const& resource_name);
```
Set the texture by resource name.
- `resource_name` — Texture key in the Resource_manager

```cpp
std::string const& texture_name() const;
```
Get the current texture resource name.
- *returns* — Reference to the texture name string

```cpp
void set_texture_rect(sf::IntRect rect);
```
Restrict drawing to a sub-rectangle of the texture (for atlases).
- `rect` — Source rectangle within the texture

```cpp
void clear_texture_rect();
```
Clear any texture sub-rectangle (draw the whole texture).

```cpp
void set_color(sf::Color color);
```
Set the sprite tint/color.
- `color` — Tint color (white = untinted)

```cpp
sf::Color color() const;
```
Get the sprite tint/color.
- *returns* — Current tint color

```cpp
void set_origin_centered(bool centered);
```
Center the sprite origin (vs top-left).
- `centered` — True to center the origin

```cpp
void set_layer(render::Render_layer layer);
```
Set the render layer for submission.
- `layer` — Render layer

```cpp
void set_sort_key(float key);
```
Set the sort key within the layer (higher draws later).
- `key` — Sort key

```cpp
void on_start() override;
```
Acquire the texture from the Resource_manager on start.

```cpp
void render(render::Renderer& renderer) override;
```
Build and submit the sprite for this frame.
- `renderer` — Renderer to submit to


### `class Transform : public Game_component`

Position / rotation / scale of a Game_object in world space, with optional parenting for hierarchical transforms.

Every Game_object owns a Transform implicitly (created in its constructor), so you rarely add one manually. Other components read it via owner().transform().

Local vs world: if the object has a transform parent, local_* values are relative to the parent and world_* compose up the chain. With no parent, local and world are identical.

**Methods**

```cpp
sf::Vector2f position() const;
```
Get the local position.
- *returns* — Local position in pixels

```cpp
void set_position(sf::Vector2f p);
```
Set the local position.
- `p` — New local position

```cpp
void set_position(float x, float y);
```
Set the local position from components.
- `x` — X coordinate
- `y` — Y coordinate

```cpp
void move(sf::Vector2f delta);
```
Translate the local position by a delta.
- `delta` — Offset to add to the current position

```cpp
float rotation() const;
```
Get the local rotation.
- *returns* — Rotation in degrees

```cpp
void set_rotation(float degrees);
```
Set the local rotation.
- `degrees` — New rotation in degrees

```cpp
void rotate(float degrees);
```
Rotate the local rotation by a delta.
- `degrees` — Degrees to add to the current rotation

```cpp
sf::Vector2f scale() const;
```
Get the local scale.
- *returns* — Scale factors per axis

```cpp
void set_scale(sf::Vector2f s);
```
Set the local scale.
- `s` — New scale factors per axis

```cpp
void set_scale(float x, float y);
```
Set the local scale from components.
- `x` — X scale factor
- `y` — Y scale factor

```cpp
sf::Vector2f world_position() const;
```
Get the world position, composing the parent chain.
- *returns* — World position in pixels

```cpp
float world_rotation() const;
```
Get the world rotation, composing the parent chain.
- *returns* — World rotation in degrees

```cpp
sf::Vector2f world_scale() const;
```
Get the world scale, composing the parent chain.
- *returns* — World scale factors per axis

```cpp
sf::Transform world_matrix() const;
```
Get the combined world transform matrix.
- *returns* — SFML transform suitable for RenderStates or point transforms

```cpp
void set_parent(Transform* parent);
```
Set the parent transform (nullptr to detach).
- `parent` — Parent transform, or nullptr for none

```cpp
Transform* parent() const;
```
Get the parent transform.
- *returns* — Parent transform, or nullptr if none


### `class World`

Container and driver for all Game_objects in a scene.

Spawns objects (assigning unique ids), forwards the engine loop to them, and reaps destroyed objects at a safe point so components can destroy objects (including their own) mid-update without invalidating iteration.

A Scene typically owns one World:

```cpp
class Gameplay : public scene::Scene {
    game::World _world;
    void fixed_update(float dt) override { _world.fixed_update(dt); }
    void update(float dt)       override { _world.update(dt); }
    void render(render::Renderer& r) override { _world.render(r); }
};
```

**Methods**

```cpp
Game_object* spawn(std::string name = "object");
```
Create a new Game_object in the world.
- `name` — Optional name (defaults to "object")
- *returns* — Non-owning pointer to the new object (owned by the World)

```cpp
void clear();
```
Immediately destroy and remove all objects.

```cpp
Game_object* find(std::string const& name);
```
Find the first live object with the given name.
- `name` — Name to search for
- *returns* — Pointer to the object, or nullptr if none match

```cpp
Game_object* find_by_id(std::uint64_t id);
```
Find the live object with the given id.
- `id` — Unique identifier to search for
- *returns* — Pointer to the object, or nullptr if not found

```cpp
std::size_t count() const;
```
Get the number of objects (live plus pending-spawn).
- *returns* — Object count

```cpp
std::vector<std::unique_ptr<Game_object>>& objects();
```
Access all live objects (for systems that iterate).
- *returns* — Reference to the object vector

```cpp
std::vector<std::unique_ptr<Game_object>> const& objects() const;
```
Access all live objects (const).
- *returns* — Const reference to the object vector

```cpp
void fixed_update(float fixed_dt);
```
Fixed-timestep update; forwards to all objects.
- `fixed_dt` — Fixed delta time in seconds

```cpp
void update(float dt);
```
Per-frame update; merges spawns, updates objects, reaps destroyed.
- `dt` — Frame delta time in seconds

```cpp
void render(render::Renderer& renderer);
```
Per-frame render; forwards to all objects.
- `renderer` — Renderer to submit drawables to



## User Interface

### enum `Layout_mode`

### enum `Text_align`

### enum `UI_anchor`

Nine-point anchor for positioning widgets relative to their parent.

The anchor defines which point of the parent the widget's offset is measured from.

| Value | Meaning |
| --- | --- |
| `TOP_LEFT` | — |
| `TOP_CENTER` | — |
| `TOP_RIGHT` | — |
| `MIDDLE_LEFT` | — |
| `MIDDLE_CENTER` | — |
| `MIDDLE_RIGHT` | — |
| `BOTTOM_LEFT` | — |
| `BOTTOM_CENTER` | — |
| `BOTTOM_RIGHT` | — |

### enum `Widget_state`

Interaction state of a widget. Drives which Visual is shown and which text color is used.

| Value | Meaning |
| --- | --- |
| `NORMAL` | — |
| `HOVERED` | — |
| `PRESSED` | — |
| `FOCUSED` | — |
| `DISABLED` | — |
| `COUNT` | — |

### `class Button : public Widget`

Clickable push button with text and per-state visuals.

**Methods**

```cpp
explicit Button(std::string const& name);
```
Construct a Button.
- `name` — Widget name (unique within its parent)

```cpp
void set_text(std::string const& text);
```
Set the text.
- `text` — New text

```cpp
std::string const& text() const;
```
Get the text.
- *returns* — The text

```cpp
void set_on_click(std::function<void()> cb);
```
Set the "on click" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Checkbox : public Widget`

Boolean checkbox with a label drawn to the right of the box.

**Methods**

```cpp
explicit Checkbox(std::string const& name);
```
Construct a Checkbox.
- `name` — Widget name (unique within its parent)

```cpp
void set_label(std::string const& label);
```
Set the label.
- `label` — New label

```cpp
void set_checked(bool checked);
```
Set the checked.
- `checked` — New checked

```cpp
bool is_checked() const;
```
Check whether checked.
- *returns* — True if checked

```cpp
void set_on_changed(std::function<void(bool)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Context_menu : public Widget`

A popup list of clickable entries shown at a position.

**Methods**

```cpp
explicit Context_menu(std::string const& name);
```
Construct a Context_menu.
- `name` — Widget name (unique within its parent)

```cpp
void add_entry(std::string const& label, std::function<void()> action);
```
Add entry.
- `label` — label

```cpp
void clear();
```
Clear.

```cpp
void show_at(sf::Vector2f pos);
```
Show at.
- `pos` — pos

```cpp
void hide();
```
Hide.

```cpp
bool is_shown() const;
```
Check whether shown.
- *returns* — True if shown

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Draw_buffer`

Per-widget persistent storage for drawables submitted each frame.

The Renderer requires submitted drawables to remain alive until end_frame(). Widgets build transient shapes/text each frame; this buffer owns them for the duration of the frame so the references the Renderer holds stay valid.

Call clear() at the start of each render, then use the add_* helpers which store the drawable and submit a stable reference to the Renderer.

std::deque is used because it does not invalidate references on push_back, unlike std::vector - critical since the Renderer holds pointers into here.

**Methods**

```cpp
void clear();
```


### `class Dropdown : public Widget`

Drop-down selector. Expands an option list on click (UI_OVERLAY).

**Methods**

```cpp
explicit Dropdown(std::string const& name);
```
Construct a Dropdown.
- `name` — Widget name (unique within its parent)

```cpp
void set_options(std::vector<std::string> options);
```
Set the options.
- `options` — New options

```cpp
void set_selected(std::size_t index);
```
Set the selected.
- `index` — New selected

```cpp
std::size_t selected_index() const;
```
Get the selected index.
- *returns* — The selected index

```cpp
std::string const& selected_text() const;
```
Get the selected text.
- *returns* — The selected text

```cpp
void set_on_changed(std::function<void(std::size_t,std::string const&)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Focus_manager : public events::Event_listener`

Keyboard focus traversal for a UI_manager's widget tree.

Collects focusable widgets (Widget::is_focusable(), visible + enabled) in tree order and cycles focus with Tab / Shift+Tab, plus directional helpers. It defers to the UI_manager for the actual focused-widget state so mouse clicks and tab navigation stay in sync.

Construct with the UI_manager; it registers Tab handlers automatically.

```cpp
ui::Focus_manager focus(gui);   // now Tab cycles inputs in the UI
```

**Methods**

```cpp
explicit Focus_manager(UI_manager& manager);
```
Construct a Focus_manager.
- `manager` — manager

```cpp
void focus_next();
```
Move focus to the next focusable widget (wraps around).

```cpp
void focus_previous();
```
Move focus to the previous focusable widget (wraps around).

```cpp
void clear_focus();
```
Clear keyboard focus entirely.

```cpp
void set_enabled(bool enabled);
```
Whether Tab handling is active.

```cpp
bool is_enabled() const;
```
Check whether enabled.
- *returns* — True if enabled


### `class Gauge : public Widget`

Radial/arc meter for cooldowns, speed, health rings, etc. Value in [0,1]; sweeps from start_angle clockwise by up to sweep degrees.

**Methods**

```cpp
explicit Gauge(std::string const& name);
```
Construct a Gauge.
- `name` — Widget name (unique within its parent)

```cpp
void set_value(float v01);
```
Set the value.
- `v01` — New value

```cpp
float value() const;
```
Get the value.
- *returns* — The value

```cpp
void set_angles(float start_deg, float sweep_deg);
```
Set the angles.
- `start_deg` — start deg
- `sweep_deg` — sweep deg

```cpp
void set_thickness(float px);
```
Set the thickness.
- `px` — New thickness

```cpp
void set_colors(sf::Color track, sf::Color fill);
```
Set the colors.
- `track` — track
- `fill` — fill

```cpp
void set_show_text(bool on);
```
Set the show text.
- `on` — New show text

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Grid : public Widget`

Container that arranges children in a grid (set columns).

**Methods**

```cpp
explicit Grid(std::string const& name);
```
Construct a Grid.
- `name` — Widget name (unique within its parent)

```cpp
void set_columns(std::size_t c);
```
Set the columns.
- `c` — New columns


### `class H_box : public Widget`

Container that arranges children left-to-right.


### `class Icon_button : public Widget`

Button showing a texture icon centered in its rect.

**Methods**

```cpp
explicit Icon_button(std::string const& name);
```
Construct a Icon_button.
- `name` — Widget name (unique within its parent)

```cpp
void set_icon(std::string const& texture_name);
```
Set the icon.
- `texture_name` — New icon

```cpp
void set_icon_size(sf::Vector2f size);
```

```cpp
void set_icon_tint(sf::Color c);
```
Set the icon tint.
- `c` — New icon tint

```cpp
void set_on_click(std::function<void()> cb);
```
Set the "on click" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Image : public Widget`

Displays a texture (optionally a sub-rect), with tint + aspect.

**Methods**

```cpp
explicit Image(std::string const& name);
```
Construct a Image.
- `name` — Widget name (unique within its parent)

```cpp
void set_texture(std::string const& resource_name);
```
Set the texture.
- `resource_name` — New texture

```cpp
void set_uv_rect(sf::IntRect rect);
```
Set the uv rect.
- `rect` — New uv rect

```cpp
void clear_uv_rect();
```
Clear uv rect.

```cpp
void set_tint(sf::Color c);
```
Set the tint.
- `c` — New tint

```cpp
void set_preserve_aspect(bool p);
```
Set the preserve aspect.
- `p` — New preserve aspect

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Item_grid : public Widget`

A grid of Item_slots with drag-drop between them built in.

**Methods**

```cpp
explicit Item_grid(std::string const& name);
```
Construct a Item_grid.
- `name` — Widget name (unique within its parent)

```cpp
void configure(std::size_t columns, std::size_t rows, float slot_size, float gap);
```
Configure.
- `columns` — columns
- `rows` — rows
- `slot_size` — slot size
- `gap` — gap

```cpp
Item_slot* slot(std::size_t index);
```
Slot.
- `index` — index

```cpp
std::size_t slot_count() const;
```
Get the slot count.
- *returns* — The slot count

```cpp
void on_layout() override;
```
Handle the layout event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Item_slot : public Widget`

An inventory slot holding an item id (>=0 = filled, -1 = empty), shown via an icon texture. Supports drag-out and drop-in using the UI_manager drag payload (carries the item id, drag_kind = "item").

**Methods**

```cpp
explicit Item_slot(std::string const& name);
```
Construct a Item_slot.
- `name` — Widget name (unique within its parent)

```cpp
void set_item(std::int64_t item_id, std::string const& icon_texture);
```
Set the item.
- `item_id` — item id
- `icon_texture` — icon texture

```cpp
void clear_item();
```
Clear item.

```cpp
std::int64_t item_id() const;
```
Get the item id.
- *returns* — The item id

```cpp
void set_count(int count);
```

```cpp
void set_on_changed(std::function<void()> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
void on_released() override;
```
Handle the released event. _(protected)_

```cpp
std::string style_key() const override;
```


### `class Label : public Widget`

A non-interactive text display with alignment and optional wrap.

**Methods**

```cpp
explicit Label(std::string const& name);
```
Construct a Label.
- `name` — Widget name (unique within its parent)

```cpp
void set_text(std::string const& text);
```
Set the text.
- `text` — New text

```cpp
std::string const& text() const;
```
Get the text.
- *returns* — The text

```cpp
void set_align(Text_align a);
```
Set the align.
- `a` — New align

```cpp
void set_wrap(bool wrap);
```
Set the wrap.
- `wrap` — New wrap

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class List_box : public Widget`

Scrollable selectable list of text items (single or multi-select).

**Methods**

```cpp
explicit List_box(std::string const& name);
```
Construct a List_box.
- `name` — Widget name (unique within its parent)

```cpp
void set_items(std::vector<std::string> items);
```
Set the items.
- `items` — New items

```cpp
void add_item(std::string const& s);
```
Add item.
- `s` — s

```cpp
void clear_items();
```
Clear items.

```cpp
void set_selected(std::size_t i);
```
Set the selected.
- `i` — New selected

```cpp
std::size_t selected() const;
```
Get the selected.
- *returns* — The selected

```cpp
std::string const& selected_text() const;
```
Get the selected text.
- *returns* — The selected text

```cpp
void set_multi_select(bool on);
```
Set the multi select.
- `on` — New multi select

```cpp
std::vector<std::size_t> const& selected_indices() const;
```
Get the selected indices.
- *returns* — The selected indices

```cpp
void set_item_height(float h);
```
Set the item height.
- `h` — New item height

```cpp
void set_on_changed(std::function<void(std::size_t)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Menu_bar : public Widget`

Horizontal menu bar with top-level entries. Each entry fires a callback. For nested submenus combine with a Context_menu.

**Methods**

```cpp
explicit Menu_bar(std::string const& name);
```
Construct a Menu_bar.
- `name` — Widget name (unique within its parent)

```cpp
void add_entry(std::string const& label, std::function<void()> action);
```
Add entry.
- `label` — label

```cpp
void clear();
```
Clear.

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Minimap : public Widget`

A minimap that maps world coordinates into its rect and draws blips, plus an optional viewport rectangle. Set the world bounds, then push blips each frame. Click maps back to world via on_clicked(world_pos).

**Methods**

```cpp
explicit Minimap(std::string const& name);
```
Construct a Minimap.
- `name` — Widget name (unique within its parent)

```cpp
void set_world_bounds(sf::Vector2f top_left, sf::Vector2f size);
```
Set the world bounds.
- `top_left` — top left
- `size` — size

```cpp
void clear_blips();
```
Clear blips.

```cpp
void add_blip(sf::Vector2f world, sf::Color color, float radius = 2.f);
```
Add blip.
- `world` — world
- `color` — color
- `radius` — radius

```cpp
void set_viewport(sf::Vector2f world_tl, sf::Vector2f world_size);
```
Set the viewport.
- `world_tl` — world tl
- `world_size` — world size

```cpp
void set_on_clicked(std::function<void(sf::Vector2f)> cb);
```
Set the "on clicked" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Modal : public Widget`

Dimmed-backdrop dialog. show()/hide(); click backdrop closes if set.

**Methods**

```cpp
explicit Modal(std::string const& name);
```
Construct a Modal.
- `name` — Widget name (unique within its parent)

```cpp
void set_title(std::string const& t);
```
Set the title.
- `t` — New title

```cpp
void set_closeable(bool c);
```
Set the closeable.
- `c` — New closeable

```cpp
void show();
```
Show.

```cpp
void set_on_close(std::function<void()> cb);
```
Set the "on close" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Nine_slice_panel : public Widget`

A panel whose background is a nine-slice texture.

**Methods**

```cpp
explicit Nine_slice_panel(std::string const& name);
```
Construct a Nine_slice_panel.
- `name` — Widget name (unique within its parent)

```cpp
void set_texture(std::string const& resource_name);
```
Set the texture.
- `resource_name` — New texture

```cpp
void set_border(float top,float right,float bottom,float left);
```
Set the border.
- `top` — top
- `right` — right
- `bottom` — bottom
- `left` — left

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Number_field : public Widget`

Numeric text entry with min/max clamping and decimals. Accepts only digits, '-', and '.'. Commits on Enter or focus loss.

**Methods**

```cpp
explicit Number_field(std::string const& name);
```
Construct a Number_field.
- `name` — Widget name (unique within its parent)

```cpp
void set_range(double min,double max);
```
Set the range.
- `min` — min
- `max` — max

```cpp
void set_value(double v);
```
Set the value.
- `v` — New value

```cpp
double value() const;
```
Get the value.
- *returns* — The value

```cpp
void set_decimals(int d);
```
Set the decimals.
- `d` — New decimals

```cpp
void set_on_changed(std::function<void(double)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Panel : public Widget`

Styled container. Use layout modes for auto-arrangement.

**Methods**

```cpp
explicit Panel(std::string const& name);
```
Construct a Panel.
- `name` — Widget name (unique within its parent)

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Progress_bar : public Widget`

Animated fill bar [0,1], horizontal or vertical, optional % label.

**Methods**

```cpp
explicit Progress_bar(std::string const& name);
```
Construct a Progress_bar.
- `name` — Widget name (unique within its parent)

```cpp
void set_value(float v);
```
Set the value.
- `v` — New value

```cpp
float value() const;
```
Get the value.
- *returns* — The value

```cpp
void set_animated(bool on, float speed = 3.f);
```
Set the animated.
- `on` — on
- `speed` — speed

```cpp
void set_orientation(Orientation o);
```
Set the orientation.
- `o` — New orientation

```cpp
void set_show_label(bool show);
```
Set the show label.
- `show` — New show label

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Radial_menu : public Widget`

Pie/wheel selection menu. show_at() opens it centered at a point; the hovered wedge highlights; release selects.

**Methods**

```cpp
explicit Radial_menu(std::string const& name);
```
Construct a Radial_menu.
- `name` — Widget name (unique within its parent)

```cpp
void set_entries(std::vector<std::string> entries);
```
Set the entries.
- `entries` — New entries

```cpp
void set_radius(float inner, float outer);
```
Set the radius.
- `inner` — inner
- `outer` — outer

```cpp
void show_at(sf::Vector2f center);
```
Show at.
- `center` — center

```cpp
void hide();
```
Hide.

```cpp
bool is_shown() const;
```
Check whether shown.
- *returns* — True if shown

```cpp
void set_on_selected(std::function<void(std::size_t,std::string const&)> cb);
```
Set the "on selected" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Radio_button : public Widget`

One option in a Radio_group.

**Methods**

```cpp
explicit Radio_button(std::string const& name);
```
Construct a Radio_button.
- `name` — Widget name (unique within its parent)

```cpp
void set_label(std::string const& l);
```
Set the label.
- `l` — New label

```cpp
bool is_selected() const;
```
Check whether selected.
- *returns* — True if selected

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Radio_group`

Manages mutual exclusion across a set of Radio_buttons.

**Methods**

```cpp
Radio_button* add(Widget& parent, std::string const& name, std::string const& label);
```
Add.
- `parent` — parent
- `name` — Widget name (unique within its parent)
- `label` — label

```cpp
void select(std::size_t index);
```
Select.
- `index` — index

```cpp
std::size_t selected() const;
```
Get the selected.
- *returns* — The selected

```cpp
void set_on_changed(std::function<void(std::size_t)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke


### `class Resizable_window : public Widget`

Draggable + resizable titled window. Drag title to move, drag the bottom-right corner handle to resize.

**Methods**

```cpp
explicit Resizable_window(std::string const& name);
```
Construct a Resizable_window.
- `name` — Widget name (unique within its parent)

```cpp
void set_title(std::string const& t);
```
Set the title.
- `t` — New title

```cpp
void set_min_size(sf::Vector2f s);
```
Set the min size.
- `s` — New min size

```cpp
void set_title_height(float h);
```
Set the title height.
- `h` — New title height

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
void on_released() override;
```
Handle the released event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Rich_text : public Widget`

Text composed of styled spans (per-span color + size), laid out left-to-right with wrapping. Build with add_span(); good for log windows, dialogue with colored names, damage numbers, etc.

**Methods**

```cpp
explicit Rich_text(std::string const& name);
```
Construct a Rich_text.
- `name` — Widget name (unique within its parent)

```cpp
void clear_spans();
```
Clear spans.

```cpp
void add_span(std::string const& text, sf::Color color, float size = 14.f);
```
Add span.
- `text` — text
- `color` — color
- `size` — size

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Scroll_view : public Widget`

Scrollable viewport. Set content size; children are offset by scroll. Mouse wheel scrolls while hovered. Draws a thumb on the active axis.

**Methods**

```cpp
explicit Scroll_view(std::string const& name);
```
Construct a Scroll_view.
- `name` — Widget name (unique within its parent)

```cpp
void set_dir(Dir d);
```
Set the dir.
- `d` — New dir

```cpp
void set_speed(float s);
```
Set the speed.
- `s` — New speed

```cpp
void set_content_size(sf::Vector2f s);
```
Set the content size.
- `s` — New content size

```cpp
void scroll_to(sf::Vector2f off);
```
Scroll to.
- `off` — off

```cpp
sf::Vector2f scroll_offset() const;
```
Get the scroll offset.
- *returns* — The scroll offset

```cpp
void on_layout() override;
```
Handle the layout event. _(protected)_

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Segmented_control : public Widget`

Horizontal row of mutually-exclusive segments (like iOS segmented control).

**Methods**

```cpp
explicit Segmented_control(std::string const& name);
```
Construct a Segmented_control.
- `name` — Widget name (unique within its parent)

```cpp
void set_segments(std::vector<std::string> segs);
```
Set the segments.
- `segs` — New segments

```cpp
void set_selected(std::size_t i);
```
Set the selected.
- `i` — New selected

```cpp
std::size_t selected() const;
```
Get the selected.
- *returns* — The selected

```cpp
void set_on_changed(std::function<void(std::size_t)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Separator : public Widget`

Thin horizontal/vertical divider line.

**Methods**

```cpp
explicit Separator(std::string const& name);
```
Construct a Separator.
- `name` — Widget name (unique within its parent)

```cpp
void set_orientation(Orientation o);
```
Set the orientation.
- `o` — New orientation

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Slider : public Widget`

Horizontal/vertical value slider with draggable knob.

**Methods**

```cpp
explicit Slider(std::string const& name);
```
Construct a Slider.
- `name` — Widget name (unique within its parent)

```cpp
void set_range(float min, float max);
```
Set the range.
- `min` — min
- `max` — max

```cpp
void set_value(float v);
```
Set the value.
- `v` — New value

```cpp
float value() const;
```
Get the value.
- *returns* — The value

```cpp
void set_step(float step);
```
Set the step.
- `step` — New step

```cpp
void set_orientation(Orientation o);
```
Set the orientation.
- `o` — New orientation

```cpp
void set_on_changed(std::function<void(float)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
void on_released() override;
```
Handle the released event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Spacer : public Widget`

Invisible fixed-size gap for use inside layout containers.

**Methods**

```cpp
explicit Spacer(std::string const& name);
```
Construct a Spacer.
- `name` — Widget name (unique within its parent)


### `class Spin_box : public Widget`

Numeric spin edit: [-] [value] [+], all skinnable. The two buttons and the value label are internal child widgets exposed via minus_button()/plus_button()/value_field() so you can restyle them (e.g. give the buttons textures) or reposition them relative to each other.

**Methods**

```cpp
explicit Spin_box(std::string const& name);
```
Construct a Spin_box.
- `name` — Widget name (unique within its parent)

```cpp
void set_range(float min,float max);
```
Set the range.
- `min` — min
- `max` — max

```cpp
void set_value(float v);
```
Set the value.
- `v` — New value

```cpp
float value() const;
```
Get the value.
- *returns* — The value

```cpp
void set_step(float step);
```
Set the step.
- `step` — New step

```cpp
void set_decimals(int d);
```
Set the decimals.
- `d` — New decimals

```cpp
void set_value_width(UI_length w);
```
Set the value width.
- `w` — New value width

```cpp
void set_on_changed(std::function<void(float)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
Button* minus_button();
```
Get the minus button.
- *returns* — The minus button

```cpp
Button* plus_button();
```
Get the plus button.
- *returns* — The plus button

```cpp
Label* value_field();
```
Get the value field.
- *returns* — The value field

```cpp
void on_layout() override;
```
Handle the layout event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Tab_view : public Widget`

Tabbed container; one child shown at a time, tab strip on top.

**Methods**

```cpp
explicit Tab_view(std::string const& name);
```
Construct a Tab_view.
- `name` — Widget name (unique within its parent)

```cpp
void add_tab(std::string const& tab_name, std::unique_ptr<Widget> content);
```
Add tab.
- `tab_name` — tab name
- `content` — content

```cpp
void set_active_tab(std::size_t i);
```
Set the active tab.
- `i` — New active tab

```cpp
std::size_t active_tab() const;
```
Get the active tab.
- *returns* — The active tab

```cpp
void set_tab_height(float h);
```
Set the tab height.
- `h` — New tab height

```cpp
void set_on_tab_changed(std::function<void(std::size_t,std::string const&)> cb);
```
Set the "on tab changed" callback.
- `cb` — Callback to invoke

```cpp
void on_layout() override;
```
Handle the layout event. _(protected)_

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Table : public Widget`

Simple data table: column headers + rows of cell strings.

**Methods**

```cpp
explicit Table(std::string const& name);
```
Construct a Table.
- `name` — Widget name (unique within its parent)

```cpp
void set_columns(std::vector<std::string> headers);
```
Set the columns.
- `headers` — New columns

```cpp
void set_column_widths(std::vector<float> widths);
```

```cpp
void clear_rows();
```
Clear rows.

```cpp
void add_row(std::vector<std::string> cells);
```
Add row.
- `cells` — cells

```cpp
std::size_t row_count() const;
```
Get the row count.
- *returns* — The row count

```cpp
void set_row_height(float h);
```
Set the row height.
- `h` — New row height

```cpp
void set_on_row_clicked(std::function<void(std::size_t)> cb);
```
Set the "on row clicked" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `struct Text_appearance`

How text is drawn for a widget, per state where desired.


### `class Text_area : public Widget`

Multi-line editable text box with wrapping and wheel scrolling.

**Methods**

```cpp
explicit Text_area(std::string const& name);
```
Construct a Text_area.
- `name` — Widget name (unique within its parent)

```cpp
void set_text(std::string const& t);
```
Set the text.
- `t` — New text

```cpp
std::string const& text() const;
```
Get the text.
- *returns* — The text

```cpp
void set_placeholder(std::string const& p);
```
Set the placeholder.
- `p` — New placeholder

```cpp
void set_on_changed(std::function<void(std::string const&)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Text_field : public Widget`

Single-line editable text input. Click to focus, type to edit, Enter to submit, Backspace to delete. Supports placeholder + password.

**Methods**

```cpp
explicit Text_field(std::string const& name);
```
Construct a Text_field.
- `name` — Widget name (unique within its parent)

```cpp
void set_text(std::string const& t);
```
Set the text.
- `t` — New text

```cpp
std::string const& text() const;
```
Get the text.
- *returns* — The text

```cpp
void set_placeholder(std::string const& p);
```
Set the placeholder.
- `p` — New placeholder

```cpp
void set_max_length(std::size_t n);
```
Set the max length.
- `n` — New max length

```cpp
void set_password(bool on);
```
Set the password.
- `on` — New password

```cpp
void set_on_submit(std::function<void(std::string const&)> cb);
```
Set the "on submit" callback.
- `cb` — Callback to invoke

```cpp
void set_on_changed(std::function<void(std::string const&)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `class Theme`

Holds default Visual_sets keyed by widget style name ("button", "label", "panel", "slider_track", ...).

Widgets look up their defaults via style_key(); users override per-widget on top. Themes can be built in code or loaded from a TOML config (sections named `"ui.<key>"`) using your Config system.

A built-in dark theme is provided via Theme::dark() so the UI looks reasonable out of the box with zero setup.

**Methods**

```cpp
void set(std::string const& key, Visual_set vs);
```
Set.
- `key` — key
- `vs` — vs

```cpp
Visual_set const& get(std::string const& key) const;
```
Get.
- `key` — key

```cpp
bool has(std::string const& key) const;
```
Has.
- `key` — key

```cpp
void load_from_config(core::Config const& config);
```
Load from config.
- `config` — config

```cpp
static Theme dark();
```
Get the dark.
- *returns* — The dark


### `class Toast : public Widget`

Brief notification that auto-fades out.

**Methods**

```cpp
explicit Toast(std::string const& name);
```
Construct a Toast.
- `name` — Widget name (unique within its parent)

```cpp
void set_text(std::string const& t);
```
Set the text.
- `t` — New text

```cpp
void show(float duration_seconds = 3.f);
```
Show.
- `duration_seconds` — duration seconds

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Toggle : public Widget`

Animated on/off switch with a sliding knob and optional label.

**Methods**

```cpp
explicit Toggle(std::string const& name);
```
Construct a Toggle.
- `name` — Widget name (unique within its parent)

```cpp
void set_on(bool on);
```
Set the on.
- `on` — New on

```cpp
bool is_on() const;
```
Check whether on.
- *returns* — True if on

```cpp
void set_label(std::string const& label);
```
Set the label.
- `label` — New label

```cpp
void set_on_changed(std::function<void(bool)> cb);
```
Set the "on changed" callback.
- `cb` — Callback to invoke

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_clicked() override;
```
Handle the clicked event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Tooltip : public Widget`

Hover popup that fades in after a delay and follows the mouse.

**Methods**

```cpp
explicit Tooltip(std::string const& name);
```
Construct a Tooltip.
- `name` — Widget name (unique within its parent)

```cpp
void set_text(std::string const& t);
```
Set the text.
- `t` — New text

```cpp
void set_delay(float seconds);
```
Set the delay.
- `seconds` — New delay

```cpp
void attach_to(Widget* target);
```
Attach to.
- `target` — target

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string

```cpp
bool draws_own_background() const override;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background


### `class Tree_view : public Widget`

Expandable tree of labeled nodes.

**Methods**

```cpp
Node* add_child(std::string const& l);
```
Add child.
- `l` — l

```cpp
explicit Tree_view(std::string const& name);
```
Construct a Tree_view.
- `name` — Widget name (unique within its parent)

```cpp
Node& root_node();
```
Get the root node.
- *returns* — The root node

```cpp
void set_row_height(float h);
```
Set the row height.
- `h` — New row height

```cpp
void set_indent(float i);
```
Set the indent.
- `i` — New indent

```cpp
void set_on_selected(std::function<void(Node const&)> cb);
```
Set the "on selected" callback.
- `cb` — Callback to invoke

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `struct UI_length`

A length value that can be either absolute pixels or a percentage of the parent's corresponding dimension.


### `class UI_manager`

Owns the widget tree, the active Theme, focus and hover tracking, and drives the per-frame layout/update/render passes.

**Methods**

```cpp
Widget* add(std::unique_ptr<Widget> w);
```
Add.
- `w` — w

```cpp
Widget* find(std::string const& name);
```
Find.
- `name` — Widget name (unique within its parent)

```cpp
void remove(std::string const& name);
```
Remove.
- `name` — Widget name (unique within its parent)

```cpp
void clear();
```
Clear.

```cpp
Widget* root();
```
Get the root.
- *returns* — The root

```cpp
void set_theme(Theme theme);
```
Set the theme.
- `theme` — New theme

```cpp
Theme& theme();
```
Get the theme.
- *returns* — The theme

```cpp
Theme const& theme() const;
```
Get the theme.
- *returns* — The theme

```cpp
void load_theme(core::Config const& config);
```
Load theme.
- `config` — config

```cpp
void set_focused(Widget* w);
```
Set the focused.
- `w` — New focused

```cpp
Widget* focused() const;
```
Get the focused.
- *returns* — The focused

```cpp
render::Renderer& renderer();
```
Get the renderer.
- *returns* — The renderer

```cpp
resources::Resource_manager& resource_manager();
```
Get the resource manager.
- *returns* — The resource manager

```cpp
sf::Vector2f mouse_position() const;
```
Get the mouse position.
- *returns* — The mouse position

```cpp
void begin_drag(std::any payload, std::int64_t kind = 0);
```
Begin drag.
- `payload` — payload
- `kind` — kind

```cpp
bool is_dragging() const;
```
Check whether dragging.
- *returns* — True if dragging

```cpp
std::int64_t drag_kind() const;
```
Get the drag kind.
- *returns* — The drag kind

```cpp
std::any const& drag_payload() const;
```
Get the drag payload.
- *returns* — The drag payload

```cpp
void end_drag();
```
End drag.

```cpp
void layout();
```
Layout.

```cpp
void update(float dt);
```
Update.
- `dt` — dt

```cpp
void render();
```
Render.

```cpp
void set_mouse_position(sf::Vector2f p);
```
Set the mouse position.
- `p` — New mouse position


### `class UI_system`

Bundles a UI_manager + Focus_manager and wires them into the engine loop, so a Scene gets working UI with one object.

It pulls the renderer + resource manager from the Application, syncs the UI mouse position from the window each frame, runs layout/update, and renders on the UI layers. Keyboard focus traversal (Tab) is handled by the embedded Focus_manager.

```cpp
class Settings : public scene::Scene {
    ui::UI_system _ui{ application() };
    void on_enter() override { build_widgets(_ui.manager()); }
    void update(float dt) override { _ui.update(dt); }
    void render(render::Renderer& r) override { _ui.render(); }
};
```

**Methods**

```cpp
explicit UI_system(app::Application& application);
```
Construct a UI_system.
- `application` — application

```cpp
UI_manager& manager();
```
Get the manager.
- *returns* — The manager

```cpp
Focus_manager& focus();
```
Get the focus.
- *returns* — The focus

```cpp
void update(float dt);
```
Sync mouse position from the window, run layout + update.

```cpp
void render();
```
Render the UI to the renderer's UI layers.

```cpp
void layout();
```
Re-run layout (call on window resize).


### `class V_box : public Widget`

Container that arranges children top-to-bottom.


### `class Virtual_list : public Widget`

Virtualized list: only visible rows are built each frame, so it handles millions of items. Provide a count and a row-text provider.

**Methods**

```cpp
explicit Virtual_list(std::string const& name);
```
Construct a Virtual_list.
- `name` — Widget name (unique within its parent)

```cpp
void set_item_count(std::size_t n);
```
Set the item count.
- `n` — New item count

```cpp
void set_item_height(float h);
```
Set the item height.
- `h` — New item height

```cpp
void set_text_provider(std::function<std::string(std::size_t)> fn);
```
Set the text provider.
- `size_t` — New text provider

```cpp
void set_on_clicked(std::function<void(std::size_t)> cb);
```
Set the "on clicked" callback.
- `cb` — Callback to invoke

```cpp
std::size_t item_count() const;
```
Get the item count.
- *returns* — The item count

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string


### `struct Visual`

A single drawable appearance for a widget background.

A Visual can be: nothing, a solid/outlined color rect, a stretched texture, or a nine-slice texture. This is the single flexible primitive every widget uses to draw itself - no widget hardcodes its background anymore.

**Methods**

```cpp
static Visual none();
```


### `struct Visual_set`

A complete appearance for a widget across all interaction states.

Holds one Visual per state (NORMAL is the fallback for any unset state) and a text appearance per state. This is what makes every widget fully skinnable without touching widget code: assign Visuals for each state and you are done.

Any state left unset falls back to NORMAL, so the common case (only setting NORMAL + HOVERED) just works.

**Methods**

```cpp
Visual_set& set_visual(Widget_state state, Visual v);
```

```cpp
Visual_set& set_text(Widget_state state, Text_appearance t);
```

```cpp
Visual_set& normal(Visual v);
```
Normal.
- `v` — v

```cpp
Visual_set& hovered(Visual v);
```
Hovered.
- `v` — v

```cpp
Visual_set& pressed(Visual v);
```
Pressed.
- `v` — v

```cpp
Visual_set& focused(Visual v);
```
Focused.
- `v` — v

```cpp
Visual_set& disabled(Visual v);
```
Disabled.
- `v` — v

```cpp
Visual const& visual_for(Widget_state state) const;
```

```cpp
return _empty_visual();
```
Get the empty visual.
- *returns* — The empty visual

```cpp
Text_appearance const& text_for(Widget_state state) const;
```

```cpp
return _empty_text();
```
Get the empty text.
- *returns* — The empty text

```cpp
bool has_text() const;
```


### `class Widget : public events::Event_listener`

Base class for all named UI widgets.

Provides the shared machinery every widget needs: tree structure, layout (anchor + offset, px/% sizing, optional auto-arrange of children), per-state appearance via a Visual_set, visibility/enable/hover/focus/press state, and automatic registration/cleanup with the SFML event manager.

Concrete widgets (Button, Slider, ...) derive from this, add their own typed API (plain setter methods, no chaining), and override the protected hooks on_update() / on_render() / on_layout(). They draw through draw_buffer() so submitted drawables stay alive until the renderer flushes.

Styling: a widget pulls its default Visual_set from the active Theme using its style_key() (set by each subclass), then applies any per-widget overrides the user supplied via set_visual()/set_text_style().

**Methods**

```cpp
explicit Widget(std::string name);
```
Widget.
- `name` — Widget name (unique within its parent)

```cpp
std::string const& name() const;
```
Get the name.
- *returns* — The name

```cpp
Widget* add_child(std::unique_ptr<Widget> child);
```
Add child.
- `child` — child

```cpp
void remove_child(std::string const& name);
```
Remove child.
- `name` — Widget name (unique within its parent)

```cpp
Widget* find(std::string const& name);
```

```cpp
std::vector<std::unique_ptr<Widget>> const& children() const;
```
Get the children.
- *returns* — The children

```cpp
Widget* parent() const;
```
Get the parent.
- *returns* — The parent

```cpp
void set_anchor(UI_anchor a);
```
Set the anchor.
- `a` — New anchor

```cpp
void set_offset(sf::Vector2f o);
```
Set the offset.
- `o` — New offset

```cpp
void set_offset(float x, float y);
```
Set the offset.
- `x` — x
- `y` — y

```cpp
void set_width(UI_length w);
```
Set the width.
- `w` — New width

```cpp
void set_height(UI_length h);
```
Set the height.
- `h` — New height

```cpp
void set_size(UI_length w, UI_length h);
```
Set the size.
- `w` — w
- `h` — h

```cpp
void set_padding(float all);
```
Set the padding.
- `all` — New padding

```cpp
void set_padding(float top, float right, float bottom, float left);
```
Set the padding.
- `top` — top
- `right` — right
- `bottom` — bottom
- `left` — left

```cpp
void set_layout_mode(Layout_mode m);
```
Set the layout mode.
- `m` — New layout mode

```cpp
void set_spacing(float s);
```
Set the spacing.
- `s` — New spacing

```cpp
void set_grid_columns(std::size_t c);
```
Set the grid columns.
- `c` — New grid columns

```cpp
UI_anchor anchor() const;
```
Get the anchor.
- *returns* — The anchor

```cpp
sf::Vector2f offset() const;
```
Get the offset.
- *returns* — The offset

```cpp
sf::FloatRect rect() const;
```
Get the rect.
- *returns* — The rect

```cpp
sf::FloatRect inner_rect() const;
```
Get the inner rect.
- *returns* — The inner rect

```cpp
void set_visual(Widget_state state, Visual v);
```
Set the visual.
- `state` — state
- `v` — v

```cpp
void set_visual_all(Visual v);
```

```cpp
void set_text_style(Widget_state state, Text_appearance t);
```
Set the text style.
- `state` — state
- `t` — t

```cpp
void set_text_style_all(Text_appearance t);
```
Set the text style all.
- `t` — New text style all

```cpp
void set_visuals(Visual_set vs);
```
Set the visuals.
- `vs` — New visuals

```cpp
Visual_set& visuals();
```
Get the visuals.
- *returns* — The visuals

```cpp
Visual_set const& visuals() const;
```
Get the visuals.
- *returns* — The visuals

```cpp
void set_visible(bool v);
```
Set the visible.
- `v` — New visible

```cpp
bool is_visible() const;
```
Check whether visible.
- *returns* — True if visible

```cpp
void set_enabled(bool e);
```
Set the enabled.
- `e` — New enabled

```cpp
bool is_enabled() const;
```
Check whether enabled.
- *returns* — True if enabled

```cpp
bool is_hovered() const;
```
Check whether hovered.
- *returns* — True if hovered

```cpp
bool is_pressed() const;
```
Check whether pressed.
- *returns* — True if pressed

```cpp
bool is_focused() const;
```
Check whether focused.
- *returns* — True if focused

```cpp
void request_focus();
```
Request focus.

```cpp
bool is_focusable() const;
```
Whether this widget participates in keyboard focus traversal (tab order). Inputs (text fields, sliders, buttons) set this true; static widgets (labels, panels) leave it false. Default false.

```cpp
void set_focusable(bool focusable);
```
Set the focusable.
- `focusable` — New focusable

```cpp
Widget_state current_state() const;
```
Get the current state.
- *returns* — The current state

```cpp
void set_on_overlay(bool on);
```
Set the "on overlay" callback.
- `cb` — Callback to invoke

```cpp
bool on_overlay() const;
```
Handle the overlay event.

```cpp
void do_layout(sf::FloatRect const& parent_rect);
```
Do layout.
- `parent_rect` — parent rect

```cpp
void update(float dt);
```
Update.
- `dt` — dt

```cpp
void render(render::Renderer& renderer);
```
Render.
- `renderer` — renderer

```cpp
UI_manager& ui_manager();
```
Get the ui manager.
- *returns* — The ui manager

```cpp
resources::Resource_manager& resource_manager();
```
Get the resource manager.
- *returns* — The resource manager

```cpp
sf::Vector2f mouse_position() const;
```
Get the mouse position.
- *returns* — The mouse position

```cpp
virtual void on_layout();
```
Handle the layout event. _(protected)_

```cpp
virtual void on_update(float dt);
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
virtual void on_render(render::Renderer& renderer);
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
virtual std::string style_key() const;
```

```cpp
virtual bool draws_own_background() const;
```
Whether the widget draws its own background. _(protected)_
- *returns* — True if it draws its own background

```cpp
Draw_buffer& draw_buffer();
```
Get the draw buffer. _(protected)_
- *returns* — The draw buffer

```cpp
void draw_background(render::Renderer& renderer, sf::FloatRect const& dst);
```
Draw background. _(protected)_
- `renderer` — renderer
- `dst` — dst

```cpp
Visual const& resolved_visual() const;
```
Get the resolved visual. _(protected)_
- *returns* — The resolved visual

```cpp
Text_appearance const& resolved_text_style() const;
```
Get the resolved text style. _(protected)_
- *returns* — The resolved text style

```cpp
void enable_click_events();
```
Enable click events. _(protected)_

```cpp
virtual void on_pressed();
```
Handle the pressed event. _(protected)_

```cpp
virtual void on_released();
```
Handle the released event. _(protected)_

```cpp
virtual void on_clicked();
```

```cpp
void set_pressed(bool p);
```
Set the pressed. _(protected)_
- `p` — New pressed


### `class Window : public Widget`

Draggable titled window with a title bar and optional close button. Children placed inside appear below the title bar (use inner_rect).

**Methods**

```cpp
explicit Window(std::string const& name);
```
Construct a Window.
- `name` — Widget name (unique within its parent)

```cpp
void set_title(std::string const& t);
```
Set the title.
- `t` — New title

```cpp
void set_closeable(bool c);
```
Set the closeable.
- `c` — New closeable

```cpp
void set_title_height(float h);
```
Set the title height.
- `h` — New title height

```cpp
void set_on_close(std::function<void()> cb);
```
Set the "on close" callback.
- `cb` — Callback to invoke

```cpp
void on_update(float dt) override;
```
Handle the update event. _(protected)_
- `dt` — dt

```cpp
void on_render(render::Renderer& renderer) override;
```
Handle the render event. _(protected)_
- `renderer` — renderer

```cpp
void on_pressed() override;
```
Handle the pressed event. _(protected)_

```cpp
void on_released() override;
```
Handle the released event. _(protected)_

```cpp
std::string style_key() const override;
```
Get the theme style key for this widget. _(protected)_
- *returns* — Style key string



## Rendering

### enum `Render_layer`

Drawing order layers. Lower values draw first (further back).

Within a layer, items sort by sort_key (lower = further back). BACKGROUND..FOREGROUND use the world camera. UI..DEBUG use the UI camera.

| Value | Meaning |
| --- | --- |
| `BACKGROUND` | Skybox, far parallax |
| `WORLD_TILES` | Tilemap |
| `WORLD_OBJECTS` | Static decoration |
| `ENTITIES` | Characters, dynamic objects |
| `PARTICLES` | Particle effects |
| `FOREGROUND` | Near parallax, overlays |
| `UI` | HUD |
| `UI_OVERLAY` | Modals, pause menu |
| `DEBUG` | Bounding boxes, gizmos |
| `COUNT` | — |

### `class Camera`

2D camera wrapping sf::View.

Supports target following with smoothing, screen shake, zoom interpolation, and world bounds clamping. Call tick(dt) each frame.

**Methods**

```cpp
void set_target_position(sf::Vector2f const world_pos);
```
Set target position the camera smoothly moves towards.
- `world_pos` — Target world position

```cpp
void snap_to(sf::Vector2f const world_pos);
```
Instantly teleport camera to position.
- `world_pos` — Position to snap to

```cpp
void set_follow_smoothing(float const seconds_to_reach);
```
Set follow smoothing time.
- `seconds_to_reach` — 0 = instant

```cpp
sf::Vector2f position() const;
```
Get current (smoothed) camera position.
- *returns* — Current position

```cpp
sf::Vector2f target_position() const;
```
Get target position.
- *returns* — Target position

```cpp
void set_zoom(float const zoom);
```
Set zoom target (smoothed). 1.0 = nominal, higher = more zoomed in.
- `zoom` — Target zoom level

```cpp
void set_zoom_immediate(float const zoom);
```
Set zoom immediately.
- `zoom` — Zoom level

```cpp
void set_zoom_smoothing(float const seconds_to_reach);
```
Set zoom smoothing time.
- `seconds_to_reach` — 0 = instant

```cpp
float zoom() const;
```
Get current zoom level.
- *returns* — Zoom level

```cpp
void shake(float const magnitude_pixels, float const duration_seconds);
```
Trigger a screen shake effect.
- `magnitude_pixels` — Max shake offset in pixels
- `duration_seconds` — How long shake lasts

```cpp
void stop_shake();
```
Stop shake immediately.

```cpp
void set_bounds(sf::FloatRect const world_bounds);
```
Clamp camera so viewport stays within world bounds.
- `world_bounds` — Bounds rect

```cpp
void clear_bounds();
```
Disable bounds clamping.

```cpp
sf::FloatRect bounds() const;
```
Get current bounds.
- *returns* — Bounds rect

```cpp
void set_size(sf::Vector2f const size);
```
Set world units visible.
- `size` — View size

```cpp
sf::Vector2f size() const;
```
Get view size.
- *returns* — View size

```cpp
sf::View const& view() const;
```
Get the underlying sf::View.
- *returns* — Const reference to view

```cpp
sf::FloatRect visible_rect() const;
```
Get the currently visible world rect.
- *returns* — Visible rect

```cpp
sf::Vector2f window_to_world(sf::Vector2i const pixel, sf::Vector2u const window_size) const;
```
Convert window pixel to world position.
- `pixel` — Window pixel coordinate
- `window_size` — Window size
- *returns* — World position

```cpp
sf::Vector2i world_to_window(sf::Vector2f const world, sf::Vector2u const window_size) const;
```
Convert world position to window pixel.
- `world` — World position
- `window_size` — Window size
- *returns* — Window pixel

```cpp
void tick(float const dt);
```
Tick camera - advance smoothing and shake.
- `dt` — Delta time in seconds


### `class Debug_draw`

Immediate-mode debug drawing.

Draws lines, boxes, circles, text to the DEBUG render layer. All submissions are cleared each frame. Only active when enabled.

debug.line({0,0}, {100,100}, sf::Color::Red); debug.box(collider_rect, sf::Color::Green); debug.text({50,50}, "hp=42");

**Methods**

```cpp
void set_enabled(bool const flag);
```
Enable or disable debug drawing.
- `flag` — Enabled state

```cpp
bool is_enabled() const;
```
Check if debug drawing is enabled.
- *returns* — Enabled state

```cpp
void line(sf::Vector2f const a, sf::Vector2f const b, sf::Color const color);
```
Draw a line between two points.

```cpp
void box(sf::FloatRect const rect, sf::Color const color);
```
Draw a hollow rectangle.

```cpp
void filled_box(sf::FloatRect const rect, sf::Color const color);
```
Draw a filled rectangle.

```cpp
void circle(sf::Vector2f const center, float const radius, sf::Color const color, int const segments = 24);
```
Draw a hollow circle.
- `segments` — Number of line segments

```cpp
void cross(sf::Vector2f const center, float const radius, sf::Color const color);
```
Draw a cross/plus at a point.

```cpp
void arrow(sf::Vector2f const from, sf::Vector2f const to, sf::Color const color);
```
Draw an arrow from one point to another.

```cpp
void flush();
```
Submit all accumulated geometry to the renderer. Called by game loop.

```cpp
void clear();
```
Clear all accumulated geometry.


### `class Renderer`

Layered render frontend.

Game code submits drawables to layers. On end_frame() queues are sorted by sort_key and flushed in layer order with the appropriate camera applied.

Submitted drawables MUST remain alive until end_frame() completes.

**Methods**

```cpp
explicit Renderer(sf::RenderWindow& window);
```
Construct with a render window.
- `window` — SFML render window

```cpp
Camera& world_camera();
```
Get world camera (used by BACKGROUND..FOREGROUND layers).

```cpp
Camera const& world_camera() const;
```

```cpp
Camera& ui_camera();
```
Get UI camera (used by UI..DEBUG layers).

```cpp
Camera const& ui_camera() const;
```

```cpp
void set_layer_camera(Render_layer const layer, Camera* camera);
```
Override the camera for a specific layer.
- `layer` — Layer to override
- `camera` — Camera to use (nullptr = use default)

```cpp
Camera* layer_camera(Render_layer const layer);
```
Get the camera assigned to a layer.
- `layer` — Render layer
- *returns* — Camera pointer

```cpp
void begin_frame();
```
Clear window and submission queues. Call at start of frame.

```cpp
void end_frame();
```
Sort queues, draw all layers, present. Call at end of frame.

```cpp
void set_clear_color(sf::Color const color);
```
Set window clear color.
- `color` — Clear color

```cpp
sf::Color clear_color() const;
```
Get window clear color.
- *returns* — Clear color

```cpp
sf::RenderWindow& window();
```
Get the render window.
- *returns* — Reference to sf::RenderWindow

```cpp
std::size_t last_frame_draw_calls() const;
```
Get draw call count from last frame.
- *returns* — Draw call count



## Resources

### enum `Animation_mode`

Playback mode for an animation.

| Value | Meaning |
| --- | --- |
| `LOOP` | Loop back to start when last frame is reached |
| `ONE_SHOT` | Play once and stop on last frame |
| `HOLD` | Play once and hold on last frame indefinitely |
| `PING_PONG` | Play forward then backward repeatedly |

### `struct Animation`

A named animation sequence with configurable playback.


### `class Resource_manager`

Manages all SFML resources with reference counting and automatic unloading.

Resources are identified by string keys and loaded from file paths. Reference counting is handled via shared_ptr - when no outside handles remain the resource is queued for unloading at end of frame via flush_unused().

Usage:  auto tex = res.`load<sf::Texture>`("player", "assets/player.png"); auto sheet = res.`load<Spritesheet>`("player_sheet", "assets/player.png"); sheet->add_animation({...}); auto music = res.`load<sf::Music>`("battle", "assets/battle.ogg"); res.play_music("battle");

**Methods**

```cpp
static Resource_manager& instance();
```
Get singleton instance.
- *returns* — Reference to Resource_manager

```cpp
std::shared_ptr<T> get(std::string const& name);
```

```cpp
void unload(std::string const& name);
```
Manually unload a resource immediately.
- `name` — String key

```cpp
void flush_unused();
```
Unload all resources with no outside references. Call once per frame at the END of your game loop.

```cpp
void clear();
```
Unload all resources immediately.

```cpp
void play_music(std::string const& name, bool loop = true);
```
Play a loaded music track.
- `name` — String key
- `loop` — Whether to loop

```cpp
void stop_music(std::string const& name);
```
Stop a loaded music track.
- `name` — String key

```cpp
void pause_music(std::string const& name);
```
Pause a loaded music track.
- `name` — String key

```cpp
void resume_music(std::string const& name);
```
Resume a paused music track.
- `name` — String key

```cpp
void set_music_volume(std::string const& name, float volume);
```
Set volume of a music track.
- `name` — String key
- `volume` — Volume 0.0 - 100.0

```cpp
void stop_all_music();
```
Stop all currently playing music tracks.


### `class Spritesheet`

A texture with named animations and baked-in animator state.

**Methods**

```cpp
explicit Spritesheet(sf::Texture texture);
```
Construct from an existing texture.
- `texture` — Loaded SFML texture

```cpp
void add_animation(Animation const& animation);
```
Add a named animation.
- `animation` — Animation to add

```cpp
void set_animation(std::string const& name);
```
Set the current animation by name.
- `name` — Animation name

```cpp
void tick(float const dt);
```
Advance the animation by delta time.
- `dt` — Delta time in seconds

```cpp
void pause();
```
Pause playback.

```cpp
void resume();
```
Resume playback.

```cpp
void reset();
```
Reset current animation to first frame.

```cpp
bool is_finished() const;
```
Check if a one-shot animation has finished.
- *returns* — True if finished

```cpp
sf::IntRect current_frame() const;
```
Get the current frame rect.
- *returns* — Current sf::IntRect

```cpp
sf::Texture const& texture() const;
```
Get the underlying texture.
- *returns* — Reference to sf::Texture

```cpp
std::optional<Animation> get_animation(std::string const& name) const;
```
Get animation by name.
- `name` — Animation name
- *returns* — Optional animation

```cpp
std::string const& current_animation_name() const;
```
Get current animation name.
- *returns* — Current animation name

```cpp
int current_frame_index() const;
```
Get current frame index.
- *returns* — Current frame index



## Audio

### enum `Audio_bus`

Logical audio routing channels.

Volume and mute are set per-bus. Effective volume = MASTER * bus volume.

| Value | Meaning |
| --- | --- |
| `MASTER` | Scales all other buses |
| `MUSIC` | Background music tracks |
| `SFX` | Gameplay sound effects |
| `UI` | Interface sounds (button clicks, menu sounds) |
| `VOICE` | Dialogue and voiceover |
| `AMBIENT` | Environmental ambience |
| `COUNT` | Number of buses - not a valid bus |

### `class Audio_system`

Top-level audio facade.

Manages all audio buses and routes play requests to Sfx_player or Music_player. Game code calls play_sfx("explosion") and the system resolves the sound buffer from Resource_manager and submits it to the SFX pool.

Effective volume for any bus = MASTER volume * bus volume, clamped [0, 1]. Muted buses produce no sound regardless of volume settings.

**Methods**

```cpp
explicit Audio_system(resources::Resource_manager& rm);
```
Construct with resource manager.
- `rm` — Resource manager for resolving sound assets

```cpp
void set_bus_volume(Audio_bus const bus, float const volume);
```
Set raw volume for a bus.
- `bus` — Target bus
- `volume` — Volume [0, 1]

```cpp
float bus_volume(Audio_bus const bus) const;
```
Get raw volume for a bus (not scaled by master).
- `bus` — Target bus
- *returns* — Raw volume [0, 1]

```cpp
float effective_volume(Audio_bus const bus) const;
```
Get effective volume for a bus (master * bus, 0 if muted).
- `bus` — Target bus
- *returns* — Effective volume [0, 1]

```cpp
void set_bus_muted(Audio_bus const bus, bool const muted);
```
Set mute state for a bus.
- `bus` — Target bus
- `muted` — Muted state

```cpp
bool is_bus_muted(Audio_bus const bus) const;
```
Check if a bus is muted.
- `bus` — Target bus
- *returns* — True if muted

```cpp
void load_mix(core::Config const& config);
```
Load bus volumes and mute states from config.
- `config` — Config to read from

```cpp
void save_mix(core::Config& config) const;
```
Save bus volumes and mute states to config.
- `config` — Config to write to

```cpp
void stop_sfx(int const play_id);
```
Stop a specific SFX instance.
- `play_id` — ID returned from play_sfx()

```cpp
void stop_all_sfx();
```
Stop all currently playing SFX.

```cpp
Music_player& music();
```
Get the music player for direct control.
- *returns* — Reference to Music_player

```cpp
Music_player const& music() const;
```
Get the music player (const).
- *returns* — Const reference to Music_player

```cpp
void set_listener_position(sf::Vector2f const world_pos);
```
Set listener world position for spatial SFX attenuation.
- `world_pos` — Listener position in world space

```cpp
sf::Vector2f listener_position() const;
```
Get current listener world position.
- *returns* — Listener world position

```cpp
void update(float const dt);
```
Update SFX pool and music crossfades.
- `dt` — Delta time in seconds


### `class Music_player`

Streaming music player with crossfade support.

Manages two slots: an active track and an outgoing track fading out. Crossfade smoothly transitions between them. Direct play() replaces the current track immediately with no fade.

Music is loaded via Resource_manager using string keys. The resource manager handles the sf::Music lifetime and streaming from disk.

**Methods**

```cpp
explicit Music_player(resources::Resource_manager& rm);
```
Construct with resource manager.
- `rm` — Resource manager for resolving music assets

```cpp
void play(std::string const& resource_name, bool const loop = true);
```
Play a track immediately, replacing the current one without fade.
- `resource_name` — Name of sf::Music in Resource_manager
- `loop` — Whether to loop

```cpp
void stop();
```
Stop all playback immediately.

```cpp
void pause();
```
Pause playback.

```cpp
void resume();
```
Resume paused playback.

```cpp
bool is_playing() const;
```
Check if music is currently playing.
- *returns* — True if playing

```cpp
std::string const& current_track() const;
```
Get the resource name of the current active track.
- *returns* — Current track resource name, empty if none

```cpp
void update(float const dt, float const effective_volume);
```
Drive crossfade interpolation and apply bus volume.
- `dt` — Delta time in seconds
- `effective_volume` — Combined bus volume (master * music bus)


### `struct Sfx_options`

Options for playing a sound effect.


### `class Sfx_player`

Pooled sound effect player.

Maintains a fixed pool of sf::Sound instances. play() finds a free slot, applies options, and starts playback. If the pool is full, the oldest playing sound is evicted to make room.

Spatial attenuation is calculated manually in 2D against the listener position updated each frame by Audio_system.

**Methods**

```cpp
void stop(int const play_id);
```
Stop a specific sound instance.
- `play_id` — ID returned from play()

```cpp
void stop_all();
```
Stop all currently playing sounds.



## Core Services

### enum `Config_format`

Supported configuration file formats.

| Value | Meaning |
| --- | --- |
| `JSON` | JSON format (human-readable, structured) |
| `TOML` | TOML format (human-readable, modern INI) |
| `BINARY` | Binary format (compact, fast, non-human-readable) |

### enum `SFML_event_type`

SFML window and input event types with specific key/button granularity.

| Value | Meaning |
| --- | --- |
| `UNKNOWN` | Window events |
| `WINDOW_CLOSED` | — |
| `WINDOW_RESIZED` | — |
| `WINDOW_LOST_FOCUS` | — |
| `WINDOW_GAINED_FOCUS` | — |
| `WINDOW_TEXT_ENTERED` | Mouse movement |
| `MOUSE_MOVED` | — |
| `MOUSE_ENTER_WINDOW` | — |
| `MOUSE_LEAVE_WINDOW` | Mouse wheel |
| `MOUSE_WHEEL_UP` | — |
| `MOUSE_WHEEL_DOWN` | Mouse button pressed |
| `MOUSE_BUTTON_LEFT_PRESS` | — |
| `MOUSE_BUTTON_RIGHT_PRESS` | — |
| `MOUSE_BUTTON_MIDDLE_PRESS` | — |
| `MOUSE_BUTTON_EXTRA_1_PRESS` | — |
| `MOUSE_BUTTON_EXTRA_2_PRESS` | Mouse button held |
| `MOUSE_BUTTON_LEFT_HELD` | — |
| `MOUSE_BUTTON_RIGHT_HELD` | — |
| `MOUSE_BUTTON_MIDDLE_HELD` | — |
| `MOUSE_BUTTON_EXTRA_1_HELD` | — |
| `MOUSE_BUTTON_EXTRA_2_HELD` | Mouse button released |
| `MOUSE_BUTTON_LEFT_RELEASE` | — |
| `MOUSE_BUTTON_RIGHT_RELEASE` | — |
| `MOUSE_BUTTON_MIDDLE_RELEASE` | — |
| `MOUSE_BUTTON_EXTRA_1_RELEASE` | — |
| `MOUSE_BUTTON_EXTRA_2_RELEASE` | Joystick |
| `JOYSTICK_BUTTON_PRESSED` | — |
| `JOYSTICK_BUTTON_RELEASED` | — |
| `JOYSTICK_MOVED` | — |
| `JOYSTICK_CONNECTED` | — |
| `JOYSTICK_DISCONNECTED` | Touch |
| `TOUCH_BEGAN` | — |
| `TOUCH_MOVED` | — |
| `TOUCH_ENDED` | Sensor |
| `SENSOR_CHANGED` | Keyboard pressed (A-Z) |
| `KEYPRESS_Z` | Keyboard pressed (0-9) |
| `KEYPRESS_PAUSE` | Keyboard held (A-Z) |
| `KEYHELD_Z` | Keyboard held (0-9) |
| `KEYHELD_PAUSE` | Keyboard released (A-Z) |
| `KEYRELEASE_Z` | Keyboard released (0-9) |
| `KEYRELEASE_PAUSE` | — |

### `class Config`

Hierarchical configuration with sections and key-value pairs.

Supports nested sections for organizing related settings. Values can be primitives or strings.

**Methods**

```cpp
void set(std::string const& key, T const& value);
```

```cpp
void set(std::string const& section, std::string const& key, T const& value);
```

```cpp
std::optional<T> get(std::string const& key) const;
```

```cpp
std::optional<T> get(std::string const& section, std::string const& key) const;
```

```cpp
T get_or(std::string const& key, T const& default_value) const;
```

```cpp
T get_or(std::string const& section, std::string const& key, T const& default_value) const;
```

```cpp
bool has(std::string const& key) const;
```
Check if key exists in root section.
- `key` — Parameter name
- *returns* — True if key exists

```cpp
bool has(std::string const& section, std::string const& key) const;
```
Check if key exists in section.
- `section` — Section path
- `key` — Parameter name
- *returns* — True if key exists

```cpp
bool has_section(std::string const& section) const;
```
Check if section exists.
- `section` — Section path
- *returns* — True if section exists

```cpp
void remove(std::string const& key);
```
Remove key from root section.
- `key` — Parameter name

```cpp
void remove(std::string const& section, std::string const& key);
```
Remove key from section.
- `section` — Section path
- `key` — Parameter name

```cpp
void remove_section(std::string const& section);
```
Remove entire section.
- `section` — Section path

```cpp
std::vector<std::string> keys() const;
```
Get all keys in root section.
- *returns* — Vector of key names

```cpp
std::vector<std::string> keys(std::string const& section) const;
```
Get all keys in specific section.
- `section` — Section path
- *returns* — Vector of key names

```cpp
std::vector<std::string> sections() const;
```
Get all section names.
- *returns* — Vector of section paths

```cpp
void clear();
```
Clear all data.

```cpp
void merge(Config const& other);
```
Merge another config into this one.
- `other` — Config to merge (overwrites existing keys)


### `class Config_manager`

Singleton manager for named configuration objects.

Manages multiple configs (e.g., "settings", "keybinds", "save_data"). Handles loading/saving with multiple format support.

**Methods**

```cpp
static Config_manager& instance();
```
Get singleton instance.
- *returns* — Reference to Config_manager instance

```cpp
Config& get(std::string const& name);
```
Create or get a named config.
- `name` — Config identifier
- *returns* — Reference to config

```cpp
bool has(std::string const& name) const;
```
Check if named config exists.
- `name` — Config identifier
- *returns* — True if config exists

```cpp
void remove(std::string const& name);
```
Remove a named config.
- `name` — Config identifier

```cpp
void clear();
```
Clear all configs.


### `class Event_listener`

Base class for objects that listen to events.

Automatically generates unique listener IDs for event deregistration.

**Methods**

```cpp
std::uint64_t listener_id() const;
```
Get this listener's unique ID. _(protected)_
- *returns* — Listener ID


### `struct Game_event_data`

Data associated with custom game events.


### `class Game_event_manager`

Manages custom game events.

Allows emitting and listening to custom string-based events with any payload. Examples: "player_died", "enemy_spawned", "level_completed", etc.

**Methods**

```cpp
static Game_event_manager& instance();
```
Get singleton instance.
- *returns* — Reference to Game_event_manager

```cpp
void deregister_callback(std::uint64_t const callback_id);
```
Deregister specific callback.
- `callback_id` — ID returned from register_callback

```cpp
void deregister_listener(std::uint64_t const listener_id);
```
Deregister all callbacks for a listener.
- `listener_id` — Listener ID to remove

```cpp
void deregister_type(Game_event_type const& type);
```
Deregister all callbacks for an event type.
- `type` — Event type to clear

```cpp
void clear();
```
Clear all registered callbacks.


### `struct SFML_event_data`

Data associated with SFML events.


### `class SFML_event_manager`

Manages SFML window and input events with per-key/button granularity.

**Methods**

```cpp
static SFML_event_manager& instance();
```
Get singleton instance.
- *returns* — Reference to SFML_event_manager

```cpp
void process_events(sf::Window& window);
```
Process SFML events from window.
- `window` — SFML window to poll events from

```cpp
void process_events(sf::RenderWindow& window);
```
Process SFML events from render window.
- `window` — SFML render window to poll events from

```cpp
void deregister_callback(std::uint64_t const callback_id);
```
Deregister specific callback.
- `callback_id` — ID returned from register_callback

```cpp
void deregister_listener(std::uint64_t const listener_id);
```
Deregister all callbacks for a listener.
- `listener_id` — Listener ID to remove

```cpp
void deregister_type(SFML_event_type const type);
```
Deregister all callbacks for an event type.
- `type` — Event type to clear

```cpp
void clear();
```
Clear all registered callbacks.


### `class Time_manager`

Singleton manager for multiple named timers.

Allows independent timing for gameplay, UI, physics, particles, etc. Each timer can have different time scales and pause states.

**Methods**

```cpp
static Time_manager& instance();
```
Get singleton instance.
- *returns* — Reference to Time_manager instance

```cpp
Timer& get_timer(std::string_view const name);
```
Create or get a named timer.
- `name` — Timer identifier
- *returns* — Reference to timer

```cpp
std::optional<std::reference_wrapper<Timer>> try_get_timer(std::string_view const name);
```
Get timer if it exists.
- `name` — Timer identifier
- *returns* — Optional reference to timer

```cpp
bool has_timer(std::string_view const name) const;
```
Check if named timer exists.
- `name` — Timer identifier
- *returns* — True if timer exists

```cpp
void remove_timer(std::string_view const name);
```
Remove a named timer.
- `name` — Timer identifier

```cpp
void tick_all();
```
Update all timers.

```cpp
void pause_all();
```
Pause all timers.

```cpp
void resume_all();
```
Resume all timers.

```cpp
Timer& main();
```
Get the main default timer.
- *returns* — Reference to main timer


### `class Timer`

Individual timer tracking elapsed time, delta, and fixed-step accumulation.

Each timer maintains its own clock, time scale, frame count, and fixed-step accumulator. Supports pause/resume and time scaling for slow-motion or fast-forward effects.

**Methods**

```cpp
void tick();
```
Update timer for current frame.

```cpp
void reset();
```
Reset timer to initial state.

```cpp
void pause();
```
Pause the timer (delta becomes 0 until resumed).

```cpp
void resume();
```
Resume a paused timer.

```cpp
bool is_paused() const;
```
Check if timer is currently paused.
- *returns* — True if paused

```cpp
float delta_time() const;
```
Get frame delta time with scaling and clamping applied.
- *returns* — Delta time in seconds

```cpp
float unscaled_delta_time() const;
```
Get raw frame delta without scaling or clamping.
- *returns* — Unscaled delta time in seconds

```cpp
double total_time() const;
```
Get total elapsed time with scaling applied.
- *returns* — Total time in seconds

```cpp
double unscaled_total_time() const;
```
Get total elapsed time without scaling.
- *returns* — Unscaled total time in seconds

```cpp
std::uint64_t frame_count() const;
```
Get number of frames processed by this timer.
- *returns* — Frame count

```cpp
void set_time_scale(float const scale);
```
Set time scale multiplier.
- `scale` — Time scale (0=paused, 0.5=half speed, 1=normal, 2=double speed)

```cpp
float time_scale() const;
```
Get current time scale.
- *returns* — Time scale multiplier

```cpp
void set_max_delta(float const seconds);
```
Set maximum delta clamp to prevent huge time spikes.
- `seconds` — Maximum delta in seconds

```cpp
float max_delta() const;
```
Get maximum delta clamp value.
- *returns* — Max delta in seconds

```cpp
void set_fixed_dt(float const seconds);
```
Set fixed timestep for physics updates.
- `seconds` — Fixed step in seconds (e.g., 1/60 for 60Hz physics)

```cpp
float fixed_dt() const;
```
Get fixed timestep value.
- *returns* — Fixed delta in seconds

```cpp
bool consume_fixed_step();
```
Consume one fixed timestep from accumulator if available.
- *returns* — True if a fixed step was consumed, false if accumulator empty



## Utilities

### enum `Log_lvl`

Logging severity levels.

Each level is a bit flag, allowing bitwise combinations for filtering. Levels are ordered from most verbose (TRACE) to most critical (ERROR).

| Value | Meaning |
| --- | --- |
| `TRACE` | Detailed trace information for debugging |
| `DEBUG` | Debug-level messages for development |
| `INFO` | Informational messages about program flow |
| `WARN` | Warning messages for potentially problematic situations |
| `ERR` | Error messages for failures and exceptions |

### `class Logger`

Thread-safe logger with source location tracking and colored output.

Logger uses RAII to buffer messages and flush them atomically on destruction. Each log message includes timestamp, severity, file location, and supports stream-style formatting via operator<<.


### `class Random`

High-quality random number generator using xoshiro256++ algorithm.

Provides cryptographically-strong pseudo-random number generation with excellent statistical properties. Passes all BigCrush and PractRand tests. Much faster and higher quality than std::mt19937.

Supports deterministic seeding for reproducible sequences, making it suitable for procedural generation, simulations, and gameplay.

**Methods**

```cpp
explicit Random(std::uint64_t const seed);
```
Construct with explicit seed for reproducible sequences.
- `seed` — Initial seed value

```cpp
void seed(std::uint64_t const s);
```
Reseed the generator.
- `s` — New seed value

```cpp
std::uint64_t current_seed() const;
```
Get the current seed value.
- *returns* — Current seed

```cpp
int range(int const min, int const max);
```
Generate random integer in range [min, max] (inclusive).
- `min` — Minimum value (inclusive)
- `max` — Maximum value (inclusive)
- *returns* — Random integer in range

```cpp
long range(long const min, long const max);
```
Generate random long integer in range [min, max] (inclusive).
- `min` — Minimum value (inclusive)
- `max` — Maximum value (inclusive)
- *returns* — Random long in range

```cpp
float range(float const min, float const max);
```
Generate random float in range [min, max).
- `min` — Minimum value (inclusive)
- `max` — Maximum value (exclusive)
- *returns* — Random float in range

```cpp
double range(double const min, double const max);
```
Generate random double in range [min, max).
- `min` — Minimum value (inclusive)
- `max` — Maximum value (exclusive)
- *returns* — Random double in range

```cpp
bool chance(float const probability);
```
Return true with given probability.
- `probability` — Probability of returning true [0.0, 1.0]
- *returns* — True with given probability, false otherwise

```cpp
sf::Vector2f in_unit_circle();
```
Generate random point inside unit circle.
- *returns* — Point uniformly distributed inside circle of radius 1

```cpp
sf::Vector2f in_circle(float const radius);
```
Generate random point inside circle of given radius.
- `radius` — Radius of circle
- *returns* — Point uniformly distributed inside circle

```cpp
sf::Vector2f on_unit_circle();
```
Generate random point on unit circle perimeter.
- *returns* — Point on circle of radius 1

```cpp
sf::Vector2f in_rect(sf::FloatRect const rect);
```
Generate random point inside rectangle.
- `rect` — Rectangle bounds
- *returns* — Point uniformly distributed inside rectangle

```cpp
static Random& global();
```
Get global shared Random instance.
- *returns* — Reference to global Random instance

```cpp
T& pick(std::span<T> items);
```

```cpp
void shuffle(std::span<T> items);
```



## Debug

### `class Debug_overlay`

On-screen developer overlay: FPS / frame-time readout, a frame-time graph, profiler scope timings, and arbitrary user "watch" lines.

Pulls numbers from a Profiler and draws them on the renderer's UI_OVERLAY layer with a font loaded from the Resource_manager. Toggle with set_visible() (e.g. bound to F3). Keepalive drawables are held internally to satisfy the renderer's outlive-end_frame() contract.

```cpp
Debug_overlay overlay;
overlay.set_font("default_font");
// each frame, after profiler.end_frame():
overlay.set_watch("objects", std::to_string(world.count()));
overlay.render(renderer, profiler);
```

**Methods**

```cpp
void set_font(std::string font_resource);
```
Set the font resource used for text.
- `font_resource` — Font key in the Resource_manager

```cpp
void set_visible(bool visible);
```
Show or hide the overlay.
- `visible` — True to show

```cpp
bool is_visible() const;
```
Check whether the overlay is visible.
- *returns* — True if visible

```cpp
void toggle();
```
Toggle overlay visibility.

```cpp
void set_position(sf::Vector2f pos);
```
Set the overlay's top-left position.
- `pos` — Position in pixels

```cpp
void set_text_color(sf::Color color);
```
Set the text color.
- `color` — Text color

```cpp
void set_background(sf::Color color);
```
Set the background panel color (alpha supported).
- `color` — Background color

```cpp
void set_show_scopes(bool show);
```
Show or hide profiler scope timings in the overlay.
- `show` — True to list scope timings

```cpp
void set_watch(std::string const& key, std::string const& value);
```
Set or update a custom watch line.
- `key` — Watch label
- `value` — Watch value

```cpp
void clear_watches();
```
Remove all custom watch lines.

```cpp
void render(render::Renderer& renderer, Profiler const& profiler);
```
Draw the overlay. No-op when hidden.
- `renderer` — Renderer to submit to
- `profiler` — Profiler to read stats from


### `class Profile_scope`

RAII scope timer: times from construction to destruction.


### `class Profiler`

Lightweight per-frame timing: a rolling FPS/frame-time average plus named scope timers for measuring sections of a frame.

Usage each frame:

```cpp
profiler.begin_frame();
{ TITAN_PROFILE_SCOPE(profiler, "physics"); world.fixed_update(dt); }
{ TITAN_PROFILE_SCOPE(profiler, "render");  world.render(r); }
profiler.end_frame();
```

Then read fps(), average_frame_ms(), or scope_ms("physics") for display in a Debug_overlay or logs. A singleton accessor is provided for convenience so scope macros work without threading a reference everywhere.

**Methods**

```cpp
static Profiler& instance();
```
Get the shared profiler instance.
- *returns* — Reference to the singleton profiler

```cpp
void begin_frame();
```
Mark the start of a frame.

```cpp
void end_frame();
```
Mark the end of a frame and update timing stats.

```cpp
float fps() const;
```
Get the smoothed frames per second.
- *returns* — Frames per second

```cpp
float average_frame_ms() const;
```
Get the smoothed frame time.
- *returns* — Average frame time in milliseconds

```cpp
float last_frame_ms() const;
```
Get the most recent frame time.
- *returns* — Last frame time in milliseconds

```cpp
std::uint64_t frame_index() const;
```
Get the number of completed frames.
- *returns* — Frame index

```cpp
void begin_scope(std::string const& name);
```
Begin timing a named scope.
- `name` — Scope name

```cpp
void end_scope(std::string const& name);
```
End timing a named scope.
- `name` — Scope name

```cpp
float scope_ms(std::string const& name) const;
```
Get the last measured time for a scope.
- `name` — Scope name
- *returns* — Last duration in milliseconds (0 if unknown)

```cpp
std::vector<std::pair<std::string, float>> scopes() const;
```
Get all current scope timings.
- *returns* — Vector of (name, last milliseconds) pairs

```cpp
void set_smoothing(float factor);
```
Set the smoothing factor for averaged timings.
- `factor` — EMA factor in (0,1]; 1 = no smoothing

```cpp
void set_enabled(bool enabled);
```
Enable or disable the profiler.
- `enabled` — True to enable timing

```cpp
bool is_enabled() const;
```
Check whether the profiler is enabled.
- *returns* — True if enabled

```cpp
void reset();
```
Reset all timings and counters.

