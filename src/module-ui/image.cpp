// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/image.hpp"

#include "module-ui/include/visual.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Image::Image(std::string const& name) : Widget(name) {}
void Image::set_texture(std::string const& t){_tex=t;}
void Image::set_uv_rect(sf::IntRect r){_uv=r;}
void Image::clear_uv_rect(){_uv.reset();}
void Image::set_tint(sf::Color c){_tint=c;}
void Image::set_preserve_aspect(bool p){_aspect=p;}
void Image::on_render(render::Renderer& renderer){
    if(_tex.empty()) return;
    Visual v=Visual::texture(_tex,_tint,_aspect);
    v.uv_rect=_uv;
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    visual_renderer::draw(v,rect(),renderer,draw_buffer(),resource_manager(),layer);
}
} // namespace titan::ui
