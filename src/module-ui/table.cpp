// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/table.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Table::Table(std::string const& name):Widget(name){
    auto& sfml=events::SFML_event_manager::instance();
    _click=sfml.register_callback(events::SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE,
        [this](events::SFML_event_data const&){
            if(!is_enabled()||!is_hovered()) return;
            sf::Vector2f m=mouse_position(); sf::FloatRect r=rect();
            float dy=m.y-r.position.y-_row_h; // skip header
            if(dy<0.f) return;
            std::size_t idx=(std::size_t)(dy/_row_h);
            if(idx<_rows.size() && _on_row_clicked) _on_row_clicked(idx);
        }, listener_id());
}
void Table::set_columns(std::vector<std::string> h){_headers=std::move(h);}
void Table::set_column_widths(std::vector<float> w){_widths=std::move(w);}
void Table::clear_rows(){_rows.clear();}
void Table::add_row(std::vector<std::string> c){_rows.push_back(std::move(c));}
std::size_t Table::row_count() const { return _rows.size(); }
void Table::set_row_height(float h){_row_h=h;}
void Table::set_on_row_clicked(std::function<void(std::size_t)> cb){_on_row_clicked=std::move(cb);}
void Table::on_render(render::Renderer& renderer){
    sf::FloatRect r=rect();
    auto const& ta=resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try{ font=resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name);}catch(std::exception const&){return;}
    render::Render_layer layer = on_overlay()?render::Render_layer::UI_OVERLAY:render::Render_layer::UI;
    auto col_x=[&](std::size_t i){ float x=r.position.x; for(std::size_t k=0;k<i&&k<_widths.size();++k) x+=_widths[k]; return x; };
    // header
    sf::RectangleShape hdr; hdr.setPosition(r.position); hdr.setSize({r.size.x,_row_h}); hdr.setFillColor(sf::Color(34,34,52));
    draw_buffer().add_rect(renderer,layer,std::move(hdr));
    for(std::size_t i=0;i<_headers.size();++i){
        sf::Text t(*font,_headers[i],static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
        sf::FloatRect tb=t.getLocalBounds();
        t.setPosition({col_x(i)+6.f,r.position.y+(_row_h-tb.size.y)*0.5f-tb.position.y});
        draw_buffer().add_text(renderer,layer,std::move(t));
    }
    // rows
    for(std::size_t row=0;row<_rows.size();++row){
        float y=r.position.y+_row_h*(float)(row+1u);
        if(y>r.position.y+r.size.y) break;
        for(std::size_t i=0;i<_rows[row].size();++i){
            sf::Text t(*font,_rows[row][i],static_cast<unsigned int>(ta.size)); t.setFillColor(ta.color);
            sf::FloatRect tb=t.getLocalBounds();
            t.setPosition({col_x(i)+6.f,y+(_row_h-tb.size.y)*0.5f-tb.position.y});
            draw_buffer().add_text(renderer,layer,std::move(t));
        }
    }
}
} // namespace titan::ui
