# Titan Engine — Read Me

A practical, example-driven guide to building a 2D game with Titan. Every snippet
uses the engine's real API. Code is C++20 and uses SFML 3 types directly where
the engine exposes them.

> **Conventions:** classes are `Capitalised`, members `_with_underscore`,
> functions `snake_case`. Everything lives under the `titan::` namespace
> (`titan::app`, `titan::scene`, `titan::game`, `titan::ui`, `titan::render`,
> `titan::resources`, `titan::audio`, `titan::core`, `titan::utils`,
> `titan::debug`).

---

## Table of contents

1. [Getting Started](#1-getting-started)
2. [The Application shell](#2-the-application-shell)
3. [The Scene system](#3-the-scene-system)
4. [Resources](#4-resources)
5. [Game objects & components](#5-game-objects--components)
   - [Transform](#transform)
   - [Sprite_renderer](#sprite_renderer)
   - [Script](#script)
   - [Animator](#animator)
   - [Rigidbody](#rigidbody)
   - [Collider & Collision_system](#collider--collision_system)
   - [Health](#health)
   - [Audio_emitter](#audio_emitter)
6. [Audio](#6-audio)
7. [Config & persistence](#7-config--persistence)
8. [The UI system](#8-the-ui-system)
   - [Layout model](#layout-model)
   - [Widget catalogue](#widget-catalogue)
   - [Theming & per-state visuals](#theming--per-state-visuals)
   - [Focus & keyboard](#focus--keyboard)
9. [Debug tools](#9-debug-tools)
10. [Further reading](#10-further-reading)

---

## 1. Getting Started

### Building & the umbrella header

Titan builds with CMake; SFML and the JSON/TOML/msgpack libraries are fetched automatically. The build globs `src/**.cpp`, so any file you add under `src/` is picked up with no CMake edits.

Pull in the whole engine with one header:

```cpp
#include "titan.hpp"   // brings in every module
```

Or include only what you use, e.g. `#include "module-game/include/game.hpp"`.

The minimal program:

```cpp
#include "titan.hpp"

int main() {
    titan::app::Application app;   // default 1280x720 window titled "Titan"
    return app.run();              // runs until the window is closed
}
```

### Adding Titan to your project

In your project's `CMakeLists.txt`, fetch Titan and link the `Titan::Titan` target. That target carries its include path and all dependencies transitively — you do not need to find or link SFML yourself.

```cpp
cmake_minimum_required(VERSION 3.28)
project(MyGame LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    Titan
    GIT_REPOSITORY https://github.com/HannesW101/Titan
    GIT_TAG        v1.0.0        # pin to a released version
)
FetchContent_MakeAvailable(Titan)

add_executable(MyGame main.cpp)
target_link_libraries(MyGame PRIVATE Titan::Titan)
```

> **One-time note for CMake 4+**: Titan's msgpack dependency predates CMake 4's minimum-version policy. If you build with CMake 4 or newer and hit a policy error from a fetched dependency, configure once with `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`. This affects the third-party dependency only, not your own project.

### Dependencies & system packages

Fetched automatically at configure time, pinned to these versions:

| Dependency | Version |
| --- | --- |
| SFML | `3.1.0` |
| nlohmann/json | `v3.12.0` |
| toml++ | `v3.4.0` |
| msgpack-c | `cpp-7.0.0` |

On Linux, SFML 3.1 needs these system packages (install once):

```bash
sudo apt install libxrandr-dev libxcursor-dev libxi-dev libudev-dev \
                 libgl1-mesa-dev libfreetype-dev libharfbuzz-dev \
                 libopenal-dev libvorbis-dev libflac-dev
```

> **SFML 3.1 notes**: HarfBuzz is new in SFML 3.1 (text shaping). Titan disables SFML's Network module, so MbedTLS is **not** required. On Windows/macOS SFML supplies its own backends and the apt packages don't apply.

That is the whole integration. In code you now write:

```cpp
#include "titan.hpp"
```

> **Verified**: This exact setup was used to fetch every dependency, compile the full Titan library, link against SFML 3.1.0 (with nlohmann/json 3.12.0, toml++ 3.4.0, and msgpack 7.0.0), and produce a running binary — including the complete games at the end of this manual.

### Hello, Titan

The smallest complete program opens a window and runs the main loop until you close it:

```cpp
#include "titan.hpp"

int main() {
    titan::app::Application app;   // 1280x720 window titled "Titan"
    return app.run();              // blocks until the window closes
}
```

### Engine layout

Everything lives under `titan::`, split into focused namespaces. Include `titan.hpp` for all of it, or a single module header (e.g. `module-game/include/game.hpp`) for just one area.

| Namespace | Module | What it gives you |
| --- | --- | --- |
| `titan::app` | Application | Window, main loop, subsystem ownership |
| `titan::scene` | Scene | Screen stack, transitions, lifecycle |
| `titan::game` | Game | World, game objects, components, systems |
| `titan::ui` | UI | Retained-mode widget tree, theming, focus |
| `titan::render` | Render | Layered renderer, cameras |
| `titan::resources` | Resources | Ref-counted asset loading |
| `titan::audio` | Audio | Bus mixer, SFX, music |
| `titan::core` | Core | Config, timing, events |
| `titan::utils` | Utils | Math, logging, random, string helpers |
| `titan::debug` | Debug | Profiler, on-screen overlay |

> **Conventions**: Types are `Capitalised`, member variables `_underscored`, functions `snake_case`. Setters are plain (no chaining): configure an object with a sequence of `set_*` calls.


## 2. The Application shell

`titan::app::Application` owns the window, renderer, audio system, the
Resource_manager (a singleton it references), and the frame timer. It runs the
main loop and calls back into your code.

### Configuring the window

```cpp
using namespace titan;

app::Application_config cfg;
cfg.title          = "My Game";
cfg.width          = 1600;
cfg.height         = 900;
cfg.fullscreen     = false;
cfg.resizable      = true;
cfg.vsync          = true;
cfg.framerate_limit = 0;            // 0 = uncapped (ignored when vsync is on)
cfg.fixed_timestep  = 1.0f / 60.0f; // gameplay/physics step
cfg.max_fixed_steps_per_frame = 5;  // anti "spiral of death"
cfg.clear_color    = sf::Color(18, 18, 28);

app::Application app(cfg);
```

### The loop & callbacks

The loop each frame: poll input → tick timer → run zero or more fixed updates →
run one variable update → render.

```cpp
app.set_on_start([&]{
    // subsystems are up; load the first scene or build initial UI here
});

app.set_fixed_update([&](float dt){
    // dt == cfg.fixed_timestep, called 0+ times/frame — physics & gameplay
});

app.set_update([&](float dt){
    // dt == real frame delta — animation, UI, input-driven logic
});

app.set_render([&](render::Renderer& r){
    // submit drawables; begin/end frame is handled for you
});

app.set_on_stop([&]{ /* loop ended, subsystems still alive */ });

return app.run();
```

Call `app.quit()` to stop after the current frame. Closing the window also stops
the loop.

### Subsystem access

```cpp
sf::RenderWindow&            win   = app.window();
render::Renderer&            r     = app.renderer();
resources::Resource_manager& res   = app.resources();
audio::Audio_system&         sound = app.audio();
core::Timer&                 timer = app.frame_timer();
```

### Changing settings at runtime

```cpp
app::Application_config c = app.config();
c.fullscreen = true;
app.apply_config(c);   // recreates the window in place; existing refs stay valid
```

> In practice you rarely wire the loop callbacks by hand — you hand control to a
> `Scene_manager` (next section), which is the normal way to structure a game.

---

## 3. The Scene system

A **Scene** is a screen: main menu, gameplay, pause overlay, settings. A
**Scene_manager** owns a stack of scenes and drives their lifecycle.

### Defining a scene

```cpp
using namespace titan;

class Main_menu : public scene::Scene {
public:
    Main_menu() : Scene("main_menu") {}

    void on_enter() override { /* build UI, load menu assets */ }
    void on_exit()  override { /* drop references; resources auto-free */ }

    void update(float dt) override { /* ... */ }
    void render(render::Renderer& r) override { /* ... */ }
};
```

Override only what you need. Hooks: `on_enter`, `on_exit`, `on_pause` (something
pushed on top), `on_resume` (became top again), `fixed_update`, `update`,
`render`.

### Wiring the manager into the app

```cpp
scene::Scene_manager scenes;

app.set_on_start([&]{
    scenes.attach(app);                              // gives scenes subsystem access
    scenes.push(std::make_unique<Main_menu>());
});
app.set_fixed_update([&](float dt){ scenes.fixed_update(dt); });
app.set_update      ([&](float dt){ scenes.update(dt); });
app.set_render      ([&](render::Renderer& r){ scenes.render(r); });
```

### Transitions (deferred — safe to call mid-update)

```cpp
scenes().push(std::make_unique<Gameplay>());   // pause current, add on top
scenes().pop();                                // remove top, resume below
scenes().replace(std::make_unique<Gameplay>()); // clear stack, set this one
scenes().clear();                              // empty the stack
```

Inside a scene, reach the manager and app via `scenes()` and `application()`.

### Overlays (pause menu over a frozen game)

```cpp
class Pause_menu : public scene::Scene {
public:
    Pause_menu() : Scene("pause") {
        set_renders_below(true);   // keep drawing the game underneath
        set_updates_below(false);  // but freeze its updates
    }
};
// from gameplay:
scenes().push(std::make_unique<Pause_menu>());
```

Scene-tier resources free automatically: when a scene is popped and drops its
`shared_ptr`s, the manager calls `flush_unused()` so anything no longer
referenced is released.

---

## 4. Resources

`titan::resources::Resource_manager` is a singleton with reference-counted,
templated load/get. Supported types include `sf::Texture`, `sf::Font`,
`sf::SoundBuffer`, `sf::Music`, and `titan::resources::Spritesheet`.

```cpp
auto& res = titan::resources::Resource_manager::instance();
// or: app.resources();

res.load<sf::Texture>("player", "assets/player.png");
res.load<sf::Font>("ui_font", "assets/Roboto.ttf");
res.load<sf::SoundBuffer>("explosion", "assets/boom.wav");

std::shared_ptr<sf::Texture> tex = res.get<sf::Texture>("player");

res.unload("player");   // force-unload one
res.flush_unused();     // release everything with no remaining owners
res.clear();            // wipe all
```

Because loads are cached by name, calling `load` twice with the same name
returns the existing resource.

---

## 5. Game objects & components

The gameplay model is **composition**: a `World` owns `Game_object`s, and each
object is a bag of `Game_component`s plus a built-in `Transform`. You assemble
behaviour from components instead of subclassing per entity type.

### World basics

```cpp
using namespace titan::game;

World world;

Game_object* player = world.spawn("player");
player->transform().set_position(400.f, 300.f);

// per frame (usually from a Scene):
world.fixed_update(dt);   // physics/gameplay
world.update(dt);         // merges new spawns, updates, reaps destroyed
world.render(renderer);

// queries
Game_object* p = world.find("player");
Game_object* byId = world.find_by_id(1);
std::size_t  n = world.count();
```

Objects are destroyed deferred-style: `obj->destroy()` marks it, and `World`
removes it at a safe point, so a component can destroy its own object mid-update.

### Adding & getting components

```cpp
auto* sr = player->add_component<Sprite_renderer>();   // returns T*
player->add_component<Health>(100);                    // ctor args forwarded

Sprite_renderer* found = player->get_component<Sprite_renderer>(); // or nullptr
bool hasHp = player->has_component<Health>();
player->remove_component<Health>();
```

Component lifecycle hooks fire automatically: `on_attach` (added), `on_start`
(first active frame), `fixed_update`, `update`, `render`, `on_detach`.

---

### Transform

Every object has one (built in). Local space, plus world space that composes a
parent chain.

```cpp
Transform& t = obj->transform();
t.set_position(100.f, 50.f);
t.move({ 10.f, 0.f });
t.set_rotation(45.f);          // degrees
t.rotate(5.f);
t.set_scale(2.f, 2.f);

sf::Vector2f wp = t.world_position();
sf::Transform m = t.world_matrix();

// parenting: a turret that follows its tank
turret->transform().set_parent(&tank->transform());
```

---

### Sprite_renderer

Draws a texture at the owner's world transform. Texture is pulled from the
Resource_manager by name.

```cpp
auto* sr = obj->add_component<Sprite_renderer>();
sr->set_texture("player");              // resource name
sr->set_origin_centered(true);          // default true
sr->set_color(sf::Color::White);        // tint
sr->set_layer(render::Render_layer::ENTITIES);  // default ENTITIES
sr->set_sort_key(0.f);                  // draw order within the layer

// draw a sub-rectangle (atlases)
sr->set_texture_rect(sf::IntRect({0,0},{32,32}));
sr->clear_texture_rect();
```

Render layers, back to front: `BACKGROUND`, `WORLD_TILES`, `WORLD_OBJECTS`,
`ENTITIES`, `PARTICLES`, `FOREGROUND`, `UI`, `UI_OVERLAY`, `DEBUG`.

---

### Script

Run behaviour through callbacks instead of writing a new component class. Each
callback receives the owning object.

```cpp
auto* s = obj->add_component<Script>();

s->set_start_fn([](Game_object& self){
    self.transform().set_position(0.f, 0.f);
});

s->set_update_fn([](Game_object& self, float dt){
    self.transform().move({ 60.f * dt, 0.f });   // 60 px/sec to the right
});

s->set_fixed_update_fn([](Game_object& self, float fixed_dt){ /* ... */ });
s->set_detach_fn([](Game_object& self){ /* cleanup */ });
```

---

### Animator

Plays named animations from a `Spritesheet` and feeds frames to the object's
`Sprite_renderer`. Put both on the same object.

```cpp
// the spritesheet resource must be loaded first
res.load<resources::Spritesheet>("hero", "assets/hero_sheet.png");

auto* sr = obj->add_component<Sprite_renderer>();
sr->set_texture("hero");

auto* anim = obj->add_component<Animator>();
anim->set_spritesheet("hero");

resources::Animation run;
run.name = "run";
run.frames = { sf::IntRect({0,0},{32,32}),
               sf::IntRect({32,0},{32,32}),
               sf::IntRect({64,0},{32,32}) };
run.fps = 12.f;
anim->add_animation(run);

anim->play("run");
anim->pause();
anim->resume();
anim->reset();
bool done = anim->is_finished();   // for ONE_SHOT animations
```

---

### Rigidbody

Lightweight kinematic motion integrated each fixed step into the Transform.
Not a full physics engine — pair it with a Collider for response.

```cpp
auto* rb = obj->add_component<Rigidbody>();
rb->set_velocity({ 50.f, 0.f });        // px/sec
rb->set_gravity({ 0.f, 980.f });        // px/sec^2
rb->set_mass(2.f);
rb->set_linear_damping(0.1f);           // fraction of velocity lost per second
rb->add_force({ 200.f, 0.f });          // accumulated for the next fixed step
rb->set_acceleration({ 0.f, 0.f });     // constant base acceleration
```

---

### Collider & Collision_system

A `Collider` is an AABB or circle at the owner's position plus an offset. A
`Collision_system` tests all pairs each fixed step and fires enter/stay/exit
callbacks on both colliders.

```cpp
auto* c = obj->add_component<Collider>();
c->set_box({ 16.f, 16.f });        // 32x32 box (half-extents)
// or: c->set_circle(20.f);
c->set_offset({ 0.f, -4.f });
c->set_trigger(false);             // true = overlap reported, no implied response

c->set_on_enter([](Collider& self, Collider& other){
    // e.g. deal damage to the other object
    if (auto* hp = other.owner().get_component<Health>()) hp->damage(10);
});
c->set_on_stay ([](Collider& self, Collider& other){ /* ... */ });
c->set_on_exit ([](Collider& self, Collider& other){ /* ... */ });
```

Drive it from your scene, after movement:

```cpp
game::Collision_system collisions;   // keep one alive across frames

void fixed_update(float dt) override {
    _world.fixed_update(dt);   // moves things
    collisions.update(_world); // then detect + fire callbacks
}
```

`collisions.clear()` forgets all tracked contacts (e.g. on reset).

---

### Health

Hit points with damage/heal and death handling. Destroys the owner on death by
default; the death callback runs first.

```cpp
auto* hp = obj->add_component<Health>(100);   // max 100, full

hp->damage(30);          // -> 70
hp->heal(10);            // -> 80 (capped at max)
int cur = hp->current();
int mx  = hp->max();
bool dead = hp->is_dead();

hp->set_on_damage([](Game_object& self, int amount){ /* flash red */ });
hp->set_on_death ([](Game_object& self){ /* spawn loot, play sfx */ });

hp->set_destroy_on_death(false);   // keep the object after death (e.g. ragdoll)
hp->revive();                      // back to full, clears dead state
```

---

### Audio_emitter

Plays positioned SFX. The Audio_system isn't owned by components, so wire it in
(the scene has it via `application().audio()`).

```cpp
auto* em = obj->add_component<Audio_emitter>();
em->set_audio_system(application().audio());
em->set_bus(audio::Audio_bus::SFX);
em->set_falloff_radius(600.f);   // distance to silence
em->set_spatial(true);

em->play("explosion");                  // positioned at owner's transform
em->play("hit", /*volume*/0.8f, /*pitch*/1.2f);
```

---

## 6. Audio

`titan::audio::Audio_system` mixes through buses: `MASTER`, `MUSIC`, `SFX`,
`UI`, `VOICE`, `AMBIENT`. Effective volume = MASTER × bus.

```cpp
audio::Audio_system& sound = app.audio();

sound.set_bus_volume(audio::Audio_bus::MASTER, 0.8f);
sound.set_bus_volume(audio::Audio_bus::SFX, 1.0f);
float v = sound.bus_volume(audio::Audio_bus::MUSIC);
sound.set_bus_muted(audio::Audio_bus::SFX, false);

// one-shot effect (resource name = sf::SoundBuffer key)
int id = sound.play_sfx("explosion", audio::Audio_bus::SFX);

// persistence (see Config below)
sound.save_mix(config);   // write all bus volumes/mutes into a config
sound.load_mix(config);   // restore them
```

The Application calls `sound.update(dt)` for you each frame.

---

## 7. Config & persistence

`titan::core::Config` is a sectioned key/value store; `Config_manager` keeps
named configs ("settings", "keybinds", …). Value types: `bool`, `int`, `long`,
`float`, `double`, `std::string`.

```cpp
core::Config& cfg = core::Config_manager::instance().get("settings");

cfg.set<int>("video", "width", 1600);
cfg.set<bool>("video", "fullscreen", true);

int w = cfg.get<int>("video", "width").value_or(1280);
bool fs = cfg.get<bool>("video", "fullscreen").value_or(false);

if (cfg.has("video", "width")) { /* ... */ }
```

The Application_config can serialise itself to a config:

```cpp
app::Application_config ac = app.config();
ac.save_to_config(cfg);     // writes [application] section
ac.load_from_config(cfg);   // reads it back
```

---

## 8. The UI system

UI is a retained-mode tree of `Widget`s. The easiest entry point is
`titan::ui::UI_system`, which bundles a `UI_manager` + `Focus_manager` and wires
into the loop. A scene typically owns one.

```cpp
class Settings : public scene::Scene {
public:
    Settings() : Scene("settings") {}

    void on_enter() override {
        _ui = std::make_unique<ui::UI_system>(application());
        build_ui(_ui->manager());
    }
    void update(float dt) override { _ui->update(dt); }
    void render(render::Renderer&) override { _ui->render(); }

private:
    std::unique_ptr<ui::UI_system> _ui;
};
```

`UI_system::update` syncs the mouse from the window, runs layout, and updates;
`render()` draws on the UI layers. Call `layout()` again on window resize.

### Layout model

Widgets position with an **anchor** (where in the parent), an **offset**, and a
**size**. Sizes use `UI_length::px(n)` (pixels) or `UI_length::pct(f)` (fraction
of parent, 0–1).

Anchors: `TOP_LEFT`, `TOP_CENTER`, `TOP_RIGHT`, `MIDDLE_LEFT`, `MIDDLE_CENTER`,
`MIDDLE_RIGHT`, `BOTTOM_LEFT`, `BOTTOM_CENTER`, `BOTTOM_RIGHT`.

```cpp
using namespace titan::ui;

auto panel = std::make_unique<Panel>("panel");
panel->set_anchor(UI_anchor::MIDDLE_CENTER);
panel->set_size(UI_length::px(420.f), UI_length::px(480.f));
panel->set_padding(16.f);
```

Containers auto-arrange children. `V_box`/`H_box` stack vertically/horizontally;
`Grid` lays out in columns; `Spacer` pushes things apart.

```cpp
auto box = std::make_unique<V_box>("col");
box->set_size(UI_length::pct(1.f), UI_length::pct(1.f));
box->set_spacing(10.f);
box->add_child(std::move(child_a));
box->add_child(std::move(child_b));
panel->add_child(std::move(box));

// Grid
auto grid = std::make_unique<Grid>("grid");
grid->set_columns(3);
grid->set_spacing(6.f);
```

Add the top-level widget to the manager (it takes ownership):

```cpp
UI_manager& gui = _ui->manager();
gui.add(std::move(panel));
Widget* found = gui.find("panel");
```

### Widget catalogue

All widgets share the layout setters above (`set_anchor`, `set_size`, etc.) and
the visual/theming API. Setters are plain (no chaining). Construct with
`std::make_unique<T>("name")`, configure, then `add_child`/`add`.

**Button**
```cpp
auto b = std::make_unique<Button>("start");
b->set_text("Start Game");
b->set_on_click([]{ /* ... */ });
```

**Label**
```cpp
auto l = std::make_unique<Label>("title");
l->set_text("Hello");
l->set_align(Text_align::CENTER);   // LEFT | CENTER | RIGHT
l->set_wrap(true);
```

**Checkbox / Toggle**
```cpp
auto cb = std::make_unique<Checkbox>("fs");
cb->set_label("Fullscreen");
cb->set_checked(false);
cb->set_on_changed([](bool on){ /* ... */ });

auto tg = std::make_unique<Toggle>("vsync");
tg->set_label("VSync");
tg->set_on(true);
tg->set_on_changed([](bool on){ /* ... */ });
```

**Slider**
```cpp
auto s = std::make_unique<Slider>("vol");
s->set_range(0.f, 1.f);
s->set_value(0.8f);
s->set_step(0.05f);                    // 0 = continuous
s->set_orientation(Slider::Orientation::HORIZONTAL);
s->set_on_changed([](float v){ /* ... */ });
```

**Spin_box** (numeric +/- with editable field)
```cpp
auto sp = std::make_unique<Spin_box>("count");
sp->set_range(0.f, 100.f);
sp->set_value(50.f);
sp->set_step(5.f);
sp->set_decimals(0);
sp->set_on_changed([](float v){ /* ... */ });
```

**Text_field**
```cpp
auto tf = std::make_unique<Text_field>("name");
tf->set_placeholder("Enter name...");
tf->set_max_length(24);
tf->set_password(false);
tf->set_on_changed([](std::string const& s){ /* ... */ });
tf->set_on_submit ([](std::string const& s){ /* Enter pressed */ });
```

**Dropdown**
```cpp
auto dd = std::make_unique<Dropdown>("quality");
dd->set_options({ "Low", "Medium", "High", "Ultra" });
dd->set_selected(2);
dd->set_on_changed([](std::size_t i, std::string const& text){ /* ... */ });
```

**Progress_bar / Gauge**
```cpp
auto pb = std::make_unique<Progress_bar>("hp");
pb->set_value(0.75f);              // 0..1
pb->set_show_label(true);
pb->set_animated(true, 3.f);

auto g = std::make_unique<Gauge>("speed");
g->set_value(0.5f);
g->set_angles(-90.f, 270.f);       // start, sweep (degrees)
g->set_thickness(8.f);
g->set_colors(sf::Color(60,60,90), sf::Color(80,140,230));
g->set_show_text(true);
```

**List_box**
```cpp
auto lb = std::make_unique<List_box>("saves");
lb->set_items({ "Slot 1", "Slot 2", "Slot 3" });
lb->set_item_height(24.f);
lb->set_multi_select(false);
std::size_t sel = lb->selected();
```

**Tab_view**
```cpp
auto tabs = std::make_unique<Tab_view>("tabs");
tabs->add_tab("Video", std::move(video_panel));
tabs->add_tab("Audio", std::move(audio_panel));
tabs->set_on_tab_changed([](std::size_t i, std::string const& name){ /* ... */ });
```

**Inventory: Item_grid / Item_slot** (drag-and-drop built in)
```cpp
auto grid = std::make_unique<Item_grid>("inv");
grid->configure(/*cols*/5, /*rows*/4, /*slot px*/48.f, /*gap*/4.f);
// after layout, slots exist:
if (auto* slot = grid->slot(0)) {
    slot->set_item(/*item id*/1001, "icon_sword");
    slot->set_count(3);
}
```

Other available widgets: `Image`, `Separator`, `Tooltip`, `Toast`,
`Icon_button`, `Radio_button`/`Radio_group`, `Segmented_control`, `Number_field`,
`Text_area`, `Rich_text`, `Scroll_view`, `Window`, `Resizable_window`, `Modal`,
`Nine_slice_panel`, `Tree_view`, `Table`, `Virtual_list`, `Minimap`,
`Radial_menu`, `Menu_bar`, `Context_menu`.

### Theming & per-state visuals

Every widget draws its background from a `Visual`, which is one of: none, solid
color, texture, or nine-slice. Widgets pick visuals per interaction state
(`NORMAL`, `HOVERED`, `PRESSED`, `FOCUSED`, `DISABLED`) from the active `Theme`,
and you can override per widget.

A built-in dark theme is applied by default. Build visuals with the factories:

```cpp
Visual flat   = Visual::color(sf::Color(40,40,60));
Visual edged  = Visual::color(sf::Color(40,40,60), sf::Color(90,90,140), 1.5f);
Visual tex    = Visual::texture("btn_normal");
Visual frame  = Visual::nine_slice("frame9", 12, 12, 12, 12); // top,right,bottom,left
```

Override one state, or set a whole set:

```cpp
button->set_visual(Widget_state::NORMAL,  Visual::texture("btn_n"));
button->set_visual(Widget_state::HOVERED, Visual::texture("btn_h"));
button->set_visual(Widget_state::PRESSED, Visual::texture("btn_p"));

// or build a Visual_set:
Visual_set vs;
vs.normal (Visual::texture("btn_n"));
vs.hovered(Visual::texture("btn_h"));
vs.pressed(Visual::texture("btn_p"));
button->set_visuals(vs);
```

Load a theme from a TOML config (sections like `[ui.button]` with per-state
keys `normal_bg`, `hovered_texture`, `pressed_nine`, …):

```cpp
core::Config& theme_cfg = core::Config_manager::instance().get("theme");
// theme_cfg loaded from a .toml ...
gui.load_theme(theme_cfg);
```

### Focus & keyboard

The `Focus_manager` (inside `UI_system`) cycles focusable widgets with Tab /
Shift+Tab. Inputs (Button, Slider, Text_field, …) are focusable by default.

```cpp
_ui->focus().focus_next();
_ui->focus().focus_previous();
_ui->focus().clear_focus();
_ui->focus().set_enabled(true);   // toggle Tab handling
```

---

## 9. Debug tools

### Profiler

Frame timing plus named scope timers. A singleton is provided.

```cpp
auto& prof = debug::Profiler::instance();

// each frame:
prof.begin_frame();
{
    TITAN_PROFILE_SCOPE(prof, "physics");
    world.fixed_update(dt);
}
{
    TITAN_PROFILE_SCOPE(prof, "render");
    world.render(r);
}
prof.end_frame();

float fps = prof.fps();
float ms  = prof.average_frame_ms();
float phys = prof.scope_ms("physics");
```

### Debug_overlay

On-screen FPS, scope timings, a frame-time graph, and custom watch lines.

```cpp
debug::Debug_overlay overlay;
overlay.set_font("ui_font");          // a loaded sf::Font resource
overlay.set_show_scopes(true);

// each frame, after profiler.end_frame():
overlay.set_watch("objects", std::to_string(world.count()));
overlay.render(app.renderer(), prof);

overlay.toggle();                      // e.g. bound to F3
```

---

## 10. Further reading

The complete API reference lives in `Titan Engine — docs/Library_Overview.md`.
The worked gameplay example lives in `Titan Engine — docs/Examples.md`.
