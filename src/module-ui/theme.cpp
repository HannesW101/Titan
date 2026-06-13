// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/theme.hpp"

#include "module-utils/include/logger.hpp"

#include <array>
#include <stdexcept>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Theme
// ============================================================================

void Theme::set(std::string const& key, Visual_set vs) {
    _styles[key] = std::move(vs);
}

Visual_set const& Theme::get(std::string const& key) const {
    auto it = _styles.find(key);
    return (it == _styles.end()) ? _empty : it->second;
}

bool Theme::has(std::string const& key) const {
    return _styles.find(key) != _styles.end();
}

// ----------------------------------------------------------------------------
sf::Color Theme::parse_color(std::string const& hex) {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') { h = h.substr(1u); }
    if (h.size() != 6u && h.size() != 8u) { return sf::Color::White; }
    try {
        auto byte = [&](std::size_t p){ return static_cast<std::uint8_t>(std::stoul(h.substr(p,2u),nullptr,16)); };
        std::uint8_t a = (h.size()==8u) ? byte(6u) : 255u;
        return { byte(0u), byte(2u), byte(4u), a };
    } catch (std::exception const&) { return sf::Color::White; }
}

// ----------------------------------------------------------------------------
void Theme::load_from_config(core::Config const& config) {

    static constexpr std::array<std::pair<char const*, Widget_state>, 5> states{{
        { "normal",   Widget_state::NORMAL },
        { "hovered",  Widget_state::HOVERED },
        { "pressed",  Widget_state::PRESSED },
        { "focused",  Widget_state::FOCUSED },
        { "disabled", Widget_state::DISABLED },
    }};

    for (auto const& section : config.sections()) {
        if (section.size() < 4u || section.substr(0u, 3u) != "ui.") { continue; }
        std::string const key = section.substr(3u);

        Visual_set vs;

        for (auto const& [prefix, state] : states) {
            std::string const p = prefix;

            // Texture / nine-slice take priority if present
            if (auto tex = config.get<std::string>(section, p + "_nine")) {
                float b = 8.0f;
                if (auto bb = config.get<float>(section, p + "_nine_border")) { b = *bb; }
                vs.set_visual(state, Visual::nine_slice(*tex, b, b, b, b));
                continue;
            }
            if (auto tex = config.get<std::string>(section, p + "_texture")) {
                vs.set_visual(state, Visual::texture(*tex));
                continue;
            }

            // Otherwise color
            if (auto bg = config.get<std::string>(section, p + "_bg")) {
                sf::Color fill = parse_color(*bg);
                sf::Color border = sf::Color::Transparent;
                float thick = 0.0f;
                if (auto bc = config.get<std::string>(section, p + "_border")) { border = parse_color(*bc); }
                if (auto bt = config.get<float>(section, p + "_border_thickness")) { thick = *bt; }
                vs.set_visual(state, Visual::color(fill, border, thick));
            }

            // Text appearance
            Text_appearance ta;
            bool has_text = false;
            if (auto tc = config.get<std::string>(section, p + "_text")) { ta.color = parse_color(*tc); has_text = true; }
            if (auto ts = config.get<float>(section, p + "_text_size"))  { ta.size = *ts; has_text = true; }
            if (auto tf = config.get<std::string>(section, p + "_font")) { ta.font_name = *tf; has_text = true; }
            if (has_text) { vs.set_text(state, ta); }
        }

        if (auto t = config.get<float>(section, "transition")) { vs.transition_seconds = *t; }

        _styles[key] = std::move(vs);
    }
}

// ============================================================================
// Built-in dark theme
// ============================================================================

