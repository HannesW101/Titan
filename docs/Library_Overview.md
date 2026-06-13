# Titan Engine — Library Overview

The complete API reference for Titan, generated from the headers.

## Complete API Reference

Every public class, method, and enum in Titan, grouped by module and generated directly from the headers. Each entry shows its signature, what it does, its parameters, and its return value.

### Core Application

#### class Application : public events::Event_listener

The engine's runtime shell. Owns the window and every core subsystem (renderer, resources, audio, the frame timer) and runs the main loop: while running: poll events (SFML_event_manager) accumulate dt while accumulator >= fixed_dt: fixed_update(fixed_dt) // gameplay/physics update(dt) // per-frame logic, animations, UI render(renderer) // submit drawables Game code does not subclass Application. Instead it installs callbacks (set_update / set_fixed_update / set_render) or, more commonly, hands control to a Scene_manager (next module) whose hooks Application calls. This keeps the shell decoupled from any particular game structure. Lifetime: construct with an Application_config, optionally wire callbacks, then call run(). run() returns when the window closes or quit() is called.

#### struct Application_config

Settings that define how the Application creates its window and runs its main loop. Construct one, tweak fields, and pass to Application. Can be loaded from / saved to a titan::core::Config so the window size, fullscreen flag, vsync, and frame caps live in the user's settings file.

### Scenes

#### enum Op

#### struct Pending

A queued transition operation.

#### class Scene

Base class for a game screen: main menu, gameplay level, pause overlay, settings, etc. A Scene owns whatever it needs (UI, game world, scene-tier resources) and exposes lifecycle hooks the Scene_manager drives. Lifecycle: on_enter() once, when the scene is first pushed/activated on_resume() when it becomes the top scene again (e.g. overlay closed) on_pause() when another scene is pushed on top of it fixed_update(dt) zero+ times per frame, fixed timestep (gameplay/physics) update(dt) once per frame (animations, UI, input-driven logic) render(renderer) once per frame on_exit() once, right before the scene is destroyed/popped Transparency flags let a pushed scene (e.g. a pause menu) request that the scene beneath it keep rendering and/or updating, enabling overlays. Scenes get access to the owning Application (window, renderer, resources, audio) and the Scene_manager (to request transitions) once attached.

#### class Scene_manager

Owns a stack of Scenes and drives their lifecycle. The top scene is "active". Scenes below it are paused; an overlay scene can opt to let those below keep rendering/updating (see Scene::set_renders_below). Transition operations (push / pop / replace / clear) are **deferred**: they are recorded when requested and applied at a safe point (start of the next update), so a scene can safely request "replace me" from inside its own update() without being destroyed mid-call. On any scene destruction the manager calls Resource_manager

#### class Settings_scene : public Scene

A ready-made settings overlay: volume sliders bound to the audio buses (Master / Music / SFX / UI), live-updating the mix and persisting it to a named Config on close. Demonstrates the full UI integration path: Scene + UI_system + Audio_system + Config working together. Push it over gameplay as an overlay (it renders the scene below but freezes its updates): scenes().push(std::make_unique<Settings_scene>("settings")); On "Back", it saves the audio mix to the config (via Audio_system::save_mix) and pops itself.

### Game Objects & Components

#### enum Collider_shape

The geometric shape a Collider tests with.

| Value | Meaning |
| --- | --- |
| `AABB` | Axis-aligned box, sized by half_extents |
| `CIRCLE` | Circle, sized by radius |

#### class Animator : public Game_component

Plays named animations from a Spritesheet and feeds the current frame rect into the owner's Sprite_renderer each frame. Pairs with a Sprite_renderer on the same object: the Animator owns timing and frame selection, the Sprite_renderer owns drawing. The Spritesheet resource is loaded by name from the Resource_manager. auto* sr = obj->add_component<Sprite_renderer>(); sr->set_texture("hero_sheet"); auto* anim = obj->add_component<Animator>(); anim->set_spritesheet("hero_sheet"); anim->add_animation({ "run", { rect0, rect1, rect2 }, 12.f }); anim->play("run");

#### class Audio_emitter : public Game_component

