// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-scene/include/scene_manager.hpp"

#include "module-app/include/application.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-render/include/renderer.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Color.hpp"

#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::scene {

// ============================================================================
// Setup
// ============================================================================

// ----------------------------------------------------------------------------
void Scene_manager::attach(
    app::Application& application
    ) {

    _app = &application;
}

// ----------------------------------------------------------------------------
void Scene_manager::_attach_scene(
    Scene& scene
    ) {

    scene._app     = _app;
    scene._manager = this;
}

// ----------------------------------------------------------------------------
void Scene_manager::_flush_resources() {

    // Scenes drop their resource shared_ptrs in on_exit(); reclaim the unused.
    resources::Resource_manager::instance().flush_unused();
}

// ============================================================================
// Transition requests (deferred)
// ============================================================================

// ----------------------------------------------------------------------------
void Scene_manager::push(
    std::unique_ptr<Scene> scene
    ) {

    _pending.push_back({ Op::PUSH, std::move(scene) });
}

// ----------------------------------------------------------------------------
void Scene_manager::pop() {

    _pending.push_back({ Op::POP, nullptr });
}

// ----------------------------------------------------------------------------
void Scene_manager::replace(
    std::unique_ptr<Scene> scene
    ) {

    _pending.push_back({ Op::REPLACE, std::move(scene) });
}

// ----------------------------------------------------------------------------
void Scene_manager::replace_with_fade(
    std::unique_ptr<Scene> scene,
    float                  fade_seconds
    ) {

    _fade_pending_scene = std::move(scene);
    _fade_duration      = fade_seconds;
    _fade_elapsed       = 0.0f;
    _fade_phase         = Fade_phase::OUT;
}

// ----------------------------------------------------------------------------
float Scene_manager::fade_alpha() const {

    if (_fade_phase == Fade_phase::NONE) { return 0.0f; }
    float t = (_fade_duration > 0.0f) ? (_fade_elapsed / _fade_duration) : 1.0f;
    if (t > 1.0f) { t = 1.0f; }
    return (_fade_phase == Fade_phase::OUT) ? t : (1.0f - t);
}

// ----------------------------------------------------------------------------
void Scene_manager::clear() {

    _pending.push_back({ Op::CLEAR, nullptr });
}

// ============================================================================
// Apply deferred transitions
// ============================================================================

// ----------------------------------------------------------------------------
void Scene_manager::_do_replace(
    std::unique_ptr<Scene> scene
    ) {

    while (!_stack.empty()) {
        _stack.back()->on_exit();
        _stack.pop_back();
    }
    _flush_resources();
    _attach_scene(*scene);
    _stack.push_back(std::move(scene));
    _stack.back()->on_enter();
}

// ----------------------------------------------------------------------------
void Scene_manager::_apply_pending() {

    if (_pending.empty()) { return; }

    // Move out the queue first; applying ops must not be disrupted if a hook
    // queues further transitions (those run next frame).
    std::vector<Pending> ops;
    ops.swap(_pending);

    for (auto& p : ops) {

        switch (p.op) {

            case Op::PUSH: {
                if (!_stack.empty()) { _stack.back()->on_pause(); }
                _attach_scene(*p.scene);
                _stack.push_back(std::move(p.scene));
                _stack.back()->on_enter();
                break;
            }

            case Op::POP: {
                if (!_stack.empty()) {
                    _stack.back()->on_exit();
                    _stack.pop_back();
                    _flush_resources();
                    if (!_stack.empty()) { _stack.back()->on_resume(); }
                }
                break;
            }

            case Op::REPLACE: {
                _do_replace(std::move(p.scene));
                break;
            }

            case Op::CLEAR: {
                while (!_stack.empty()) {
                    _stack.back()->on_exit();
                    _stack.pop_back();
                }
                _flush_resources();
                break;
            }

            default:
                break;
        }
    }
}

// ============================================================================
// Per-frame
// ============================================================================

// ----------------------------------------------------------------------------
void Scene_manager::fixed_update(
    float fixed_dt
    ) {

    if (_stack.empty()) { return; }

    // Walk down from the top while scenes opt to let those below update.
    std::size_t start = _stack.size();
    while (start > 0u) {

        --start;

        if (!_stack[start]->updates_below()) { break; }
    }

    for (std::size_t i = start; i < _stack.size(); ++i) {

        _stack[i]->fixed_update(fixed_dt);
    }
}

// ----------------------------------------------------------------------------
void Scene_manager::update(
    float dt
    ) {

    // Apply any transitions requested last frame, before running update logic.
    _apply_pending();

    // Advance fade-transition timer.
    if (_fade_phase != Fade_phase::NONE) {

        _fade_elapsed += dt;

        if (_fade_elapsed >= _fade_duration) {

            if (_fade_phase == Fade_phase::OUT) {
                // Fully black — swap the scene, then start fading in.
                _do_replace(std::move(_fade_pending_scene));
                _fade_elapsed = 0.0f;
                _fade_phase   = Fade_phase::IN;
            } else {
                // Fade-in complete.
                _fade_phase = Fade_phase::NONE;
            }
        }
    }

    if (_stack.empty()) { return; }

    std::size_t start = _stack.size();
    while (start > 0u) {

        --start;

        if (!_stack[start]->updates_below()) { break; }
    }

    for (std::size_t i = start; i < _stack.size(); ++i) {

        _stack[i]->update(dt);
    }
}

// ----------------------------------------------------------------------------
void Scene_manager::render(
    render::Renderer& renderer
    ) {

    if (_stack.empty()) { return; }

    // Find the lowest scene that must render: walk down while each top scene
    // says "render below me", then draw bottom-up so overlays land on top.
    std::size_t start = _stack.size();
    while (start > 0u) {

        --start;

        if (!_stack[start]->renders_below()) { break; }
    }

    for (std::size_t i = start; i < _stack.size(); ++i) {

        _stack[i]->render(renderer);
    }

    // Draw fullscreen black rect during fade transitions.
    // Drawn directly to the window (not through the submit queue) so lifetime
    // is not an issue and it always lands on top of all queued layers.
    if (_fade_phase != Fade_phase::NONE) {

        sf::Vector2u const sz = renderer.window().getSize();
        sf::RectangleShape fade_rect(sf::Vector2f{
            static_cast<float>(sz.x),
            static_cast<float>(sz.y)
        });
        std::uint8_t const alpha = static_cast<std::uint8_t>(fade_alpha() * 255.0f);
        fade_rect.setFillColor(sf::Color(0, 0, 0, alpha));

        // Reset to default view so the rect covers the full window in screen space.
        renderer.window().setView(renderer.window().getDefaultView());
        renderer.window().draw(fade_rect);
    }
}

// ============================================================================
// Queries
// ============================================================================

// ----------------------------------------------------------------------------
Scene* Scene_manager::current() {

    return _stack.empty() ? nullptr : _stack.back().get();
}

// ----------------------------------------------------------------------------
std::size_t Scene_manager::depth() const { return _stack.size();  }
bool        Scene_manager::empty() const { return _stack.empty(); }

} // namespace titan::scene
