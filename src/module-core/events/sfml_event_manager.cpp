// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/events/include/sfml_event_manager.hpp"
#include "module-utils/include/logger.hpp"

#include "SFML/Window/Window.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class SFML_event_manager
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
SFML_event_manager::SFML_event_manager()
    : _callbacks         (  )
    , _held_keys         (  )
    , _held_mouse_buttons(  )
    , _next_callback_id  (1u)
{}

// ----------------------------------------------------------------------------
SFML_event_manager& SFML_event_manager::instance() {

    static SFML_event_manager instance;
    return instance;
}

// ----------------------------------------------------------------------------
void SFML_event_manager::process_events(
    sf::Window& window
    ) {

    // Process held keys and mouse buttons
    _process_held_keys();
    _process_held_mouse_buttons();

    // Poll SFML events
    while (auto event = window.pollEvent()) {

        SFML_event_data data{*event};

        if ([[maybe_unused]] auto* closed = event->getIf<sf::Event::Closed>()) {

            _dispatch(SFML_event_type::WINDOW_CLOSED, data);
        }
        else if ([[maybe_unused]] auto* resized = event->getIf<sf::Event::Resized>()) {

            _dispatch(SFML_event_type::WINDOW_RESIZED, data);
        }
        else if ([[maybe_unused]] auto* focus_lost = event->getIf<sf::Event::FocusLost>()) {

            _dispatch(SFML_event_type::WINDOW_LOST_FOCUS, data);
        }
        else if ([[maybe_unused]] auto* focus_gained = event->getIf<sf::Event::FocusGained>()) {

            _dispatch(SFML_event_type::WINDOW_GAINED_FOCUS, data);
        }
        else if ([[maybe_unused]] auto* text_entered = event->getIf<sf::Event::TextEntered>()) {

            _dispatch(SFML_event_type::WINDOW_TEXT_ENTERED, data);
        }
        else if ([[maybe_unused]] auto* key_pressed = event->getIf<sf::Event::KeyPressed>()) {

            _held_keys[key_pressed->code] = true;
            auto const type               = _key_to_press_event(key_pressed->code);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* key_released = event->getIf<sf::Event::KeyReleased>()) {

            _held_keys[key_released->code] = false;
            auto const type                = _key_to_release_event(key_released->code);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* wheel_scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {

            if (wheel_scrolled->delta > 0.0f) {

                _dispatch(SFML_event_type::MOUSE_WHEEL_UP, data);
            } else {

                _dispatch(SFML_event_type::MOUSE_WHEEL_DOWN, data);
            }
        }
        else if ([[maybe_unused]] auto* button_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {

            _held_mouse_buttons[button_pressed->button] = true;
            auto const type                             = _mouse_to_press_event(button_pressed->button);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* button_released = event->getIf<sf::Event::MouseButtonReleased>()) {

            _held_mouse_buttons[button_released->button] = false;
            auto const type                              = _mouse_to_release_event(button_released->button);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* mouse_moved = event->getIf<sf::Event::MouseMoved>()) {

            _dispatch(SFML_event_type::MOUSE_MOVED, data);
        }
        else if ([[maybe_unused]] auto* mouse_entered = event->getIf<sf::Event::MouseEntered>()) {

            _dispatch(SFML_event_type::MOUSE_ENTER_WINDOW, data);
        }
        else if ([[maybe_unused]] auto* mouse_left = event->getIf<sf::Event::MouseLeft>()) {

            _dispatch(SFML_event_type::MOUSE_LEAVE_WINDOW, data);
        }
        else if ([[maybe_unused]] auto* joy_pressed = event->getIf<sf::Event::JoystickButtonPressed>()) {

            _dispatch(SFML_event_type::JOYSTICK_BUTTON_PRESSED, data);
        }
        else if ([[maybe_unused]] auto* joy_released = event->getIf<sf::Event::JoystickButtonReleased>()) {

            _dispatch(SFML_event_type::JOYSTICK_BUTTON_RELEASED, data);
        }
        else if ([[maybe_unused]] auto* joy_moved = event->getIf<sf::Event::JoystickMoved>()) {

            _dispatch(SFML_event_type::JOYSTICK_MOVED, data);
        }
        else if ([[maybe_unused]] auto* joy_connected = event->getIf<sf::Event::JoystickConnected>()) {

            _dispatch(SFML_event_type::JOYSTICK_CONNECTED, data);
        }
        else if ([[maybe_unused]] auto* joy_disconnected = event->getIf<sf::Event::JoystickDisconnected>()) {

            _dispatch(SFML_event_type::JOYSTICK_DISCONNECTED, data);
        }
        else if ([[maybe_unused]] auto* touch_began = event->getIf<sf::Event::TouchBegan>()) {

            _dispatch(SFML_event_type::TOUCH_BEGAN, data);
        }
        else if ([[maybe_unused]] auto* touch_moved = event->getIf<sf::Event::TouchMoved>()) {

            _dispatch(SFML_event_type::TOUCH_MOVED, data);
        }
        else if ([[maybe_unused]] auto* touch_ended = event->getIf<sf::Event::TouchEnded>()) {

            _dispatch(SFML_event_type::TOUCH_ENDED, data);
        }
        else if ([[maybe_unused]] auto* sensor = event->getIf<sf::Event::SensorChanged>()) {

            _dispatch(SFML_event_type::SENSOR_CHANGED, data);
        }
    }
}

// ----------------------------------------------------------------------------
void SFML_event_manager::process_events(
    sf::RenderWindow& window
    ) {

    // Process held keys and mouse buttons
    _process_held_keys();
    _process_held_mouse_buttons();

    // Poll SFML events
    while (auto event = window.pollEvent()) {

        SFML_event_data data{*event};

        if ([[maybe_unused]] auto* closed = event->getIf<sf::Event::Closed>()) {

            _dispatch(SFML_event_type::WINDOW_CLOSED, data);
        }
        else if ([[maybe_unused]] auto* resized = event->getIf<sf::Event::Resized>()) {

            _dispatch(SFML_event_type::WINDOW_RESIZED, data);
        }
        else if ([[maybe_unused]] auto* focus_lost = event->getIf<sf::Event::FocusLost>()) {

            _dispatch(SFML_event_type::WINDOW_LOST_FOCUS, data);
        }
        else if ([[maybe_unused]] auto* focus_gained = event->getIf<sf::Event::FocusGained>()) {

            _dispatch(SFML_event_type::WINDOW_GAINED_FOCUS, data);
        }
        else if ([[maybe_unused]] auto* text_entered = event->getIf<sf::Event::TextEntered>()) {

            _dispatch(SFML_event_type::WINDOW_TEXT_ENTERED, data);
        }
        else if ([[maybe_unused]] auto* key_pressed = event->getIf<sf::Event::KeyPressed>()) {

            _held_keys[key_pressed->code] = true;
            auto const type               = _key_to_press_event(key_pressed->code);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* key_released = event->getIf<sf::Event::KeyReleased>()) {

            _held_keys[key_released->code] = false;
            auto const type                = _key_to_release_event(key_released->code);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* wheel_scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {

            if (wheel_scrolled->delta > 0.0f) {

                _dispatch(SFML_event_type::MOUSE_WHEEL_UP, data);
            } else {

                _dispatch(SFML_event_type::MOUSE_WHEEL_DOWN, data);
            }
        }
        else if ([[maybe_unused]] auto* button_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {

            _held_mouse_buttons[button_pressed->button] = true;
            auto const type                             = _mouse_to_press_event(button_pressed->button);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* button_released = event->getIf<sf::Event::MouseButtonReleased>()) {

            _held_mouse_buttons[button_released->button] = false;
            auto const type                              = _mouse_to_release_event(button_released->button);
            _dispatch(type, data);
        }
        else if ([[maybe_unused]] auto* mouse_moved = event->getIf<sf::Event::MouseMoved>()) {

            _dispatch(SFML_event_type::MOUSE_MOVED, data);
        }
        else if ([[maybe_unused]] auto* mouse_entered = event->getIf<sf::Event::MouseEntered>()) {

            _dispatch(SFML_event_type::MOUSE_ENTER_WINDOW, data);
        }
        else if ([[maybe_unused]] auto* mouse_left = event->getIf<sf::Event::MouseLeft>()) {

            _dispatch(SFML_event_type::MOUSE_LEAVE_WINDOW, data);
        }
        else if ([[maybe_unused]] auto* joy_pressed = event->getIf<sf::Event::JoystickButtonPressed>()) {

            _dispatch(SFML_event_type::JOYSTICK_BUTTON_PRESSED, data);
        }
        else if ([[maybe_unused]] auto* joy_released = event->getIf<sf::Event::JoystickButtonReleased>()) {

            _dispatch(SFML_event_type::JOYSTICK_BUTTON_RELEASED, data);
        }
        else if ([[maybe_unused]] auto* joy_moved = event->getIf<sf::Event::JoystickMoved>()) {

            _dispatch(SFML_event_type::JOYSTICK_MOVED, data);
        }
        else if ([[maybe_unused]] auto* joy_connected = event->getIf<sf::Event::JoystickConnected>()) {

            _dispatch(SFML_event_type::JOYSTICK_CONNECTED, data);
        }
        else if ([[maybe_unused]] auto* joy_disconnected = event->getIf<sf::Event::JoystickDisconnected>()) {

            _dispatch(SFML_event_type::JOYSTICK_DISCONNECTED, data);
        }
        else if ([[maybe_unused]] auto* touch_began = event->getIf<sf::Event::TouchBegan>()) {

            _dispatch(SFML_event_type::TOUCH_BEGAN, data);
        }
        else if ([[maybe_unused]] auto* touch_moved = event->getIf<sf::Event::TouchMoved>()) {

            _dispatch(SFML_event_type::TOUCH_MOVED, data);
        }
        else if ([[maybe_unused]] auto* touch_ended = event->getIf<sf::Event::TouchEnded>()) {

            _dispatch(SFML_event_type::TOUCH_ENDED, data);
        }
        else if ([[maybe_unused]] auto* sensor = event->getIf<sf::Event::SensorChanged>()) {

            _dispatch(SFML_event_type::SENSOR_CHANGED, data);
        }
    }
}

// ----------------------------------------------------------------------------
std::uint64_t SFML_event_manager::register_callback(
    SFML_event_type const  type,
    SFML_callback   const& callback,
    std::uint64_t   const  listener_id
    ) {

    std::uint64_t const callback_id = _next_callback_id++;
    _callbacks[type].push_back({callback_id, listener_id, callback});
    return callback_id;
}

// ----------------------------------------------------------------------------
void SFML_event_manager::deregister_callback(
    std::uint64_t const callback_id
    ) {

    for (auto& [type, entries] : _callbacks) {

        auto const it = std::remove_if(
            entries.begin(),
            entries.end(),
            [callback_id](Callback_entry const& entry) {

                return entry.callback_id == callback_id;
            }
            );

        entries.erase(it, entries.cend());
    }
}

// ----------------------------------------------------------------------------
void SFML_event_manager::deregister_listener(
    std::uint64_t const listener_id
    ) {

    for (auto& [type, entries] : _callbacks) {

        auto const it = std::remove_if(
            entries.begin(),
            entries.end(),
            [listener_id](Callback_entry const& entry) {

                return entry.listener_id == listener_id;
            }
            );

        entries.erase(it, entries.cend());
    }
}

// ----------------------------------------------------------------------------
void SFML_event_manager::deregister_type(
    SFML_event_type const type
    ) {

    _callbacks.erase(type);
}

// ----------------------------------------------------------------------------
void SFML_event_manager::clear() {

    _callbacks.clear();
    _held_keys.clear();
    _held_mouse_buttons.clear();
}

// ----------------------------------------------------------------------------
void SFML_event_manager::_dispatch(
    SFML_event_type const  type,
    SFML_event_data const& data
    ) {

    auto const it = _callbacks.find(type);

    if (it == _callbacks.cend()) {

        return;
    }

    // Copy vector in case callbacks modify the list
    auto const entries = it->second;

    for (auto const& entry : entries) {

        entry.callback(data);
    }
}

// ----------------------------------------------------------------------------
void SFML_event_manager::_process_held_keys() {

    for (auto const& [key, held] : _held_keys) {

        if (held) {

            auto const type = _key_to_held_event(key);

            // Create event with KeyPressed
            sf::Event const event = sf::Event::KeyPressed{
                .code       = key,
                .scancode   = sf::Keyboard::Scan::Unknown,
                .alt        = false,
                .control    = false,
                .shift      = false,
                .system     = false
                };

            SFML_event_data data{event};

            _dispatch(type, data);
        }
    }
}

// ----------------------------------------------------------------------------
void SFML_event_manager::_process_held_mouse_buttons() {

    for (auto const& [button, held] : _held_mouse_buttons) {

        if (held) {

            auto type = _mouse_to_held_event(button);

            // Create event with MouseButtonPressed
            sf::Event const event = sf::Event::MouseButtonPressed{
                .button     = button,
                .position   = {0, 0}
                };

            SFML_event_data data{event};

            _dispatch(type, data);
        }
    }
}

// ----------------------------------------------------------------------------
SFML_event_type SFML_event_manager::_key_to_press_event(
    sf::Keyboard::Key const key
    ) {

    switch (key) {

        case sf::Keyboard::Key::A: return SFML_event_type::KEYPRESS_A;
        case sf::Keyboard::Key::B: return SFML_event_type::KEYPRESS_B;
        case sf::Keyboard::Key::C: return SFML_event_type::KEYPRESS_C;
        case sf::Keyboard::Key::D: return SFML_event_type::KEYPRESS_D;
        case sf::Keyboard::Key::E: return SFML_event_type::KEYPRESS_E;
        case sf::Keyboard::Key::F: return SFML_event_type::KEYPRESS_F;
        case sf::Keyboard::Key::G: return SFML_event_type::KEYPRESS_G;
        case sf::Keyboard::Key::H: return SFML_event_type::KEYPRESS_H;
        case sf::Keyboard::Key::I: return SFML_event_type::KEYPRESS_I;
        case sf::Keyboard::Key::J: return SFML_event_type::KEYPRESS_J;
        case sf::Keyboard::Key::K: return SFML_event_type::KEYPRESS_K;
        case sf::Keyboard::Key::L: return SFML_event_type::KEYPRESS_L;
        case sf::Keyboard::Key::M: return SFML_event_type::KEYPRESS_M;
        case sf::Keyboard::Key::N: return SFML_event_type::KEYPRESS_N;
        case sf::Keyboard::Key::O: return SFML_event_type::KEYPRESS_O;
        case sf::Keyboard::Key::P: return SFML_event_type::KEYPRESS_P;
        case sf::Keyboard::Key::Q: return SFML_event_type::KEYPRESS_Q;
        case sf::Keyboard::Key::R: return SFML_event_type::KEYPRESS_R;
        case sf::Keyboard::Key::S: return SFML_event_type::KEYPRESS_S;
        case sf::Keyboard::Key::T: return SFML_event_type::KEYPRESS_T;
        case sf::Keyboard::Key::U: return SFML_event_type::KEYPRESS_U;
        case sf::Keyboard::Key::V: return SFML_event_type::KEYPRESS_V;
        case sf::Keyboard::Key::W: return SFML_event_type::KEYPRESS_W;
        case sf::Keyboard::Key::X: return SFML_event_type::KEYPRESS_X;
        case sf::Keyboard::Key::Y: return SFML_event_type::KEYPRESS_Y;
        case sf::Keyboard::Key::Z: return SFML_event_type::KEYPRESS_Z;

        case sf::Keyboard::Key::Num0: return SFML_event_type::KEYPRESS_NUM0;
        case sf::Keyboard::Key::Num1: return SFML_event_type::KEYPRESS_NUM1;
        case sf::Keyboard::Key::Num2: return SFML_event_type::KEYPRESS_NUM2;
        case sf::Keyboard::Key::Num3: return SFML_event_type::KEYPRESS_NUM3;
        case sf::Keyboard::Key::Num4: return SFML_event_type::KEYPRESS_NUM4;
        case sf::Keyboard::Key::Num5: return SFML_event_type::KEYPRESS_NUM5;
        case sf::Keyboard::Key::Num6: return SFML_event_type::KEYPRESS_NUM6;
        case sf::Keyboard::Key::Num7: return SFML_event_type::KEYPRESS_NUM7;
        case sf::Keyboard::Key::Num8: return SFML_event_type::KEYPRESS_NUM8;
        case sf::Keyboard::Key::Num9: return SFML_event_type::KEYPRESS_NUM9;

        case sf::Keyboard::Key::Escape: return SFML_event_type::KEYPRESS_ESCAPE;
        case sf::Keyboard::Key::LControl: return SFML_event_type::KEYPRESS_LCONTROL;
        case sf::Keyboard::Key::LShift: return SFML_event_type::KEYPRESS_LSHIFT;
        case sf::Keyboard::Key::LAlt: return SFML_event_type::KEYPRESS_LALT;
        case sf::Keyboard::Key::LSystem: return SFML_event_type::KEYPRESS_LSYSTEM;
        case sf::Keyboard::Key::RControl: return SFML_event_type::KEYPRESS_RCONTROL;
        case sf::Keyboard::Key::RShift: return SFML_event_type::KEYPRESS_RSHIFT;
        case sf::Keyboard::Key::RAlt: return SFML_event_type::KEYPRESS_RALT;
        case sf::Keyboard::Key::RSystem: return SFML_event_type::KEYPRESS_RSYSTEM;
        case sf::Keyboard::Key::Menu: return SFML_event_type::KEYPRESS_MENU;
        case sf::Keyboard::Key::LBracket: return SFML_event_type::KEYPRESS_LBRACKET;
        case sf::Keyboard::Key::RBracket: return SFML_event_type::KEYPRESS_RBRACKET;
        case sf::Keyboard::Key::Semicolon: return SFML_event_type::KEYPRESS_SEMICOLON;
        case sf::Keyboard::Key::Comma: return SFML_event_type::KEYPRESS_COMMA;
        case sf::Keyboard::Key::Period: return SFML_event_type::KEYPRESS_PERIOD;
        case sf::Keyboard::Key::Apostrophe: return SFML_event_type::KEYPRESS_QUOTE;
        case sf::Keyboard::Key::Slash: return SFML_event_type::KEYPRESS_SLASH;
        case sf::Keyboard::Key::Backslash: return SFML_event_type::KEYPRESS_BACKSLASH;
        case sf::Keyboard::Key::Grave: return SFML_event_type::KEYPRESS_TILDE;
        case sf::Keyboard::Key::Equal: return SFML_event_type::KEYPRESS_EQUAL;
        case sf::Keyboard::Key::Hyphen: return SFML_event_type::KEYPRESS_DASH;
        case sf::Keyboard::Key::Space: return SFML_event_type::KEYPRESS_SPACE;
        case sf::Keyboard::Key::Enter: return SFML_event_type::KEYPRESS_RETURN;
        case sf::Keyboard::Key::Backspace: return SFML_event_type::KEYPRESS_BACKSPACE;
        case sf::Keyboard::Key::Tab: return SFML_event_type::KEYPRESS_TAB;
        case sf::Keyboard::Key::PageUp: return SFML_event_type::KEYPRESS_PAGEUP;
        case sf::Keyboard::Key::PageDown: return SFML_event_type::KEYPRESS_PAGEDOWN;
        case sf::Keyboard::Key::End: return SFML_event_type::KEYPRESS_END;
        case sf::Keyboard::Key::Home: return SFML_event_type::KEYPRESS_HOME;
        case sf::Keyboard::Key::Insert: return SFML_event_type::KEYPRESS_INSERT;
        case sf::Keyboard::Key::Delete: return SFML_event_type::KEYPRESS_DELETE;

        case sf::Keyboard::Key::Left: return SFML_event_type::KEYPRESS_LEFT;
        case sf::Keyboard::Key::Right: return SFML_event_type::KEYPRESS_RIGHT;
        case sf::Keyboard::Key::Up: return SFML_event_type::KEYPRESS_UP;
        case sf::Keyboard::Key::Down: return SFML_event_type::KEYPRESS_DOWN;

        case sf::Keyboard::Key::Numpad0: return SFML_event_type::KEYPRESS_NUMPAD0;
        case sf::Keyboard::Key::Numpad1: return SFML_event_type::KEYPRESS_NUMPAD1;
        case sf::Keyboard::Key::Numpad2: return SFML_event_type::KEYPRESS_NUMPAD2;
        case sf::Keyboard::Key::Numpad3: return SFML_event_type::KEYPRESS_NUMPAD3;
        case sf::Keyboard::Key::Numpad4: return SFML_event_type::KEYPRESS_NUMPAD4;
        case sf::Keyboard::Key::Numpad5: return SFML_event_type::KEYPRESS_NUMPAD5;
        case sf::Keyboard::Key::Numpad6: return SFML_event_type::KEYPRESS_NUMPAD6;
        case sf::Keyboard::Key::Numpad7: return SFML_event_type::KEYPRESS_NUMPAD7;
        case sf::Keyboard::Key::Numpad8: return SFML_event_type::KEYPRESS_NUMPAD8;
        case sf::Keyboard::Key::Numpad9: return SFML_event_type::KEYPRESS_NUMPAD9;

        case sf::Keyboard::Key::F1: return SFML_event_type::KEYPRESS_F1;
        case sf::Keyboard::Key::F2: return SFML_event_type::KEYPRESS_F2;
        case sf::Keyboard::Key::F3: return SFML_event_type::KEYPRESS_F3;
        case sf::Keyboard::Key::F4: return SFML_event_type::KEYPRESS_F4;
        case sf::Keyboard::Key::F5: return SFML_event_type::KEYPRESS_F5;
        case sf::Keyboard::Key::F6: return SFML_event_type::KEYPRESS_F6;
        case sf::Keyboard::Key::F7: return SFML_event_type::KEYPRESS_F7;
        case sf::Keyboard::Key::F8: return SFML_event_type::KEYPRESS_F8;
        case sf::Keyboard::Key::F9: return SFML_event_type::KEYPRESS_F9;
        case sf::Keyboard::Key::F10: return SFML_event_type::KEYPRESS_F10;
        case sf::Keyboard::Key::F11: return SFML_event_type::KEYPRESS_F11;
        case sf::Keyboard::Key::F12: return SFML_event_type::KEYPRESS_F12;
        case sf::Keyboard::Key::F13: return SFML_event_type::KEYPRESS_F13;
        case sf::Keyboard::Key::F14: return SFML_event_type::KEYPRESS_F14;
        case sf::Keyboard::Key::F15: return SFML_event_type::KEYPRESS_F15;
        case sf::Keyboard::Key::Pause: return SFML_event_type::KEYPRESS_PAUSE;

        default: LOG(Log_lvl::WARN) << "UNKNOWN KEYBOARD EVENT"; return SFML_event_type::UNKNOWN;
    }
}

// ----------------------------------------------------------------------------
SFML_event_type SFML_event_manager::_key_to_held_event(
    sf::Keyboard::Key const key
    ) {

    switch (key) {

        case sf::Keyboard::Key::A: return SFML_event_type::KEYHELD_A;
        case sf::Keyboard::Key::B: return SFML_event_type::KEYHELD_B;
        case sf::Keyboard::Key::C: return SFML_event_type::KEYHELD_C;
        case sf::Keyboard::Key::D: return SFML_event_type::KEYHELD_D;
        case sf::Keyboard::Key::E: return SFML_event_type::KEYHELD_E;
        case sf::Keyboard::Key::F: return SFML_event_type::KEYHELD_F;
        case sf::Keyboard::Key::G: return SFML_event_type::KEYHELD_G;
        case sf::Keyboard::Key::H: return SFML_event_type::KEYHELD_H;
        case sf::Keyboard::Key::I: return SFML_event_type::KEYHELD_I;
        case sf::Keyboard::Key::J: return SFML_event_type::KEYHELD_J;
        case sf::Keyboard::Key::K: return SFML_event_type::KEYHELD_K;
        case sf::Keyboard::Key::L: return SFML_event_type::KEYHELD_L;
        case sf::Keyboard::Key::M: return SFML_event_type::KEYHELD_M;
        case sf::Keyboard::Key::N: return SFML_event_type::KEYHELD_N;
        case sf::Keyboard::Key::O: return SFML_event_type::KEYHELD_O;
        case sf::Keyboard::Key::P: return SFML_event_type::KEYHELD_P;
        case sf::Keyboard::Key::Q: return SFML_event_type::KEYHELD_Q;
        case sf::Keyboard::Key::R: return SFML_event_type::KEYHELD_R;
        case sf::Keyboard::Key::S: return SFML_event_type::KEYHELD_S;
        case sf::Keyboard::Key::T: return SFML_event_type::KEYHELD_T;
        case sf::Keyboard::Key::U: return SFML_event_type::KEYHELD_U;
        case sf::Keyboard::Key::V: return SFML_event_type::KEYHELD_V;
        case sf::Keyboard::Key::W: return SFML_event_type::KEYHELD_W;
        case sf::Keyboard::Key::X: return SFML_event_type::KEYHELD_X;
        case sf::Keyboard::Key::Y: return SFML_event_type::KEYHELD_Y;
        case sf::Keyboard::Key::Z: return SFML_event_type::KEYHELD_Z;

        case sf::Keyboard::Key::Num0: return SFML_event_type::KEYHELD_NUM0;
        case sf::Keyboard::Key::Num1: return SFML_event_type::KEYHELD_NUM1;
        case sf::Keyboard::Key::Num2: return SFML_event_type::KEYHELD_NUM2;
        case sf::Keyboard::Key::Num3: return SFML_event_type::KEYHELD_NUM3;
        case sf::Keyboard::Key::Num4: return SFML_event_type::KEYHELD_NUM4;
        case sf::Keyboard::Key::Num5: return SFML_event_type::KEYHELD_NUM5;
        case sf::Keyboard::Key::Num6: return SFML_event_type::KEYHELD_NUM6;
        case sf::Keyboard::Key::Num7: return SFML_event_type::KEYHELD_NUM7;
        case sf::Keyboard::Key::Num8: return SFML_event_type::KEYHELD_NUM8;
        case sf::Keyboard::Key::Num9: return SFML_event_type::KEYHELD_NUM9;

        case sf::Keyboard::Key::Escape: return SFML_event_type::KEYHELD_ESCAPE;
        case sf::Keyboard::Key::LControl: return SFML_event_type::KEYHELD_LCONTROL;
        case sf::Keyboard::Key::LShift: return SFML_event_type::KEYHELD_LSHIFT;
        case sf::Keyboard::Key::LAlt: return SFML_event_type::KEYHELD_LALT;
        case sf::Keyboard::Key::LSystem: return SFML_event_type::KEYHELD_LSYSTEM;
        case sf::Keyboard::Key::RControl: return SFML_event_type::KEYHELD_RCONTROL;
        case sf::Keyboard::Key::RShift: return SFML_event_type::KEYHELD_RSHIFT;
        case sf::Keyboard::Key::RAlt: return SFML_event_type::KEYHELD_RALT;
        case sf::Keyboard::Key::RSystem: return SFML_event_type::KEYHELD_RSYSTEM;
        case sf::Keyboard::Key::Menu: return SFML_event_type::KEYHELD_MENU;
        case sf::Keyboard::Key::LBracket: return SFML_event_type::KEYHELD_LBRACKET;
        case sf::Keyboard::Key::RBracket: return SFML_event_type::KEYHELD_RBRACKET;
        case sf::Keyboard::Key::Semicolon: return SFML_event_type::KEYHELD_SEMICOLON;
        case sf::Keyboard::Key::Comma: return SFML_event_type::KEYHELD_COMMA;
        case sf::Keyboard::Key::Period: return SFML_event_type::KEYHELD_PERIOD;
        case sf::Keyboard::Key::Apostrophe: return SFML_event_type::KEYHELD_QUOTE;
        case sf::Keyboard::Key::Slash: return SFML_event_type::KEYHELD_SLASH;
        case sf::Keyboard::Key::Backslash: return SFML_event_type::KEYHELD_BACKSLASH;
        case sf::Keyboard::Key::Grave: return SFML_event_type::KEYHELD_TILDE;
        case sf::Keyboard::Key::Equal: return SFML_event_type::KEYHELD_EQUAL;
        case sf::Keyboard::Key::Hyphen: return SFML_event_type::KEYHELD_DASH;
        case sf::Keyboard::Key::Space: return SFML_event_type::KEYHELD_SPACE;
        case sf::Keyboard::Key::Enter: return SFML_event_type::KEYHELD_RETURN;
        case sf::Keyboard::Key::Backspace: return SFML_event_type::KEYHELD_BACKSPACE;
        case sf::Keyboard::Key::Tab: return SFML_event_type::KEYHELD_TAB;
        case sf::Keyboard::Key::PageUp: return SFML_event_type::KEYHELD_PAGEUP;
        case sf::Keyboard::Key::PageDown: return SFML_event_type::KEYHELD_PAGEDOWN;
        case sf::Keyboard::Key::End: return SFML_event_type::KEYHELD_END;
        case sf::Keyboard::Key::Home: return SFML_event_type::KEYHELD_HOME;
        case sf::Keyboard::Key::Insert: return SFML_event_type::KEYHELD_INSERT;
        case sf::Keyboard::Key::Delete: return SFML_event_type::KEYHELD_DELETE;

        case sf::Keyboard::Key::Left: return SFML_event_type::KEYHELD_LEFT;
        case sf::Keyboard::Key::Right: return SFML_event_type::KEYHELD_RIGHT;
        case sf::Keyboard::Key::Up: return SFML_event_type::KEYHELD_UP;
        case sf::Keyboard::Key::Down: return SFML_event_type::KEYHELD_DOWN;

        case sf::Keyboard::Key::Numpad0: return SFML_event_type::KEYHELD_NUMPAD0;
        case sf::Keyboard::Key::Numpad1: return SFML_event_type::KEYHELD_NUMPAD1;
        case sf::Keyboard::Key::Numpad2: return SFML_event_type::KEYHELD_NUMPAD2;
        case sf::Keyboard::Key::Numpad3: return SFML_event_type::KEYHELD_NUMPAD3;
        case sf::Keyboard::Key::Numpad4: return SFML_event_type::KEYHELD_NUMPAD4;
        case sf::Keyboard::Key::Numpad5: return SFML_event_type::KEYHELD_NUMPAD5;
        case sf::Keyboard::Key::Numpad6: return SFML_event_type::KEYHELD_NUMPAD6;
        case sf::Keyboard::Key::Numpad7: return SFML_event_type::KEYHELD_NUMPAD7;
        case sf::Keyboard::Key::Numpad8: return SFML_event_type::KEYHELD_NUMPAD8;
        case sf::Keyboard::Key::Numpad9: return SFML_event_type::KEYHELD_NUMPAD9;

        case sf::Keyboard::Key::F1: return SFML_event_type::KEYHELD_F1;
        case sf::Keyboard::Key::F2: return SFML_event_type::KEYHELD_F2;
        case sf::Keyboard::Key::F3: return SFML_event_type::KEYHELD_F3;
        case sf::Keyboard::Key::F4: return SFML_event_type::KEYHELD_F4;
        case sf::Keyboard::Key::F5: return SFML_event_type::KEYHELD_F5;
        case sf::Keyboard::Key::F6: return SFML_event_type::KEYHELD_F6;
        case sf::Keyboard::Key::F7: return SFML_event_type::KEYHELD_F7;
        case sf::Keyboard::Key::F8: return SFML_event_type::KEYHELD_F8;
        case sf::Keyboard::Key::F9: return SFML_event_type::KEYHELD_F9;
        case sf::Keyboard::Key::F10: return SFML_event_type::KEYHELD_F10;
        case sf::Keyboard::Key::F11: return SFML_event_type::KEYHELD_F11;
        case sf::Keyboard::Key::F12: return SFML_event_type::KEYHELD_F12;
        case sf::Keyboard::Key::F13: return SFML_event_type::KEYHELD_F13;
        case sf::Keyboard::Key::F14: return SFML_event_type::KEYHELD_F14;
        case sf::Keyboard::Key::F15: return SFML_event_type::KEYHELD_F15;
        case sf::Keyboard::Key::Pause: return SFML_event_type::KEYHELD_PAUSE;

        default: LOG(Log_lvl::WARN) << "UNKNOWN KEYBOARD EVENT"; return SFML_event_type::UNKNOWN;
    }
}

// ----------------------------------------------------------------------------
SFML_event_type SFML_event_manager::_key_to_release_event(
    sf::Keyboard::Key const key
    ) {

    switch (key) {

        case sf::Keyboard::Key::A: return SFML_event_type::KEYRELEASE_A;
        case sf::Keyboard::Key::B: return SFML_event_type::KEYRELEASE_B;
        case sf::Keyboard::Key::C: return SFML_event_type::KEYRELEASE_C;
        case sf::Keyboard::Key::D: return SFML_event_type::KEYRELEASE_D;
        case sf::Keyboard::Key::E: return SFML_event_type::KEYRELEASE_E;
        case sf::Keyboard::Key::F: return SFML_event_type::KEYRELEASE_F;
        case sf::Keyboard::Key::G: return SFML_event_type::KEYRELEASE_G;
        case sf::Keyboard::Key::H: return SFML_event_type::KEYRELEASE_H;
        case sf::Keyboard::Key::I: return SFML_event_type::KEYRELEASE_I;
        case sf::Keyboard::Key::J: return SFML_event_type::KEYRELEASE_J;
        case sf::Keyboard::Key::K: return SFML_event_type::KEYRELEASE_K;
        case sf::Keyboard::Key::L: return SFML_event_type::KEYRELEASE_L;
        case sf::Keyboard::Key::M: return SFML_event_type::KEYRELEASE_M;
        case sf::Keyboard::Key::N: return SFML_event_type::KEYRELEASE_N;
        case sf::Keyboard::Key::O: return SFML_event_type::KEYRELEASE_O;
        case sf::Keyboard::Key::P: return SFML_event_type::KEYRELEASE_P;
        case sf::Keyboard::Key::Q: return SFML_event_type::KEYRELEASE_Q;
        case sf::Keyboard::Key::R: return SFML_event_type::KEYRELEASE_R;
        case sf::Keyboard::Key::S: return SFML_event_type::KEYRELEASE_S;
        case sf::Keyboard::Key::T: return SFML_event_type::KEYRELEASE_T;
        case sf::Keyboard::Key::U: return SFML_event_type::KEYRELEASE_U;
        case sf::Keyboard::Key::V: return SFML_event_type::KEYRELEASE_V;
        case sf::Keyboard::Key::W: return SFML_event_type::KEYRELEASE_W;
        case sf::Keyboard::Key::X: return SFML_event_type::KEYRELEASE_X;
        case sf::Keyboard::Key::Y: return SFML_event_type::KEYRELEASE_Y;
        case sf::Keyboard::Key::Z: return SFML_event_type::KEYRELEASE_Z;

        case sf::Keyboard::Key::Num0: return SFML_event_type::KEYRELEASE_NUM0;
        case sf::Keyboard::Key::Num1: return SFML_event_type::KEYRELEASE_NUM1;
        case sf::Keyboard::Key::Num2: return SFML_event_type::KEYRELEASE_NUM2;
        case sf::Keyboard::Key::Num3: return SFML_event_type::KEYRELEASE_NUM3;
        case sf::Keyboard::Key::Num4: return SFML_event_type::KEYRELEASE_NUM4;
        case sf::Keyboard::Key::Num5: return SFML_event_type::KEYRELEASE_NUM5;
        case sf::Keyboard::Key::Num6: return SFML_event_type::KEYRELEASE_NUM6;
        case sf::Keyboard::Key::Num7: return SFML_event_type::KEYRELEASE_NUM7;
        case sf::Keyboard::Key::Num8: return SFML_event_type::KEYRELEASE_NUM8;
        case sf::Keyboard::Key::Num9: return SFML_event_type::KEYRELEASE_NUM9;

        case sf::Keyboard::Key::Escape: return SFML_event_type::KEYRELEASE_ESCAPE;
        case sf::Keyboard::Key::LControl: return SFML_event_type::KEYRELEASE_LCONTROL;
        case sf::Keyboard::Key::LShift: return SFML_event_type::KEYRELEASE_LSHIFT;
        case sf::Keyboard::Key::LAlt: return SFML_event_type::KEYRELEASE_LALT;
        case sf::Keyboard::Key::LSystem: return SFML_event_type::KEYRELEASE_LSYSTEM;
        case sf::Keyboard::Key::RControl: return SFML_event_type::KEYRELEASE_RCONTROL;
        case sf::Keyboard::Key::RShift: return SFML_event_type::KEYRELEASE_RSHIFT;
        case sf::Keyboard::Key::RAlt: return SFML_event_type::KEYRELEASE_RALT;
        case sf::Keyboard::Key::RSystem: return SFML_event_type::KEYRELEASE_RSYSTEM;
        case sf::Keyboard::Key::Menu: return SFML_event_type::KEYRELEASE_MENU;
        case sf::Keyboard::Key::LBracket: return SFML_event_type::KEYRELEASE_LBRACKET;
        case sf::Keyboard::Key::RBracket: return SFML_event_type::KEYRELEASE_RBRACKET;
        case sf::Keyboard::Key::Semicolon: return SFML_event_type::KEYRELEASE_SEMICOLON;
        case sf::Keyboard::Key::Comma: return SFML_event_type::KEYRELEASE_COMMA;
        case sf::Keyboard::Key::Period: return SFML_event_type::KEYRELEASE_PERIOD;
        case sf::Keyboard::Key::Apostrophe: return SFML_event_type::KEYRELEASE_QUOTE;
        case sf::Keyboard::Key::Slash: return SFML_event_type::KEYRELEASE_SLASH;
        case sf::Keyboard::Key::Backslash: return SFML_event_type::KEYRELEASE_BACKSLASH;
        case sf::Keyboard::Key::Grave: return SFML_event_type::KEYRELEASE_TILDE;
        case sf::Keyboard::Key::Equal: return SFML_event_type::KEYRELEASE_EQUAL;
        case sf::Keyboard::Key::Hyphen: return SFML_event_type::KEYRELEASE_DASH;
        case sf::Keyboard::Key::Space: return SFML_event_type::KEYRELEASE_SPACE;
        case sf::Keyboard::Key::Enter: return SFML_event_type::KEYRELEASE_RETURN;
        case sf::Keyboard::Key::Backspace: return SFML_event_type::KEYRELEASE_BACKSPACE;
        case sf::Keyboard::Key::Tab: return SFML_event_type::KEYRELEASE_TAB;
        case sf::Keyboard::Key::PageUp: return SFML_event_type::KEYRELEASE_PAGEUP;
        case sf::Keyboard::Key::PageDown: return SFML_event_type::KEYRELEASE_PAGEDOWN;
        case sf::Keyboard::Key::End: return SFML_event_type::KEYRELEASE_END;
        case sf::Keyboard::Key::Home: return SFML_event_type::KEYRELEASE_HOME;
        case sf::Keyboard::Key::Insert: return SFML_event_type::KEYRELEASE_INSERT;
        case sf::Keyboard::Key::Delete: return SFML_event_type::KEYRELEASE_DELETE;

        case sf::Keyboard::Key::Left: return SFML_event_type::KEYRELEASE_LEFT;
        case sf::Keyboard::Key::Right: return SFML_event_type::KEYRELEASE_RIGHT;
        case sf::Keyboard::Key::Up: return SFML_event_type::KEYRELEASE_UP;
        case sf::Keyboard::Key::Down: return SFML_event_type::KEYRELEASE_DOWN;

        case sf::Keyboard::Key::Numpad0: return SFML_event_type::KEYRELEASE_NUMPAD0;
        case sf::Keyboard::Key::Numpad1: return SFML_event_type::KEYRELEASE_NUMPAD1;
        case sf::Keyboard::Key::Numpad2: return SFML_event_type::KEYRELEASE_NUMPAD2;
        case sf::Keyboard::Key::Numpad3: return SFML_event_type::KEYRELEASE_NUMPAD3;
        case sf::Keyboard::Key::Numpad4: return SFML_event_type::KEYRELEASE_NUMPAD4;
        case sf::Keyboard::Key::Numpad5: return SFML_event_type::KEYRELEASE_NUMPAD5;
        case sf::Keyboard::Key::Numpad6: return SFML_event_type::KEYRELEASE_NUMPAD6;
        case sf::Keyboard::Key::Numpad7: return SFML_event_type::KEYRELEASE_NUMPAD7;
        case sf::Keyboard::Key::Numpad8: return SFML_event_type::KEYRELEASE_NUMPAD8;
        case sf::Keyboard::Key::Numpad9: return SFML_event_type::KEYRELEASE_NUMPAD9;

        case sf::Keyboard::Key::F1: return SFML_event_type::KEYRELEASE_F1;
        case sf::Keyboard::Key::F2: return SFML_event_type::KEYRELEASE_F2;
        case sf::Keyboard::Key::F3: return SFML_event_type::KEYRELEASE_F3;
        case sf::Keyboard::Key::F4: return SFML_event_type::KEYRELEASE_F4;
        case sf::Keyboard::Key::F5: return SFML_event_type::KEYRELEASE_F5;
        case sf::Keyboard::Key::F6: return SFML_event_type::KEYRELEASE_F6;
        case sf::Keyboard::Key::F7: return SFML_event_type::KEYRELEASE_F7;
        case sf::Keyboard::Key::F8: return SFML_event_type::KEYRELEASE_F8;
        case sf::Keyboard::Key::F9: return SFML_event_type::KEYRELEASE_F9;
        case sf::Keyboard::Key::F10: return SFML_event_type::KEYRELEASE_F10;
        case sf::Keyboard::Key::F11: return SFML_event_type::KEYRELEASE_F11;
        case sf::Keyboard::Key::F12: return SFML_event_type::KEYRELEASE_F12;
        case sf::Keyboard::Key::F13: return SFML_event_type::KEYRELEASE_F13;
        case sf::Keyboard::Key::F14: return SFML_event_type::KEYRELEASE_F14;
        case sf::Keyboard::Key::F15: return SFML_event_type::KEYRELEASE_F15;
        case sf::Keyboard::Key::Pause: return SFML_event_type::KEYRELEASE_PAUSE;

        default: LOG(Log_lvl::WARN) << "UNKNOWN KEYBOARD EVENT"; return SFML_event_type::UNKNOWN;
    }
}

// ----------------------------------------------------------------------------
SFML_event_type SFML_event_manager::_mouse_to_press_event(
    sf::Mouse::Button const button
    ) {

    switch (button) {

        case sf::Mouse::Button::Left: return SFML_event_type::MOUSE_BUTTON_LEFT_PRESS;
        case sf::Mouse::Button::Right: return SFML_event_type::MOUSE_BUTTON_RIGHT_PRESS;
        case sf::Mouse::Button::Middle: return SFML_event_type::MOUSE_BUTTON_MIDDLE_PRESS;
        case sf::Mouse::Button::Extra1: return SFML_event_type::MOUSE_BUTTON_EXTRA_1_PRESS;
        case sf::Mouse::Button::Extra2: return SFML_event_type::MOUSE_BUTTON_EXTRA_2_PRESS;

        default: LOG(Log_lvl::WARN) << "UNKNOWN MOUSE EVENT"; return SFML_event_type::UNKNOWN;
    }
}

// ----------------------------------------------------------------------------
SFML_event_type SFML_event_manager::_mouse_to_held_event(
    sf::Mouse::Button const button
    ) {

    switch (button) {

        case sf::Mouse::Button::Left: return SFML_event_type::MOUSE_BUTTON_LEFT_HELD;
        case sf::Mouse::Button::Right: return SFML_event_type::MOUSE_BUTTON_RIGHT_HELD;
        case sf::Mouse::Button::Middle: return SFML_event_type::MOUSE_BUTTON_MIDDLE_HELD;
        case sf::Mouse::Button::Extra1: return SFML_event_type::MOUSE_BUTTON_EXTRA_1_HELD;
        case sf::Mouse::Button::Extra2: return SFML_event_type::MOUSE_BUTTON_EXTRA_2_HELD;

        default: LOG(Log_lvl::WARN) << "UNKNOWN MOUSE EVENT"; return SFML_event_type::UNKNOWN;
    }
}

// ----------------------------------------------------------------------------
SFML_event_type SFML_event_manager::_mouse_to_release_event(
    sf::Mouse::Button const button
    ) {

    switch (button) {

        case sf::Mouse::Button::Left: return SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE;
        case sf::Mouse::Button::Right: return SFML_event_type::MOUSE_BUTTON_RIGHT_RELEASE;
        case sf::Mouse::Button::Middle: return SFML_event_type::MOUSE_BUTTON_MIDDLE_RELEASE;
        case sf::Mouse::Button::Extra1: return SFML_event_type::MOUSE_BUTTON_EXTRA_1_RELEASE;
        case sf::Mouse::Button::Extra2: return SFML_event_type::MOUSE_BUTTON_EXTRA_2_RELEASE;

        default: LOG(Log_lvl::WARN) << "UNKNOWN MOUSE EVENT"; return SFML_event_type::UNKNOWN;
    }
}

} // namespace titan::events