Plays sound effects positioned at the owner's world location. Components don't own the Audio_system (the Application does), so the scene wires a reference in via set_audio_system(). play() then emits the named SFX with spatial attenuation centered on the owner's transform, so distant emitters are quieter. auto* em = obj->add_component<Audio_emitter>(); em->set_audio_system(app.audio()); em->set_falloff_radius(600.f); em->play("footstep");

#### class Collider : public Game_component

A collision shape attached to a Game_object, positioned at the owner's world position plus a local offset. Two shapes are supported: AABB (set via half-extents) and circle (radius). The Collision_system tests every enabled collider pair each fixed step and fires on_enter / on_stay / on_exit callbacks. A collider may be a trigger (overlap reported but no implied physical response). auto* c = obj->add_component<Collider>(); c->set_box({ 16.f, 16.f }); // 32x32 box c->set_on_enter([](Collider& self, Collider& other){ ... });

#### class Collision_system

Detects overlaps between all active Colliders in a World and fires enter / stay / exit callbacks. Call update(world) once per fixed step (after movement). It gathers every enabled Collider on active, non-destroyed objects, tests each unique pair, and tracks which pairs were touching last step so it can distinguish enter (new), stay (still), and exit (no longer) and fire the matching callbacks on both colliders. collision_system.update(world); // in Scene::fixed_update after world step The current broad test is O(n^2) pairwise, which is fine for the hundreds of colliders a typical 2D game has; a spatial grid can replace _broad_phase later without changing the callback contract.

#### class Game_component

Base class for all behavior/data attached to a Game_object. Components are the unit of composition: a Game_object is little more than a named bag of components plus a Transform. Each component gets lifecycle hooks mirroring the engine loop and a back-pointer to its owner. Lifecycle: on_attach() once, when added to an object that's in the world on_start() once, on the first frame the object is active fixed_update(dt) zero+ times/frame (physics, gameplay) update(dt) once/frame (animation, input-driven logic) render(renderer) once/frame (visual components) on_detach() once, when removed or the object is destroyed Components reach siblings through owner(): e.g. a Sprite_renderer reads the Transform via owner().transform().

#### class Game_object

An entity in the game World: a name, a unique id, a built-in Transform, and a set of components that give it behavior and appearance. Composition over inheritance: instead of subclassing per entity type, you assemble objects from components: auto* enemy = world.spawn("enemy"); enemy->transform().set_position(100.f, 50.f); enemy->add_component<Sprite_renderer>()->set_texture("orc"); enemy->add_component<Health>(30); enemy->add_component<Script>()->set_update_fn(...); Typed access: add_component<T>(args...) constructs and returns T*; get_component<T>() returns the first T* or nullptr; has_component<T>() tests. Lifecycle is driven by World, which forwards fixed_update/update/render to all enabled components and fires on_start() the first active frame. Destruction is deferred: destroy() marks the object; World removes it at a safe point so a component can destroy its own object mid-update.

#### class Health : public Game_component

Hit points with damage / heal helpers and death handling. Tracks current and max HP, fires callbacks on damage, heal, and death, and (by default) destroys the owning object when HP reaches zero. The death callback runs before destruction so you can spawn loot, play an animation, or cancel the auto-destroy by calling set_destroy_on_death(false). auto* hp = obj->add_component<Health>(100); hp->set_on_death([](Game_object& self){ spawn_explosion(self); }); hp->damage(30);

#### class Rigidbody : public Game_component

Simple kinematic motion: velocity, acceleration, forces, drag and gravity, integrated into the owner's Transform each fixed step. This is deliberately lightweight (not a full physics engine). It moves the Transform by integrating velocity semi-implicitly, which is stable for top-down movement, platformer-ish gravity, projectiles, knockback, etc. Pair with a Collider + Collision_system for response logic in callbacks. auto* rb = obj->add_component<Rigidbody>(); rb->set_gravity({ 0.f, 980.f }); rb->add_force({ 200.f, 0.f }); // one-shot impulse-ish push rb->set_velocity({ 50.f, 0.f });

#### class Script : public Game_component

A component that runs user-provided callbacks for the lifecycle hooks, so simple behaviors don't need a whole new component subclass. auto* s = obj->add_component<Script>(); s->set_start_fn ([](Game_object& self){ ... }); s->set_update_fn([](Game_object& self, float dt){ self.transform().move({ 10.f * dt, 0.f }); }); Each callback receives the owning Game_object by reference, so behavior can reach the transform, siblings, and the world.

