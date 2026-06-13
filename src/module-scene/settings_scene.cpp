// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-scene/include/settings_scene.hpp"
#include "module-scene/include/scene_manager.hpp"

#include "module-app/include/application.hpp"

#include "module-ui/include/panel.hpp"
#include "module-ui/include/boxes.hpp"
#include "module-ui/include/label.hpp"
#include "module-ui/include/slider.hpp"
#include "module-ui/include/button.hpp"

#include "module-audio/include/audio_system.hpp"
#include "module-audio/include/audio_bus.hpp"

#include "module-core/config/include/config.hpp"
#include "module-core/config/include/config_manager.hpp"

#include <array>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::scene {

    // ----------------------------------------------------------------------------
Settings_scene::Settings_scene(std::string name)
    : Scene(std::move(name))
    {

    // Overlay: render the scene below (e.g. paused game) but freeze its updates.
    set_renders_below(true);
    set_updates_below(false);
}

// ----------------------------------------------------------------------------
void Settings_scene::set_config_name(
    std::string config_name
    ) {

    _config_name = std::move(config_name);
}

// ----------------------------------------------------------------------------
void Settings_scene::on_enter() {

    _ui = std::make_unique<ui::UI_system>(application());
    _build_ui();
}

// ----------------------------------------------------------------------------
void Settings_scene::_build_ui() {

    audio::Audio_system& sound = application().audio();
    ui::UI_manager&      gui   = _ui->manager();

    auto panel = std::make_unique<ui::Panel>("settings_panel");
    panel->set_anchor(ui::UI_anchor::MIDDLE_CENTER);
    panel->set_size(ui::UI_length::px(440.0f), ui::UI_length::px(420.0f));
    panel->set_padding(20.0f);

    auto vbox = std::make_unique<ui::V_box>("settings_vbox");
    vbox->set_size(ui::UI_length::pct(1.0f), ui::UI_length::pct(1.0f));
    vbox->set_spacing(14.0f);

    auto title = std::make_unique<ui::Label>("title");
    title->set_text("Audio Settings");
    title->set_align(ui::Text_align::CENTER);
    title->set_size(ui::UI_length::pct(1.0f), ui::UI_length::px(32.0f));
    vbox->add_child(std::move(title));

    // One labeled slider per bus, bound live to the Audio_system.
    struct Bus_row { char const* label; audio::Audio_bus bus; };
    std::array<Bus_row, 4> const rows = {{
        { "Master", audio::Audio_bus::MASTER },
        { "Music",  audio::Audio_bus::MUSIC  },
        { "SFX",    audio::Audio_bus::SFX    },
        { "UI",     audio::Audio_bus::UI     },
        }};

    for (auto const& row : rows) {

        auto label = std::make_unique<ui::Label>(std::string("lbl_") + row.label);
        label->set_text(row.label);
        label->set_size(ui::UI_length::pct(1.0f), ui::UI_length::px(20.0f));
        vbox->add_child(std::move(label));

        auto slider = std::make_unique<ui::Slider>(std::string("sld_") + row.label);
        slider->set_range(0.0f, 1.0f);
        slider->set_value(sound.bus_volume(row.bus));
        slider->set_size(ui::UI_length::pct(1.0f), ui::UI_length::px(26.0f));

        audio::Audio_bus const bus = row.bus; // capture by value
        slider->set_on_changed([&sound, bus](float v) {
            sound.set_bus_volume(bus, v); // live update
            });
        vbox->add_child(std::move(slider));
    }

    auto back = std::make_unique<ui::Button>("back");
    back->set_text("Back");
    back->set_size(ui::UI_length::pct(1.0f), ui::UI_length::px(40.0f));
    back->set_on_click([this] { _save_and_close(); });
    vbox->add_child(std::move(back));

    panel->add_child(std::move(vbox));
    gui.add(std::move(panel));
}

// ----------------------------------------------------------------------------
void Settings_scene::_save_and_close() {

    // Persist the current mix into the named config, then pop the overlay.
    core::Config& cfg = core::Config_manager::instance().get(_config_name);
    application().audio().save_mix(cfg);

    scenes().pop();
}

// ----------------------------------------------------------------------------
void Settings_scene::update(
    float dt
    ) {

    if (_ui) { _ui->update(dt); }
}

// ----------------------------------------------------------------------------
void Settings_scene::render(
    render::Renderer& renderer
    ) {

    static_cast<void>(renderer); // UI_system renders via the shared renderer

    if (_ui) { _ui->render(); }
}

} // namespace titan::scene
