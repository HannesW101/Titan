// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/rich_text.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Rich_text::Rich_text(std::string const& name):Widget(name){}
void Rich_text::clear_spans(){_spans.clear();}
void Rich_text::add_span(std::string const& t,sf::Color c,float s){_spans.push_back({t,c,s});}
void Rich_text::on_render(render::Renderer& renderer){
    if(_spans.empty()) return;
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    sf::FloatRect r=rect();
    float x=r.position.x, y=r.position.y; float line_h=0.f;
    for(auto const& sp:_spans){
        // split span into words to allow wrapping
        std::string word; auto emit=[&](std::string const& w){
            if(w.empty()) return;
            sf::Text t(*font,w,static_cast<unsigned int>(sp.size)); t.setFillColor(sp.color);
            sf::FloatRect tb=t.getLocalBounds();
            if(x+tb.size.x>r.position.x+r.size.x && x>r.position.x){ x=r.position.x; y+=line_h>0.f?line_h:sp.size*1.3f; line_h=0.f; }
            t.setPosition({x,y-tb.position.y});
            draw_buffer().add_text(renderer,layer,std::move(t));
            x+=tb.size.x; line_h=std::max(line_h,sp.size*1.3f);
        };
        for(char ch:sp.text){
            if(ch==' '){ emit(word); word.clear(); x+=sp.size*0.3f; }
            else if(ch=='\n'){ emit(word); word.clear(); x=r.position.x; y+=line_h>0.f?line_h:sp.size*1.3f; line_h=0.f; }
            else word+=ch;
        }
        emit(word);
    }
}
} // namespace titan::ui