#### class Sprite_renderer : public Game_component

Draws a textured sprite at the owner's world transform. Pulls the texture from the Resource_manager by name, builds an sf::Sprite each frame positioned/rotated/scaled by the owner's Transform, and submits it to the renderer on a chosen layer (default ENTITIES). A keepalive sprite is stored so it outlives end_frame() per the renderer's contract. auto* sr = obj->add_component<Sprite_renderer>(); sr->set_texture("player"); sr->set_origin_centered(true);

#### class Transform : public Game_component

Position / rotation / scale of a Game_object in world space, with optional parenting for hierarchical transforms. Every Game_object owns a Transform implicitly (created in its constructor), so you rarely add one manually. Other components read it via owner().transform(). Local vs world: if the object has a transform parent, local_* values are relative to the parent and world_* compose up the chain. With no parent, local and world are identical.

#### class World

Container and driver for all Game_objects in a scene. Spawns objects (assigning unique ids), forwards the engine loop to them, and reaps destroyed objects at a safe point so components can destroy objects (including their own) mid-update without invalidating iteration. A Scene typically owns one World:

### User Interface

#### enum Dir

#### enum Kind

| Value | Meaning |
| --- | --- |
| `NONE` | Draw nothing |
| `COLOR` | Solid fill + optional outline |
| `TEXTURE` | Stretched/fitted texture |
| `NINE_SLICE` | Nine-slice stretchable texture |

#### enum Layout_mode

#### enum Orientation

#### enum Orientation

#### enum Orientation

#### enum Text_align

#### enum Type

| Value | Meaning |
| --- | --- |
| `PIXELS` | — |
| `PERCENT` | — |

#### enum UI_anchor

Nine-point anchor for positioning widgets relative to their parent. The anchor defines which point of the parent the widget's offset is measured from.

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

#### enum Widget_state

Interaction state of a widget. Drives which Visual is shown and which text color is used.

| Value | Meaning |
| --- | --- |
| `NORMAL` | — |
| `HOVERED` | — |
| `PRESSED` | — |
| `FOCUSED` | — |
| `DISABLED` | — |
| `COUNT` | — |

#### class Button : public Widget

Clickable push button with text and per-state visuals.

#### class Checkbox : public Widget

Boolean checkbox with a label drawn to the right of the box.

#### class Context_menu : public Widget

A popup list of clickable entries shown at a position.

#### class Draw_buffer

Per-widget persistent storage for drawables submitted each frame. The Renderer requires submitted drawables to remain alive until end_frame(). Widgets build transient shapes/text each frame; this buffer owns them for the duration of the frame so the references the Renderer holds stay valid. Call clear() at the start of each render, then use the add_* helpers which store the drawable and submit a stable reference to the Renderer. std::deque is used because it does not invalidate references on push_back, unlike std::vector - critical since the Renderer holds pointers into here.

#### class Dropdown : public Widget

Drop-down selector. Expands an option list on click (UI_OVERLAY).

#### class Focus_manager : public events::Event_listener

Keyboard focus traversal for a UI_manager's widget tree. Collects focusable widgets (Widget

#### class Gauge : public Widget

Radial/arc meter for cooldowns, speed, health rings, etc. Value in [0,1]; sweeps from start_angle clockwise by up to sweep degrees.

#### class Grid : public Widget

Container that arranges children in a grid (set columns).

#### class H_box : public Widget

Container that arranges children left-to-right.

#### class Icon_button : public Widget

Button showing a texture icon centered in its rect.

#### class Image : public Widget

Displays a texture (optionally a sub-rect), with tint + aspect.

#### class Item_grid : public Widget

A grid of Item_slots with drag-drop between them built in.

#### class Item_slot : public Widget

An inventory slot holding an item id (>=0 = filled, -1 = empty), shown via an icon texture. Supports drag-out and drop-in using the UI_manager drag payload (carries the item id, drag_kind = "item").

#### class Label : public Widget

A non-interactive text display with alignment and optional wrap.

#### class List_box : public Widget

Scrollable selectable list of text items (single or multi-select).

