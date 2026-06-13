// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-scene/include/scene.hpp"

#include <stdexcept>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::scene {

// ============================================================================
// Scene
// ============================================================================

// ----------------------------------------------------------------------------
Scene::Scene(
    std::string name
    )
    : _name(std::move(name))
{}

// ----------------------------------------------------------------------------
bool Scene::renders_below    (          ) const { return _renders_below;  }
void Scene::set_renders_below(bool value)       { _renders_below = value; }
bool Scene::updates_below    (          ) const { return _updates_below;  }
void Scene::set_updates_below(bool value)       { _updates_below = value; }

// ----------------------------------------------------------------------------
std::string const& Scene::name() const { return _name; }

// ----------------------------------------------------------------------------
app::Application& Scene::application() {

    if (!_app) {

        throw std::runtime_error("Scene '" + _name + "' has no Application context yet");
    }

    return *_app;
}

// ----------------------------------------------------------------------------
Scene_manager& Scene::scenes() {

    if (!_manager) {

        throw std::runtime_error("Scene '" + _name + "' has no Scene_manager context yet");
    }

    return *_manager;
}

} // namespace titan::scene
