# Titan Engine — Game Examples

## A complete mini-game

A single gameplay scene: a player you move with the arrow keys (via a Script),
an enemy with health, a collision that damages the enemy, and a HUD label. This
ties the whole stack together.

```cpp
#include "titan.hpp"
#include "SFML/Window/Keyboard.hpp"

using namespace titan;

class Gameplay : public scene::Scene {
public:
    Gameplay() : Scene("gameplay") {}

    void on_enter() override {
        auto& res = application().resources();
        res.load<sf::Texture>("player", "assets/player.png");
        res.load<sf::Texture>("enemy",  "assets/enemy.png");
        res.load<sf::Font>("ui_font",   "assets/Roboto.ttf");
        res.load<sf::SoundBuffer>("hit", "assets/hit.wav");

        // ---- Player ----
        _player = _world.spawn("player");
        _player->transform().set_position(200.f, 300.f);
        _player->add_component<game::Sprite_renderer>()->set_texture("player");
        _player->add_component<game::Collider>()->set_box({ 16.f, 16.f });

        auto* move = _player->add_component<game::Script>();
        move->set_update_fn([](game::Game_object& self, float dt){
            float speed = 200.f;
            sf::Vector2f d{0.f, 0.f};
            using K = sf::Keyboard::Key;
            if (sf::Keyboard::isKeyPressed(K::Left))  d.x -= 1.f;
            if (sf::Keyboard::isKeyPressed(K::Right)) d.x += 1.f;
            if (sf::Keyboard::isKeyPressed(K::Up))    d.y -= 1.f;
            if (sf::Keyboard::isKeyPressed(K::Down))  d.y += 1.f;
            self.transform().move(d * speed * dt);
        });

        // ---- Enemy ----
        _enemy = _world.spawn("enemy");
        _enemy->transform().set_position(600.f, 300.f);
        _enemy->add_component<game::Sprite_renderer>()->set_texture("enemy");
        auto* ehp = _enemy->add_component<game::Health>(50);
        ehp->set_on_death([](game::Game_object& self){ self.destroy(); });

        auto* ec = _enemy->add_component<game::Collider>();
        ec->set_box({ 16.f, 16.f });
        ec->set_on_enter([this](game::Collider& self, game::Collider& other){
            if (other.owner().name() == "player") {
                if (auto* hp = self.owner().get_component<game::Health>()) {
                    hp->damage(10);
                    application().audio().play_sfx("hit", audio::Audio_bus::SFX);
                }
            }
        });

        // ---- HUD ----
        _ui = std::make_unique<ui::UI_system>(application());
        auto label = std::make_unique<ui::Label>("hud");
        label->set_anchor(ui::UI_anchor::TOP_LEFT);
        label->set_offset(12.f, 12.f);
        label->set_size(ui::UI_length::px(300.f), ui::UI_length::px(24.f));
        label->set_text("Bump the enemy!");
        _hud = static_cast<ui::Label*>(_ui->manager().add(std::move(label)));
    }

    void fixed_update(float dt) override {
        _world.fixed_update(dt);
        _collisions.update(_world);
    }

    void update(float dt) override {
        _world.update(dt);
        if (auto* e = _world.find("enemy")) {
            if (auto* hp = e->get_component<game::Health>())
                _hud->set_text("Enemy HP: " + std::to_string(hp->current()));
        } else {
            _hud->set_text("Enemy defeated!");
        }
        _ui->update(dt);
    }

    void render(render::Renderer& r) override {
        _world.render(r);
        _ui->render();
    }

private:
    game::World            _world;
    game::Collision_system _collisions;
    game::Game_object*     _player = nullptr;
    game::Game_object*     _enemy  = nullptr;

    std::unique_ptr<ui::UI_system> _ui;
    ui::Label*                     _hud = nullptr;
};

int main() {
    app::Application_config cfg;
    cfg.title = "Titan Demo";
    cfg.width = 1280;
    cfg.height = 720;

    app::Application app(cfg);

    scene::Scene_manager scenes;
    app.set_on_start([&]{
        scenes.attach(app);
        scenes.push(std::make_unique<Gameplay>());
    });
    app.set_fixed_update([&](float dt){ scenes.fixed_update(dt); });
    app.set_update      ([&](float dt){ scenes.update(dt); });
    app.set_render      ([&](render::Renderer& r){ scenes.render(r); });

    return app.run();
}
```

That's the full normal-usage surface: configure an `Application`, structure the
game with `Scene`s, build entities from `Game_object` + components, draw a UI
with the widget tree, route sound through audio buses, persist settings via
`Config`, and watch performance with the debug tools.


---

