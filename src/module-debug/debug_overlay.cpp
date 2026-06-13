// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-debug/include/debug_overlay.hpp"
#include "module-debug/include/profiler.hpp"

#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "module-utils/include/math.hpp"
#include "module-utils/include/logger.hpp"

#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/VertexArray.hpp"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <memory>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::debug {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Debug_overlay
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void Debug_overlay::set_font   (std::string const font_resource)       { _font_name = std::move(font_resource); }
void Debug_overlay::set_visible(bool        const visible      )       { _visible = visible;                    }
bool Debug_overlay::is_visible (                               ) const { return _visible;                       }
void Debug_overlay::toggle     (                               )       { _visible = !_visible;                  }

// ----------------------------------------------------------------------------
void Debug_overlay::set_position   (sf::Vector2f const pos  ) { _position = pos;     }
void Debug_overlay::set_text_color (sf::Color    const color) { _text_color = color; }
void Debug_overlay::set_background (sf::Color    const color) { _bg_color = color;   }
void Debug_overlay::set_show_scopes(bool         const show ) { _show_scopes = show; }

// ----------------------------------------------------------------------------
void Debug_overlay::set_watch(
    std::string const& key,
    std::string const& value
    ) {

    for (auto& w : _watches) {

        if (w.first == key) {

            w.second = value;
            return;
        }
    }

    _watches.emplace_back(key, value);
}

// ----------------------------------------------------------------------------
void Debug_overlay::clear_watches() { _watches.clear(); }

// ----------------------------------------------------------------------------
void Debug_overlay::render(
    render::Renderer& renderer,
    Profiler const&   profiler
    ) {

    if (!_visible) { return; }

    // Track frame-time history for the graph.
    _frame_history.push_back(profiler.last_frame_ms());

    while (_frame_history.size() > MAX_HISTORY) { _frame_history.pop_front(); }

    std::shared_ptr<sf::Font> font;

    try {

        font = resources::Resource_manager::instance().get<sf::Font>(_font_name);
    } catch (std::exception const&) {

        LOG(Log_lvl::ERR) << "Failed to get font: " << _font_name;
        return;
    }

    if (!font) { return; }

    // ---- Compose the text block --------------------------------------------
    std::vector<std::string> lines;

    {
        char buf[64];
        std::snprintf(
            buf,
            sizeof(buf),
            "FPS %.0f  (%.2f ms)",
            static_cast<double>(profiler.fps()),
            static_cast<double>(profiler.average_frame_ms())
            );

        lines.emplace_back(buf);
    }

    if (_show_scopes) {

        auto scopes = profiler.scopes();

        std::sort(
            scopes.begin(),
            scopes.end(),
            [](auto const& a, auto const& b) { return a.second > b.second; }
            );

        for (auto const& [name, ms] : scopes) {

            char buf[96];
            std::snprintf(
                buf,
                sizeof(buf),
                "%-12s %.2f ms",
                name.c_str(),
                static_cast<double>(ms)
                );

            lines.emplace_back(buf);
        }
    }

    for (auto const& [key, value] : _watches) {

        lines.push_back(key + ": " + value);
    }

    // ---- Background panel ---------------------------------------------------
    unsigned int const font_size = 14u;
    float        const line_h    = 18.0f;
    float        const pad       = 6.0f;
    float              max_w     = 0.0f;

    // Measure widest line.
    for (auto const& line : lines) {

        sf::Text probe(*font, line, font_size);
        max_w = std::max(max_w, probe.getLocalBounds().size.x);
    }

    float const panel_w = max_w + pad * 2.0f;
    float const graph_h = 32.0f;
    float const panel_h = static_cast<float>(lines.size()) * line_h + pad * 2.0f + graph_h;

    // Clear keepalive stores up front, everything submitted this frame lives
    // in them until the next render() call (and thus past end_frame()).
    _text_keepalive.clear();
    _rect_keepalive.clear();

    {
        _rect_keepalive.emplace_back();
        sf::RectangleShape& bg = _rect_keepalive.back();
        bg.setPosition(_position);
        bg.setSize({ panel_w, panel_h });
        bg.setFillColor(_bg_color);
        renderer.submit(render::Render_layer::UI_OVERLAY, bg);
    }

    // ---- Text lines ---------------------------------------------------------
    float y = _position.y + pad;
    for (auto const& line : lines) {

        _text_keepalive.emplace_back(*font, line, font_size);
        sf::Text& t = _text_keepalive.back();
        t.setFillColor(_text_color);
        t.setPosition({ _position.x + pad, y });
        renderer.submit(render::Render_layer::UI_OVERLAY, t);
        y += line_h;
    }

    // ---- Frame-time graph ---------------------------------------------------
    if (!_frame_history.empty()) {

        float const gx = _position.x + pad;
        float const gy = y + 4.0f;
        float const gw = panel_w - pad * 2.0f;

        // Scale so 33ms (~30fps) fills the graph height.
        float const max_ms = 33.0f;

        sf::VertexArray& s_graph = _graph_keepalive;
        s_graph.setPrimitiveType(sf::PrimitiveType::LineStrip);
        s_graph.clear();

        for (std::size_t i = 0u; i < _frame_history.size(); ++i) {

            float const fx = gx + gw * (static_cast<float>(i) / static_cast<float>(MAX_HISTORY));
            float const norm = clamp01(_frame_history[i] / max_ms);
            float const fy = gy + graph_h - norm * graph_h;
            sf::Color const c = (_frame_history[i] > 16.7f) ? sf::Color(230, 120, 90) : sf::Color(120, 210, 130);
            s_graph.append(sf::Vertex{ .position = { fx, fy }, .color = c });
        }

        renderer.submit(render::Render_layer::UI_OVERLAY, s_graph);
    }
}

} // namespace titan::debug