#### class Menu_bar : public Widget

Horizontal menu bar with top-level entries. Each entry fires a callback. For nested submenus combine with a Context_menu.

#### class Minimap : public Widget

A minimap that maps world coordinates into its rect and draws blips, plus an optional viewport rectangle. Set the world bounds, then push blips each frame. Click maps back to world via on_clicked(world_pos).

#### class Modal : public Widget

Dimmed-backdrop dialog. show()/hide(); click backdrop closes if set.

#### class Nine_slice_panel : public Widget

A panel whose background is a nine-slice texture.

#### struct Node

#### class Number_field : public Widget

Numeric text entry with min/max clamping and decimals. Accepts only digits, '-', and '.'. Commits on Enter or focus loss.

#### class Panel : public Widget

Styled container. Use layout modes for auto-arrangement.

#### class Progress_bar : public Widget

Animated fill bar [0,1], horizontal or vertical, optional % label.

#### class Radial_menu : public Widget

Pie/wheel selection menu. show_at() opens it centered at a point; the hovered wedge highlights; release selects.

#### class Radio_button : public Widget

One option in a Radio_group.

#### class Radio_group

Manages mutual exclusion across a set of Radio_buttons.

#### class Resizable_window : public Widget

Draggable + resizable titled window. Drag title to move, drag the bottom-right corner handle to resize.

#### class Rich_text : public Widget

Text composed of styled spans (per-span color + size), laid out left-to-right with wrapping. Build with add_span(); good for log windows, dialogue with colored names, damage numbers, etc.

#### class Scroll_view : public Widget

Scrollable viewport. Set content size; children are offset by scroll. Mouse wheel scrolls while hovered. Draws a thumb on the active axis.

#### class Segmented_control : public Widget

Horizontal row of mutually-exclusive segments (like iOS segmented control).

#### class Separator : public Widget

Thin horizontal/vertical divider line.

#### class Slider : public Widget

Horizontal/vertical value slider with draggable knob.

#### class Spacer : public Widget

Invisible fixed-size gap for use inside layout containers.

#### class Spin_box : public Widget

Numeric spin edit: [-] [value] [+], all skinnable. The two buttons and the value label are internal child widgets exposed via minus_button()/plus_button()/value_field() so you can restyle them (e.g. give the buttons textures) or reposition them relative to each other.

#### class Tab_view : public Widget

Tabbed container; one child shown at a time, tab strip on top.

#### class Table : public Widget

Simple data table: column headers + rows of cell strings.

#### struct Text_appearance

How text is drawn for a widget, per state where desired.

#### class Text_area : public Widget

Multi-line editable text box with wrapping and wheel scrolling.

#### class Text_field : public Widget

Single-line editable text input. Click to focus, type to edit, Enter to submit, Backspace to delete. Supports placeholder + password.

#### class Theme

Holds default Visual_sets keyed by widget style name ("button", "label", "panel", "slider_track", ...). Widgets look up their defaults via style_key(); users override per-widget on top. Themes can be built in code or loaded from a TOML config (sections named "ui.<key>") using your Config system. A built-in dark theme is provided via Theme

#### class Toast : public Widget

Brief notification that auto-fades out.

#### class Toggle : public Widget

Animated on/off switch with a sliding knob and optional label.

#### class Tooltip : public Widget

Hover popup that fades in after a delay and follows the mouse.

#### class Tree_view : public Widget

Expandable tree of labeled nodes.

#### struct UI_length

A length value that can be either absolute pixels or a percentage of the parent's corresponding dimension.

#### class UI_manager

Owns the widget tree, the active Theme, focus and hover tracking, and drives the per-frame layout/update/render passes.

#### class UI_system

Bundles a UI_manager + Focus_manager and wires them into the engine loop, so a Scene gets working UI with one object. It pulls the renderer + resource manager from the Application, syncs the UI mouse position from the window each frame, runs layout/update, and renders on the UI layers. Keyboard focus traversal (Tab) is handled by the embedded Focus_manager.

#### class V_box : public Widget

Container that arranges children top-to-bottom.

#### class Virtual_list : public Widget

Virtualized list: only visible rows are built each frame, so it handles millions of items. Provide a count and a row-text provider.

#### struct Visual

