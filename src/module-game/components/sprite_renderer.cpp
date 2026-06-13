// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/sprite_renderer.hpp"
#include "module-game/include/game_object.hpp"
#include "module-game/components/include/transform.hpp"

#include "module-render/include/renderer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Angle.hpp"

#include <exception>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Sprite_renderer
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void Sprite_renderer::set_texture(
    std::string const& name
    ) {

    _texture_name = name;
    _texture.reset();
    _sprite.reset();

    if (has_owner()) { _acquire_texture(); }
}

// ----------------------------------------------------------------------------
std::string const& Sprite_renderer::texture_name() const { return _texture_name; }

// ----------------------------------------------------------------------------
void Sprite_renderer::set_texture_rect  (sf::IntRect const rect) { _rect = rect;  }
void Sprite_renderer::clear_texture_rect(                      ) { _rect.reset(); }

// ----------------------------------------------------------------------------
void      Sprite_renderer::set_color(sf::Color const color)       { _color = color; }
sf::Color Sprite_renderer::color    (                     ) const { return _color;  }

// ----------------------------------------------------------------------------
void Sprite_renderer::set_origin_centered(bool                 const centered) { _centered = centered; }
void Sprite_renderer::set_layer          (render::Render_layer const layer   ) { _layer = layer;       }
void Sprite_renderer::set_sort_key       (float                const key     ) { _sort_key = key;      }

// ----------------------------------------------------------------------------
void Sprite_renderer::_acquire_texture() {

    if (_texture_name.empty()) { return; }

    try {

        _texture = resources::Resource_manager::instance().get<sf::Texture>(_texture_name);
    } catch (std::exception const&) {

        _texture = nullptr;
    }
}

// ----------------------------------------------------------------------------
void Sprite_renderer::on_start() {

    _acquire_texture();
}

// ----------------------------------------------------------------------------
void Sprite_renderer::render(
    render::Renderer& renderer
    ) {

    if (!_texture) {

        _acquire_texture();

        if (!_texture) { return; }
    }

    Transform const& tf = owner().transform();

    // Build (or rebuild) the keepalive sprite for this frame.
    _sprite.emplace(*_texture);
    if (_rect) { _sprite->setTextureRect(*_rect); }
    _sprite->setColor(_color);

    sf::FloatRect const bounds = _sprite->getLocalBounds();
    if (_centered) {

        _sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
    }

    _sprite->setPosition(tf.world_position());
    _sprite->setRotation(sf::degrees(tf.world_rotation()));
    _sprite->setScale(tf.world_scale());

    renderer.submit(_layer, *_sprite, sf::RenderStates::Default, _sort_key);
}

} // namespace titan::game
