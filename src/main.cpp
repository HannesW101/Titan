// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "titan.hpp"
#include "SFML/Window/Keyboard.hpp"
#include <algorithm>

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan;

// ============================================================================
// Example program
// ----------------------------------------------------------------------------

// A two-paddle Pong game in a single scene. Demonstrates: World + objects,
// Script components for behaviour, Collider callbacks, and a HUD Label.
class Pong : public scene::Scene {
public:
    Pong() : Scene("pong") {}

    void on_enter() override {

        application().resources().load<sf::Font>("font", "assets/Roboto.ttf");

        float const W = 1280.f;
        float const H = 720.f;

        // --- Ball ---
        _ball = _world.spawn("ball");
        _ball->transform().set_position(W * 0.5f, H * 0.5f);

        auto* bc = _ball->add_component<game::Collider>();
        bc->set_box({ 8.f, 8.f });

        auto* bs = _ball->add_component<game::Script>();
        bs->set_start_fn([](game::Game_object& self){
            self.transform().set_position(640.f, 360.f);
            });

        // store velocity on the script via a captured state object
        auto vel = std::make_shared<sf::Vector2f>(300.f, 180.f);
        bs->set_update_fn([vel, W, H](game::Game_object& self, float dt){

            auto& t = self.transform();
            t.move(*vel * dt);
            sf::Vector2f p = t.position();

            if (p.y < 8.f || p.y > H - 8.f) { vel->y = -vel->y; } // top/bottom walls

            if (p.x < 0.f || p.x > W) { t.set_position(W*0.5f, H*0.5f); } // score reset
            });

        bc->set_on_enter([vel](game::Collider&, game::Collider& other){

            if (other.owner().name().rfind("paddle", 0) == 0) {

                vel->x = -vel->x * 1.05f;   // bounce + speed up
            }
            });

        // --- Paddles ---
        _make_paddle("paddle_left",  40.f,  H * 0.5f, sf::Keyboard::Key::W, sf::Keyboard::Key::S, H);
        _make_paddle("paddle_right", W-40.f, H * 0.5f, sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, H);

        // --- HUD ---
        _ui = std::make_unique<ui::UI_system>(application());

        auto title = std::make_unique<ui::Label>("title");
        title->set_anchor(ui::UI_anchor::TOP_CENTER);
        title->set_offset(0.f, 16.f);
        title->set_size(ui::UI_length::px(400.f), ui::UI_length::px(28.f));
        title->set_text("PONG  -  W/S vs Up/Down");
        title->set_align(ui::Text_align::CENTER);

        _ui->manager().add(std::move(title));
    }

    void fixed_update(float dt) override {

        _world.fixed_update(dt);
        _collisions.update(_world);
    }

    void update(float dt) override { _world.update(dt); _ui->update(dt); }
    void render(render::Renderer& r) override { _world.render(r); _ui->render(); }

private:
    void _make_paddle(
        std::string const& name,
        float x,
        float y,
        sf::Keyboard::Key up,
        sf::Keyboard::Key down,
        float H
        ) {

        auto* p = _world.spawn(name);
        p->transform().set_position(x, y);
        p->add_component<game::Collider>()->set_box({ 10.f, 60.f });

        auto* s = p->add_component<game::Script>();
        s->set_update_fn([up, down, H](game::Game_object& self, float dt){

            float speed = 400.f;

            if (sf::Keyboard::isKeyPressed(up))   self.transform().move({ 0.f, -speed * dt });
            if (sf::Keyboard::isKeyPressed(down)) self.transform().move({ 0.f,  speed * dt });

            sf::Vector2f pos = self.transform().position();
            pos.y = std::clamp(pos.y, 60.f, H - 60.f);
            self.transform().set_position(pos);
            });
    }

    game::World                    _world;
    game::Collision_system         _collisions;
    game::Game_object*             _ball = nullptr;
    std::unique_ptr<ui::UI_system> _ui;
};

int main() {

    app::Application_config cfg;
    cfg.title = "Titan Pong";
    app::Application app(cfg);

    scene::Scene_manager scenes;
    app.set_on_start([&]{ scenes.attach(app); scenes.push(std::make_unique<Pong>()); });

    app.set_fixed_update([&](float dt){ scenes.fixed_update(dt); });

    app.set_update([&](float dt){ scenes.update(dt); });

    app.set_render([&](render::Renderer& r){ scenes.render(r); });

    return app.run();
}
