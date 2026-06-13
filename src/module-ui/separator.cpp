// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/separator.hpp"

#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Separator::Separator(std::string const& name) : Widget(name) {}
void Separator::set_orientation(Orientation o){_orient=o;}
void Separator::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    float thick=2.f;
    sf::FloatRect line = _orient==Orientation::HORIZONTAL
        ? sf::FloatRect({r.position.x,r.position.y+(r.size.y-thick)*0.5f},{r.size.x,thick})
        : sf::FloatRect({r.position.x+(r.size.x-thick)*0.5f,r.position.y},{thick,r.size.y});
    visual_renderer::draw(resolved_visual(),line,renderer,draw_buffer(),resource_manager(),layer);
}
} // namespace titan::ui