A single drawable appearance for a widget background. A Visual can be: nothing, a solid/outlined color rect, a stretched texture, or a nine-slice texture. This is the single flexible primitive every widget uses to draw itself - no widget hardcodes its background anymore.

#### struct Visual_set

A complete appearance for a widget across all interaction states. Holds one Visual per state (NORMAL is the fallback for any unset state) and a text appearance per state. This is what makes every widget fully skinnable without touching widget code: assign Visuals for each state and you are done. Any state left unset falls back to NORMAL, so the common case (only setting NORMAL + HOVERED) just works.

#### class Widget : public events::Event_listener

Base class for all named UI widgets. Provides the shared machinery every widget needs: tree structure, layout (anchor + offset, px/% sizing, optional auto-arrange of children), per-state appearance via a Visual_set, visibility/enable/hover/focus/press state, and automatic registration/cleanup with the SFML event manager. Concrete widgets (Button, Slider, ...) derive from this, add their own typed API (plain setter methods, no chaining), and override the protected hooks on_update() / on_render() / on_layout(). They draw through draw_buffer() so submitted drawables stay alive until the renderer flushes. Styling: a widget pulls its default Visual_set from the active Theme using its style_key() (set by each subclass), then applies any per-widget overrides the user supplied via set_visual()/set_text_style().

#### class Window : public Widget

Draggable titled window with a title bar and optional close button. Children placed inside appear below the title bar (use inner_rect).

### Rendering

#### enum Render_layer

Drawing order layers. Lower values draw first (further back). Within a layer, items sort by sort_key (lower = further back). BACKGROUND..FOREGROUND use the world camera. UI..DEBUG use the UI camera.

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

#### class Camera

2D camera wrapping sf::View. Supports target following with smoothing, screen shake, zoom interpolation, and world bounds clamping. Call tick(dt) each frame.

#### struct Command

Sort and flush a single layer to the window.

#### class Debug_draw

Immediate-mode debug drawing. Draws lines, boxes, circles, text to the DEBUG render layer. All submissions are cleared each frame. Only active when enabled.

#### class Renderer

Layered render frontend. Game code submits drawables to layers. On end_frame() queues are sorted by sort_key and flushed in layer order with the appropriate camera applied. Submitted drawables MUST remain alive until end_frame() completes.

### Resources

#### enum Animation_mode

Playback mode for an animation.

| Value | Meaning |
| --- | --- |
| `LOOP` | Loop back to start when last frame is reached |
| `ONE_SHOT` | Play once and stop on last frame |
| `HOLD` | Play once and hold on last frame indefinitely |
| `PING_PONG` | Play forward then backward repeatedly |

#### struct Animation

A named animation sequence with configurable playback.

#### class Resource_manager

Manages all SFML resources with reference counting and automatic unloading. Resources are identified by string keys and loaded from file paths. Reference counting is handled via shared_ptr - when no outside handles remain the resource is queued for unloading at end of frame via flush_unused(). Usage:

#### class Spritesheet

A texture with named animations and baked-in animator state.

### Audio

#### enum Audio_bus

Logical audio routing channels. Volume and mute are set per-bus. Effective volume = MASTER * bus volume.

| Value | Meaning |
| --- | --- |
| `MASTER` | Scales all other buses |
| `MUSIC` | Background music tracks |
| `SFX` | Gameplay sound effects |
| `UI` | Interface sounds (button clicks, menu sounds) |
| `VOICE` | Dialogue and voiceover |
| `AMBIENT` | Environmental ambience |
| `COUNT` | Number of buses - not a valid bus |

#### class Audio_system

Top-level audio facade. Manages all audio buses and routes play requests to Sfx_player or Music_player. Game code calls play_sfx("explosion") and the system resolves the sound buffer from Resource_manager and submits it to the SFX pool. Effective volume for any bus = MASTER volume * bus volume, clamped [0, 1]. Muted buses produce no sound regardless of volume settings.

#### class Music_player

Streaming music player with crossfade support. Manages two slots: an active track and an outgoing track fading out. Crossfade smoothly transitions between them. Direct play() replaces the current track immediately with no fade. Music is loaded via Resource_manager using string keys. The resource manager handles the sf::Music lifetime and streaming from disk.

