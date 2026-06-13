// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/nine_slice_panel.hpp"

#include "module-ui/include/visual.hpp"
#include "module-ui/include/visual_renderer.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Nine_slice_panel::Nine_slice_panel(std::string const& name) : Widget(name) {}
void Nine_slice_panel::set_texture(std::string const& t){_tex=t;}
void Nine_slice_panel::set_border(float t,float r,float b,float l){_t=t;_r=r;_b=b;_l=l;}
void Nine_slice_panel::on_render(render::Renderer& renderer){
    if(_tex.empty()) return;
    Visual v=Visual::nine_slice(_tex,_t,_r,_b,_l);
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    visual_renderer::draw(v,rect(),renderer,draw_buffer(),resource_manager(),layer);
}
} // namespace titan::ui