Theme Theme::dark() {

    Theme t;

    auto text = [](float size, sf::Color c){ return Text_appearance{ "default_font", size, c }; };

    sf::Color const white   (235, 235, 240);
    sf::Color const grey    (160, 160, 175);
    sf::Color const dark1   ( 26,  26,  40);
    sf::Color const dark2   ( 34,  34,  52);
    sf::Color const accent  ( 80, 140, 230);
    sf::Color const accent_h(100, 160, 250);
    sf::Color const accent_p( 60, 115, 200);
    sf::Color const good    ( 80, 190, 120);
    sf::Color const border  ( 60,  60,  90);

    // Label
    {
        Visual_set vs;
        vs.normal(Visual::none());
        vs.set_text(Widget_state::NORMAL, text(14, white));
        t.set("label", vs);
    }
    // Button
    {
        Visual_set vs;
        vs.normal (Visual::color(accent,   border, 1.0f));
        vs.hovered(Visual::color(accent_h, border, 1.0f));
        vs.pressed(Visual::color(accent_p, border, 1.0f));
        vs.disabled(Visual::color(dark2, border, 1.0f));
        vs.set_text(Widget_state::NORMAL,   text(15, white));
        vs.set_text(Widget_state::DISABLED, text(15, grey));
        t.set("button", vs);
    }
    // Panel
    {
        Visual_set vs;
        vs.normal(Visual::color(dark1, border, 1.5f));
        t.set("panel", vs);
    }
    // Window
    {
        Visual_set vs;
        vs.normal(Visual::color(dark1, border, 1.5f));
        vs.set_text(Widget_state::NORMAL, text(15, white));
        t.set("window", vs);
        Visual_set tb;
        tb.normal(Visual::color(dark2, border, 0.0f));
        tb.set_text(Widget_state::NORMAL, text(15, white));
        t.set("window_titlebar", tb);
    }
    // Text field
    {
        Visual_set vs;
        vs.normal (Visual::color(sf::Color(14,14,24), border, 1.0f));
        vs.focused(Visual::color(sf::Color(14,14,24), accent, 1.5f));
        vs.set_text(Widget_state::NORMAL, text(14, white));
        t.set("text_field", vs);
    }
    // Checkbox / radio box
    {
        Visual_set vs;
        vs.normal (Visual::color(dark2, border, 1.5f));
        vs.hovered(Visual::color(sf::Color(44,44,66), accent, 1.5f));
        vs.set_text(Widget_state::NORMAL, text(14, white));
        t.set("checkbox", vs);
        t.set("radio", vs);
    }
    // Toggle
    {
        Visual_set vs;
        vs.normal(Visual::color(sf::Color(60,60,80)));
        vs.pressed(Visual::color(good));
        t.set("toggle", vs);
    }
    // Slider
    {
        Visual_set track; track.normal(Visual::color(dark2, border, 1.0f)); t.set("slider_track", track);
        Visual_set fill;  fill.normal(Visual::color(accent));               t.set("slider_fill", fill);
        Visual_set knob;  knob.normal(Visual::color(white, border, 1.0f));
                          knob.hovered(Visual::color(accent_h, border, 1.0f)); t.set("slider_knob", knob);
    }
    // Progress bar
    {
        Visual_set bg;  bg.normal(Visual::color(dark2, border, 1.0f));  t.set("progress_bg", bg);
        Visual_set fl;  fl.normal(Visual::color(good));                 t.set("progress_fill", fl);
        Visual_set vs;  vs.set_text(Widget_state::NORMAL, text(12, white)); t.set("progress", vs);
    }
    // Dropdown / list / tree / table / menu
    {
        Visual_set vs;
        vs.normal (Visual::color(dark2, border, 1.0f));
        vs.hovered(Visual::color(sf::Color(44,44,66), accent, 1.0f));
        vs.set_text(Widget_state::NORMAL, text(14, white));
        t.set("dropdown", vs);
        t.set("list_box", vs);
        t.set("tree_view", vs);
        t.set("table", vs);
        t.set("menu", vs);
    }
    {
        Visual_set item;
        item.normal (Visual::none());
        item.hovered(Visual::color(accent));
        item.pressed(Visual::color(accent_p));
        item.set_text(Widget_state::NORMAL, text(14, white));
        t.set("item", item);          // generic selectable row
        t.set("menu_item", item);
    }
    // Tab
    {
        Visual_set tab;
        tab.normal (Visual::color(dark2, border, 1.0f));
        tab.hovered(Visual::color(sf::Color(44,44,66), border, 1.0f));
        tab.pressed(Visual::color(accent, border, 1.0f));   // active tab uses pressed
        tab.set_text(Widget_state::NORMAL, text(14, white));
        t.set("tab", tab);
    }
    // Tooltip / toast
    {
        Visual_set vs;
        vs.normal(Visual::color(sf::Color(0,0,0,225), sf::Color(90,90,120), 1.0f));
        vs.set_text(Widget_state::NORMAL, text(12, white));
        t.set("tooltip", vs);
        t.set("toast", vs);
    }
    // Separator
    {
        Visual_set vs; vs.normal(Visual::color(border)); t.set("separator", vs);
    }
    // Scroll thumb
    {
        Visual_set vs; vs.normal(Visual::color(accent)); vs.hovered(Visual::color(accent_h));
        t.set("scroll_thumb", vs);
    }

    return t;
}

} // namespace titan::ui
