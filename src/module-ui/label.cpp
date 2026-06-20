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

float Label::content_height() const {
    if (!_wrap || _lines.empty()) { return rect().size.y; }
    Text_appearance const& ta = resolved_text_style();
    return static_cast<float>(_lines.size()) * ta.size * 1.9f;
}

void Label::set_text(std::string const& t) { _text = t; _cached_text = ""; }
std::string const& Label::text() const { return _text; }
void Label::set_align(Text_align a) { _align = a; }
void Label::set_wrap(bool w) { _wrap = w; }

void Label::_build_lines(sf::Font const& font, unsigned int csize, float wrap_width) {
    _lines.clear();
    // Split on explicit newlines first, then word-wrap each paragraph.
    std::istringstream par_stream(_text);
    std::string paragraph;
    while (std::getline(par_stream, paragraph)) {
        if (paragraph.empty()) { _lines.push_back(""); continue; }
        std::istringstream word_stream(paragraph);
        std::string word, line;
        while (word_stream >> word) {
            std::string test = line.empty() ? word : line + " " + word;
            sf::Text probe(font, test, csize);
            if (probe.getLocalBounds().size.x > wrap_width && !line.empty()) {
                _lines.push_back(line);
                line = word;
            } else {
                line = test;
            }
        }
        if (!line.empty()) { _lines.push_back(line); }
    }
}

void Label::on_layout() {
    if (!_wrap) { return; }
    float const w = rect().size.x;
    if (w == _cached_width && _text == _cached_text) { return; }
    _cached_width = w;
    _cached_text  = _text;

    Text_appearance const& ta = resolved_text_style();
    std::shared_ptr<sf::Font> font;
    try { font = resource_manager().get<sf::Font>(ta.font_name.empty() ? "default_font" : ta.font_name); }
    catch (...) { return; }

    float const s = px_scale();
    unsigned int const csize = static_cast<unsigned int>(ta.size * s);
    float const wrap_w = w - ta.outline_thickness * 2.0f * s;
    _build_lines(*font, csize, wrap_w > 0.0f ? wrap_w : w);
}

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

    // Use cached wrapped lines or split on newlines for non-wrap mode.
    std::vector<std::string> fresh;
    std::vector<std::string> const* lines_ptr = &_lines;
    if (!_wrap) {
        std::istringstream iss(_text); std::string ln;
        while (std::getline(iss, ln)) { fresh.push_back(ln); }
        lines_ptr = &fresh;
    }

    float const line_h = ta.size * 1.9f;
    float y = r.position.y;
    for (auto const& line : *lines_ptr) {
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