#### struct Sfx_options

Options for playing a sound effect.

#### class Sfx_player

Pooled sound effect player. Maintains a fixed pool of sf::Sound instances. play() finds a free slot, applies options, and starts playback. If the pool is full, the oldest playing sound is evicted to make room. Spatial attenuation is calculated manually in 2D against the listener position updated each frame by Audio_system.

### Core Services

#### enum Config_format

Supported configuration file formats.

| Value | Meaning |
| --- | --- |
| `JSON` | JSON format (human-readable, structured) |
| `TOML` | TOML format (human-readable, modern INI) |
| `BINARY` | Binary format (compact, fast, non-human-readable) |

#### enum SFML_event_type

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

#### class Config

Hierarchical configuration with sections and key-value pairs. Supports nested sections for organizing related settings. Values can be primitives or strings.

#### class Config_manager

Singleton manager for named configuration objects. Manages multiple configs (e.g., "settings", "keybinds", "save_data"). Handles loading/saving with multiple format support.

#### class Event_listener

Base class for objects that listen to events. Automatically generates unique listener IDs for event deregistration.

#### struct Game_event_data

Data associated with custom game events.

#### class Game_event_manager

Manages custom game events. Allows emitting and listening to custom string-based events with any payload. Examples: "player_died", "enemy_spawned", "level_completed", etc.

#### struct SFML_event_data

Data associated with SFML events.

#### class SFML_event_manager

Manages SFML window and input events with per-key/button granularity.

#### class Time_manager

Singleton manager for multiple named timers. Allows independent timing for gameplay, UI, physics, particles, etc. Each timer can have different time scales and pause states.

#### class Timer

Individual timer tracking elapsed time, delta, and fixed-step accumulation. Each timer maintains its own clock, time scale, frame count, and fixed-step accumulator. Supports pause/resume and time scaling for slow-motion or fast-forward effects.

### Utilities

#### enum Log_lvl

Logging severity levels. Each level is a bit flag, allowing bitwise combinations for filtering. Levels are ordered from most verbose (TRACE) to most critical (ERROR).

| Value | Meaning |
| --- | --- |
| `TRACE` | Detailed trace information for debugging |
| `DEBUG` | Debug-level messages for development |
| `INFO` | Informational messages about program flow |
| `WARN` | Warning messages for potentially problematic situations |
| `ERR` | Error messages for failures and exceptions |

#### class Logger

Thread-safe logger with source location tracking and colored output. Logger uses RAII to buffer messages and flush them atomically on destruction. Each log message includes timestamp, severity, file location, and supports stream-style formatting via operator<<.

#### class Random

High-quality random number generator using xoshiro256++ algorithm. Provides cryptographically-strong pseudo-random number generation with excellent statistical properties. Passes all BigCrush and PractRand tests. Much faster and higher quality than std::mt19937. Supports deterministic seeding for reproducible sequences, making it suitable for procedural generation, simulations, and gameplay.

### Debug

#### class Debug_overlay

On-screen developer overlay: FPS / frame-time readout, a frame-time graph, profiler scope timings, and arbitrary user "watch" lines. Pulls numbers from a Profiler and draws them on the renderer's UI_OVERLAY layer with a font loaded from the Resource_manager. Toggle with set_visible() (e.g. bound to F3). Keepalive drawables are held internally to satisfy the renderer's outlive-end_frame() contract. Debug_overlay overlay; overlay.set_font("default_font"); // each frame, after profiler.end_frame(): overlay.set_watch("objects", std

#### class Profile_scope

RAII scope timer: times from construction to destruction.

#### class Profiler

Lightweight per-frame timing: a rolling FPS/frame-time average plus named scope timers for measuring sections of a frame. Usage each frame: profiler.begin_frame(); { TITAN_PROFILE_SCOPE(profiler, "physics"); world.fixed_update(dt); } { TITAN_PROFILE_SCOPE(profiler, "render"); world.render(r); } profiler.end_frame(); Then read fps(), average_frame_ms(), or scope_ms("physics") for display in a Debug_overlay or logs. A singleton accessor is provided for convenience so scope macros work without threading a reference everywhere.

Titan Engine Manual · generated for v1.0 · C++20 · SFML 3

