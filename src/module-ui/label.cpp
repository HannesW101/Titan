// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/label.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

#include <sstream>
#include <vector>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Label::Label(std::string const& name) : Widget(name) {}
void Label::set_text(std::string const& t) { _text = t; }
std::string const& Label::text() const { return _text; }
void Label::set_align(Text_align a) { _align = a; }
void Label::set_wrap(bool w) { _wrap = w; }

void Label::on_render(render::Renderer& renderer) {
    if (_text.empty()) { return; }
    Text_appearance const& ta = resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try { font = resource_manager().get<sf::Font>(ta.font_name.empty() ? "default_font" : ta.font_name); }
    catch (std::exception const&) { return; }

    sf::FloatRect const r = rect();
    render::Render_layer const layer = on_overlay() ? render::Render_layer::UI_OVERLAY : render::Render_layer::UI;
    float const s = px_scale();
    unsigned int const csize = static_cast<unsigned int>(ta.size * s);

    std::vector<std::string> lines;
    if (_wrap) {
        std::istringstream iss(_text); std::string word, line;
        while (iss >> word) {
            std::string test = line.empty() ? word : line + " " + word;
            sf::Text probe(*font, test, csize);
            if (probe.getLocalBounds().size.x > r.size.x && !line.empty()) { lines.push_back(line); line = word; }
            else { line = test; }
        }
        if (!line.empty()) { lines.push_back(line); }
    } else {
        std::istringstream iss(_text); std::string line;
        while (std::getline(iss, line)) { lines.push_back(line); }
    }

    float const line_h = ta.size * 1.25f;
    float y = r.position.y;
    for (auto const& line : lines) {
        sf::Text t(*font, line, csize);
        t.setFillColor(ta.color);
        t.setOutlineColor(ta.outline_color);
        t.setOutlineThickness(ta.outline_thickness * s);
        sf::FloatRect const tb = t.getLocalBounds();
        float x = r.position.x;
        switch (_align) {
            case Text_align::LEFT:   x += 0.0f - tb.position.x; break;
            case Text_align::CENTER: x += (r.size.x - tb.size.x) * 0.5f - tb.position.x; break;
            case Text_align::RIGHT:  x += r.size.x - tb.size.x - tb.position.x; break;
            default:                 x += 0.0f - tb.position.x; break;
        }
        t.setPosition({ x, y - tb.position.y });
        draw_buffer().add_text(renderer, layer, std::move(t));
        y += line_h;
    }
}

} // namespace titan::ui
