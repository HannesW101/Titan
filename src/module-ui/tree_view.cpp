// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/tree_view.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"

#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Tree_view::Tree_view(std::string const& name):Widget(name){
    _root.expanded=true;
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_enabled()||!is_hovered()) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect(); float y=r.position.y;
            Node* hit=nullptr;
            for(auto const& c:_root.children){ hit=_hit(*c,y,m.y); if(hit) break; }
            if(hit){ if(!hit->children.empty()) hit->expanded=!hit->expanded; if(_on_selected)_on_selected(*hit); }
        }, listener_id());
}
Tree_view::Node& Tree_view::root_node(){ return _root; }
void Tree_view::set_row_height(float h){_row=h;}
void Tree_view::set_indent(float i){_indent=i;}
void Tree_view::set_on_selected(std::function<void(Node const&)> cb){_on_selected=std::move(cb);}
Tree_view::Node* Tree_view::_hit(Node& n,float& y,float my){
    if(my>=y&&my<y+_row) return &n;
    y+=_row;
    if(n.expanded) for(auto const& c:n.children) if(auto* h=_hit(*c,y,my)) return h;
    return nullptr;
}
void Tree_view::_render_node(Node const& n,render::Renderer& renderer,float& y,float indent,void* font_ptr){
    auto const& ta=resolved_text_style();
    sf::FloatRect r=rect();
    auto* font=static_cast<sf::Font*>(font_ptr);
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    if(y+_row>=r.position.y&&y<=r.position.y+r.size.y){
        if(!n.children.empty()){
            sf::ConvexShape a; a.setPointCount(3u); float s=_row*0.4f;
            if(n.expanded){ a.setPoint(0u,{0,0}); a.setPoint(1u,{s,0}); a.setPoint(2u,{s*0.5f,s}); }
            else { a.setPoint(0u,{0,0}); a.setPoint(1u,{0,s}); a.setPoint(2u,{s,s*0.5f}); }
            a.setPosition({r.position.x+indent-_indent+4.f,y+(_row-s)*0.5f}); a.setFillColor(ta.color);
            draw_buffer().add_convex(renderer,layer,std::move(a));
        }
        sf::Text t(*font,n.label,static_cast<unsigned int>(ta.size>0.f?ta.size:14.f));
        t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({r.position.x+indent,y+(_row-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
    y+=_row;
    if(n.expanded) for(auto const& c:n.children) _render_node(*c,renderer,y,indent+_indent,font_ptr);
}
void Tree_view::on_render(render::Renderer& renderer){
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    sf::FloatRect r=rect(); float y=r.position.y;
    for(auto const& c:_root.children) _render_node(*c,renderer,y,r.position.x+_indent,font.get());
}
} // namespace titan::ui
