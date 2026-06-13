#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <ostream>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::events {

// ============================================================================
// Enums
// ----------------------------------------------------------------------------

/**
 * @brief SFML window and input event types with specific key/button granularity.
 */
enum class SFML_event_type : std::uint16_t {

    // Unknown
    UNKNOWN,

    // Window events
    WINDOW_CLOSED,
    WINDOW_RESIZED,
    WINDOW_LOST_FOCUS,
    WINDOW_GAINED_FOCUS,
    WINDOW_TEXT_ENTERED,

    // Mouse movement
    MOUSE_MOVED,
    MOUSE_ENTER_WINDOW,
    MOUSE_LEAVE_WINDOW,

    // Mouse wheel
    MOUSE_WHEEL_UP,
    MOUSE_WHEEL_DOWN,

    // Mouse button pressed
    MOUSE_BUTTON_LEFT_PRESS,
    MOUSE_BUTTON_RIGHT_PRESS,
    MOUSE_BUTTON_MIDDLE_PRESS,
    MOUSE_BUTTON_EXTRA_1_PRESS,
    MOUSE_BUTTON_EXTRA_2_PRESS,

    // Mouse button held
    MOUSE_BUTTON_LEFT_HELD,
    MOUSE_BUTTON_RIGHT_HELD,
    MOUSE_BUTTON_MIDDLE_HELD,
    MOUSE_BUTTON_EXTRA_1_HELD,
    MOUSE_BUTTON_EXTRA_2_HELD,

    // Mouse button released
    MOUSE_BUTTON_LEFT_RELEASE,
    MOUSE_BUTTON_RIGHT_RELEASE,
    MOUSE_BUTTON_MIDDLE_RELEASE,
    MOUSE_BUTTON_EXTRA_1_RELEASE,
    MOUSE_BUTTON_EXTRA_2_RELEASE,

    // Joystick
    JOYSTICK_BUTTON_PRESSED,
    JOYSTICK_BUTTON_RELEASED,
    JOYSTICK_MOVED,
    JOYSTICK_CONNECTED,
    JOYSTICK_DISCONNECTED,

    // Touch
    TOUCH_BEGAN,
    TOUCH_MOVED,
    TOUCH_ENDED,

    // Sensor
    SENSOR_CHANGED,

    // Keyboard pressed (A-Z)
    KEYPRESS_A, KEYPRESS_B, KEYPRESS_C, KEYPRESS_D, KEYPRESS_E,
    KEYPRESS_F, KEYPRESS_G, KEYPRESS_H, KEYPRESS_I, KEYPRESS_J,
    KEYPRESS_K, KEYPRESS_L, KEYPRESS_M, KEYPRESS_N, KEYPRESS_O,
    KEYPRESS_P, KEYPRESS_Q, KEYPRESS_R, KEYPRESS_S, KEYPRESS_T,
    KEYPRESS_U, KEYPRESS_V, KEYPRESS_W, KEYPRESS_X, KEYPRESS_Y,
    KEYPRESS_Z,

    // Keyboard pressed (0-9)
    KEYPRESS_NUM0, KEYPRESS_NUM1, KEYPRESS_NUM2, KEYPRESS_NUM3, KEYPRESS_NUM4,
    KEYPRESS_NUM5, KEYPRESS_NUM6, KEYPRESS_NUM7, KEYPRESS_NUM8, KEYPRESS_NUM9,

    // Keyboard pressed (special keys)
    KEYPRESS_ESCAPE, KEYPRESS_LCONTROL, KEYPRESS_LSHIFT, KEYPRESS_LALT, KEYPRESS_LSYSTEM,
    KEYPRESS_RCONTROL, KEYPRESS_RSHIFT, KEYPRESS_RALT, KEYPRESS_RSYSTEM,
    KEYPRESS_MENU, KEYPRESS_LBRACKET, KEYPRESS_RBRACKET, KEYPRESS_SEMICOLON,
    KEYPRESS_COMMA, KEYPRESS_PERIOD, KEYPRESS_QUOTE, KEYPRESS_SLASH,
    KEYPRESS_BACKSLASH, KEYPRESS_TILDE, KEYPRESS_EQUAL, KEYPRESS_DASH,
    KEYPRESS_SPACE, KEYPRESS_RETURN, KEYPRESS_BACKSPACE, KEYPRESS_TAB,
    KEYPRESS_PAGEUP, KEYPRESS_PAGEDOWN, KEYPRESS_END, KEYPRESS_HOME,
    KEYPRESS_INSERT, KEYPRESS_DELETE, KEYPRESS_ADD, KEYPRESS_SUBTRACT,
    KEYPRESS_MULTIPLY, KEYPRESS_DIVIDE,

    // Keyboard pressed (arrows)
    KEYPRESS_LEFT, KEYPRESS_RIGHT, KEYPRESS_UP, KEYPRESS_DOWN,

    // Keyboard pressed (numpad)
    KEYPRESS_NUMPAD0, KEYPRESS_NUMPAD1, KEYPRESS_NUMPAD2, KEYPRESS_NUMPAD3, KEYPRESS_NUMPAD4,
    KEYPRESS_NUMPAD5, KEYPRESS_NUMPAD6, KEYPRESS_NUMPAD7, KEYPRESS_NUMPAD8, KEYPRESS_NUMPAD9,

    // Keyboard pressed (F keys)
    KEYPRESS_F1, KEYPRESS_F2, KEYPRESS_F3, KEYPRESS_F4, KEYPRESS_F5,
    KEYPRESS_F6, KEYPRESS_F7, KEYPRESS_F8, KEYPRESS_F9, KEYPRESS_F10,
    KEYPRESS_F11, KEYPRESS_F12, KEYPRESS_F13, KEYPRESS_F14, KEYPRESS_F15,
    KEYPRESS_PAUSE,

    // Keyboard held (A-Z)
    KEYHELD_A, KEYHELD_B, KEYHELD_C, KEYHELD_D, KEYHELD_E,
    KEYHELD_F, KEYHELD_G, KEYHELD_H, KEYHELD_I, KEYHELD_J,
    KEYHELD_K, KEYHELD_L, KEYHELD_M, KEYHELD_N, KEYHELD_O,
    KEYHELD_P, KEYHELD_Q, KEYHELD_R, KEYHELD_S, KEYHELD_T,
    KEYHELD_U, KEYHELD_V, KEYHELD_W, KEYHELD_X, KEYHELD_Y,
    KEYHELD_Z,

    // Keyboard held (0-9)
    KEYHELD_NUM0, KEYHELD_NUM1, KEYHELD_NUM2, KEYHELD_NUM3, KEYHELD_NUM4,
    KEYHELD_NUM5, KEYHELD_NUM6, KEYHELD_NUM7, KEYHELD_NUM8, KEYHELD_NUM9,

    // Keyboard held (special keys)
    KEYHELD_ESCAPE, KEYHELD_LCONTROL, KEYHELD_LSHIFT, KEYHELD_LALT, KEYHELD_LSYSTEM,
    KEYHELD_RCONTROL, KEYHELD_RSHIFT, KEYHELD_RALT, KEYHELD_RSYSTEM,
    KEYHELD_MENU, KEYHELD_LBRACKET, KEYHELD_RBRACKET, KEYHELD_SEMICOLON,
    KEYHELD_COMMA, KEYHELD_PERIOD, KEYHELD_QUOTE, KEYHELD_SLASH,
    KEYHELD_BACKSLASH, KEYHELD_TILDE, KEYHELD_EQUAL, KEYHELD_DASH,
    KEYHELD_SPACE, KEYHELD_RETURN, KEYHELD_BACKSPACE, KEYHELD_TAB,
    KEYHELD_PAGEUP, KEYHELD_PAGEDOWN, KEYHELD_END, KEYHELD_HOME,
    KEYHELD_INSERT, KEYHELD_DELETE, KEYHELD_ADD, KEYHELD_SUBTRACT,
    KEYHELD_MULTIPLY, KEYHELD_DIVIDE,

    // Keyboard held (arrows)
    KEYHELD_LEFT, KEYHELD_RIGHT, KEYHELD_UP, KEYHELD_DOWN,

    // Keyboard held (numpad)
    KEYHELD_NUMPAD0, KEYHELD_NUMPAD1, KEYHELD_NUMPAD2, KEYHELD_NUMPAD3, KEYHELD_NUMPAD4,
    KEYHELD_NUMPAD5, KEYHELD_NUMPAD6, KEYHELD_NUMPAD7, KEYHELD_NUMPAD8, KEYHELD_NUMPAD9,

    // Keyboard held (F keys)
    KEYHELD_F1, KEYHELD_F2, KEYHELD_F3, KEYHELD_F4, KEYHELD_F5,
    KEYHELD_F6, KEYHELD_F7, KEYHELD_F8, KEYHELD_F9, KEYHELD_F10,
    KEYHELD_F11, KEYHELD_F12, KEYHELD_F13, KEYHELD_F14, KEYHELD_F15,
    KEYHELD_PAUSE,

    // Keyboard released (A-Z)
    KEYRELEASE_A, KEYRELEASE_B, KEYRELEASE_C, KEYRELEASE_D, KEYRELEASE_E,
    KEYRELEASE_F, KEYRELEASE_G, KEYRELEASE_H, KEYRELEASE_I, KEYRELEASE_J,
    KEYRELEASE_K, KEYRELEASE_L, KEYRELEASE_M, KEYRELEASE_N, KEYRELEASE_O,
    KEYRELEASE_P, KEYRELEASE_Q, KEYRELEASE_R, KEYRELEASE_S, KEYRELEASE_T,
    KEYRELEASE_U, KEYRELEASE_V, KEYRELEASE_W, KEYRELEASE_X, KEYRELEASE_Y,
    KEYRELEASE_Z,

    // Keyboard released (0-9)
    KEYRELEASE_NUM0, KEYRELEASE_NUM1, KEYRELEASE_NUM2, KEYRELEASE_NUM3, KEYRELEASE_NUM4,
    KEYRELEASE_NUM5, KEYRELEASE_NUM6, KEYRELEASE_NUM7, KEYRELEASE_NUM8, KEYRELEASE_NUM9,

    // Keyboard released (special keys)
    KEYRELEASE_ESCAPE, KEYRELEASE_LCONTROL, KEYRELEASE_LSHIFT, KEYRELEASE_LALT, KEYRELEASE_LSYSTEM,
    KEYRELEASE_RCONTROL, KEYRELEASE_RSHIFT, KEYRELEASE_RALT, KEYRELEASE_RSYSTEM,
    KEYRELEASE_MENU, KEYRELEASE_LBRACKET, KEYRELEASE_RBRACKET, KEYRELEASE_SEMICOLON,
    KEYRELEASE_COMMA, KEYRELEASE_PERIOD, KEYRELEASE_QUOTE, KEYRELEASE_SLASH,
    KEYRELEASE_BACKSLASH, KEYRELEASE_TILDE, KEYRELEASE_EQUAL, KEYRELEASE_DASH,
    KEYRELEASE_SPACE, KEYRELEASE_RETURN, KEYRELEASE_BACKSPACE, KEYRELEASE_TAB,
    KEYRELEASE_PAGEUP, KEYRELEASE_PAGEDOWN, KEYRELEASE_END, KEYRELEASE_HOME,
    KEYRELEASE_INSERT, KEYRELEASE_DELETE, KEYRELEASE_ADD, KEYRELEASE_SUBTRACT,
    KEYRELEASE_MULTIPLY, KEYRELEASE_DIVIDE,

    // Keyboard released (arrows)
    KEYRELEASE_LEFT, KEYRELEASE_RIGHT, KEYRELEASE_UP, KEYRELEASE_DOWN,

    // Keyboard released (numpad)
    KEYRELEASE_NUMPAD0, KEYRELEASE_NUMPAD1, KEYRELEASE_NUMPAD2, KEYRELEASE_NUMPAD3, KEYRELEASE_NUMPAD4,
    KEYRELEASE_NUMPAD5, KEYRELEASE_NUMPAD6, KEYRELEASE_NUMPAD7, KEYRELEASE_NUMPAD8, KEYRELEASE_NUMPAD9,

    // Keyboard released (F keys)
    KEYRELEASE_F1, KEYRELEASE_F2, KEYRELEASE_F3, KEYRELEASE_F4, KEYRELEASE_F5,
    KEYRELEASE_F6, KEYRELEASE_F7, KEYRELEASE_F8, KEYRELEASE_F9, KEYRELEASE_F10,
    KEYRELEASE_F11, KEYRELEASE_F12, KEYRELEASE_F13, KEYRELEASE_F14, KEYRELEASE_F15,
    KEYRELEASE_PAUSE,
};

// ============================================================================
// Stream operator
// ----------------------------------------------------------------------------

inline std::ostream& operator<<(
    std::ostream&
    os, SFML_event_type const type
    ) {

    switch (type) {

        case SFML_event_type::WINDOW_CLOSED: return os << "WINDOW_CLOSED";
        case SFML_event_type::WINDOW_RESIZED: return os << "WINDOW_RESIZED";
        case SFML_event_type::WINDOW_LOST_FOCUS: return os << "WINDOW_LOST_FOCUS";
        case SFML_event_type::WINDOW_GAINED_FOCUS: return os << "WINDOW_GAINED_FOCUS";
        case SFML_event_type::WINDOW_TEXT_ENTERED: return os << "WINDOW_TEXT_ENTERED";

        case SFML_event_type::MOUSE_MOVED: return os << "MOUSE_MOVED";
        case SFML_event_type::MOUSE_ENTER_WINDOW: return os << "MOUSE_ENTER_WINDOW";
        case SFML_event_type::MOUSE_LEAVE_WINDOW: return os << "MOUSE_LEAVE_WINDOW";

        case SFML_event_type::MOUSE_WHEEL_UP: return os << "MOUSE_WHEEL_UP";
        case SFML_event_type::MOUSE_WHEEL_DOWN: return os << "MOUSE_WHEEL_DOWN";

        case SFML_event_type::MOUSE_BUTTON_LEFT_PRESS: return os << "MOUSE_BUTTON_LEFT_PRESS";
        case SFML_event_type::MOUSE_BUTTON_RIGHT_PRESS: return os << "MOUSE_BUTTON_RIGHT_PRESS";
        case SFML_event_type::MOUSE_BUTTON_MIDDLE_PRESS: return os << "MOUSE_BUTTON_MIDDLE_PRESS";
        case SFML_event_type::MOUSE_BUTTON_EXTRA_1_PRESS: return os << "MOUSE_BUTTON_EXTRA_1_PRESS";
        case SFML_event_type::MOUSE_BUTTON_EXTRA_2_PRESS: return os << "MOUSE_BUTTON_EXTRA_2_PRESS";

        case SFML_event_type::MOUSE_BUTTON_LEFT_HELD: return os << "MOUSE_BUTTON_LEFT_HELD";
        case SFML_event_type::MOUSE_BUTTON_RIGHT_HELD: return os << "MOUSE_BUTTON_RIGHT_HELD";
        case SFML_event_type::MOUSE_BUTTON_MIDDLE_HELD: return os << "MOUSE_BUTTON_MIDDLE_HELD";
        case SFML_event_type::MOUSE_BUTTON_EXTRA_1_HELD: return os << "MOUSE_BUTTON_EXTRA_1_HELD";
        case SFML_event_type::MOUSE_BUTTON_EXTRA_2_HELD: return os << "MOUSE_BUTTON_EXTRA_2_HELD";

        case SFML_event_type::MOUSE_BUTTON_LEFT_RELEASE: return os << "MOUSE_BUTTON_LEFT_RELEASE";
        case SFML_event_type::MOUSE_BUTTON_RIGHT_RELEASE: return os << "MOUSE_BUTTON_RIGHT_RELEASE";
        case SFML_event_type::MOUSE_BUTTON_MIDDLE_RELEASE: return os << "MOUSE_BUTTON_MIDDLE_RELEASE";
        case SFML_event_type::MOUSE_BUTTON_EXTRA_1_RELEASE: return os << "MOUSE_BUTTON_EXTRA_1_RELEASE";
        case SFML_event_type::MOUSE_BUTTON_EXTRA_2_RELEASE: return os << "MOUSE_BUTTON_EXTRA_2_RELEASE";

        case SFML_event_type::JOYSTICK_BUTTON_PRESSED: return os << "JOYSTICK_BUTTON_PRESSED";
        case SFML_event_type::JOYSTICK_BUTTON_RELEASED: return os << "JOYSTICK_BUTTON_RELEASED";
        case SFML_event_type::JOYSTICK_MOVED: return os << "JOYSTICK_MOVED";
        case SFML_event_type::JOYSTICK_CONNECTED: return os << "JOYSTICK_CONNECTED";
        case SFML_event_type::JOYSTICK_DISCONNECTED: return os << "JOYSTICK_DISCONNECTED";

        case SFML_event_type::TOUCH_BEGAN: return os << "TOUCH_BEGAN";
        case SFML_event_type::TOUCH_MOVED: return os << "TOUCH_MOVED";
        case SFML_event_type::TOUCH_ENDED: return os << "TOUCH_ENDED";

        case SFML_event_type::SENSOR_CHANGED: return os << "SENSOR_CHANGED";

        // KEYPRESS A-Z
        case SFML_event_type::KEYPRESS_A: return os << "KEYPRESS_A";
        case SFML_event_type::KEYPRESS_B: return os << "KEYPRESS_B";
        case SFML_event_type::KEYPRESS_C: return os << "KEYPRESS_C";
        case SFML_event_type::KEYPRESS_D: return os << "KEYPRESS_D";
        case SFML_event_type::KEYPRESS_E: return os << "KEYPRESS_E";
        case SFML_event_type::KEYPRESS_F: return os << "KEYPRESS_F";
        case SFML_event_type::KEYPRESS_G: return os << "KEYPRESS_G";
        case SFML_event_type::KEYPRESS_H: return os << "KEYPRESS_H";
        case SFML_event_type::KEYPRESS_I: return os << "KEYPRESS_I";
        case SFML_event_type::KEYPRESS_J: return os << "KEYPRESS_J";
        case SFML_event_type::KEYPRESS_K: return os << "KEYPRESS_K";
        case SFML_event_type::KEYPRESS_L: return os << "KEYPRESS_L";
        case SFML_event_type::KEYPRESS_M: return os << "KEYPRESS_M";
        case SFML_event_type::KEYPRESS_N: return os << "KEYPRESS_N";
        case SFML_event_type::KEYPRESS_O: return os << "KEYPRESS_O";
        case SFML_event_type::KEYPRESS_P: return os << "KEYPRESS_P";
        case SFML_event_type::KEYPRESS_Q: return os << "KEYPRESS_Q";
        case SFML_event_type::KEYPRESS_R: return os << "KEYPRESS_R";
        case SFML_event_type::KEYPRESS_S: return os << "KEYPRESS_S";
        case SFML_event_type::KEYPRESS_T: return os << "KEYPRESS_T";
        case SFML_event_type::KEYPRESS_U: return os << "KEYPRESS_U";
        case SFML_event_type::KEYPRESS_V: return os << "KEYPRESS_V";
        case SFML_event_type::KEYPRESS_W: return os << "KEYPRESS_W";
        case SFML_event_type::KEYPRESS_X: return os << "KEYPRESS_X";
        case SFML_event_type::KEYPRESS_Y: return os << "KEYPRESS_Y";
        case SFML_event_type::KEYPRESS_Z: return os << "KEYPRESS_Z";

        // KEYPRESS 0-9
        case SFML_event_type::KEYPRESS_NUM0: return os << "KEYPRESS_NUM0";
        case SFML_event_type::KEYPRESS_NUM1: return os << "KEYPRESS_NUM1";
        case SFML_event_type::KEYPRESS_NUM2: return os << "KEYPRESS_NUM2";
        case SFML_event_type::KEYPRESS_NUM3: return os << "KEYPRESS_NUM3";
        case SFML_event_type::KEYPRESS_NUM4: return os << "KEYPRESS_NUM4";
        case SFML_event_type::KEYPRESS_NUM5: return os << "KEYPRESS_NUM5";
        case SFML_event_type::KEYPRESS_NUM6: return os << "KEYPRESS_NUM6";
        case SFML_event_type::KEYPRESS_NUM7: return os << "KEYPRESS_NUM7";
        case SFML_event_type::KEYPRESS_NUM8: return os << "KEYPRESS_NUM8";
        case SFML_event_type::KEYPRESS_NUM9: return os << "KEYPRESS_NUM9";

        // KEYPRESS special
        case SFML_event_type::KEYPRESS_ESCAPE: return os << "KEYPRESS_ESCAPE";
        case SFML_event_type::KEYPRESS_LCONTROL: return os << "KEYPRESS_LCONTROL";
        case SFML_event_type::KEYPRESS_LSHIFT: return os << "KEYPRESS_LSHIFT";
        case SFML_event_type::KEYPRESS_LALT: return os << "KEYPRESS_LALT";
        case SFML_event_type::KEYPRESS_LSYSTEM: return os << "KEYPRESS_LSYSTEM";
        case SFML_event_type::KEYPRESS_RCONTROL: return os << "KEYPRESS_RCONTROL";
        case SFML_event_type::KEYPRESS_RSHIFT: return os << "KEYPRESS_RSHIFT";
        case SFML_event_type::KEYPRESS_RALT: return os << "KEYPRESS_RALT";
        case SFML_event_type::KEYPRESS_RSYSTEM: return os << "KEYPRESS_RSYSTEM";
        case SFML_event_type::KEYPRESS_MENU: return os << "KEYPRESS_MENU";
        case SFML_event_type::KEYPRESS_LBRACKET: return os << "KEYPRESS_LBRACKET";
        case SFML_event_type::KEYPRESS_RBRACKET: return os << "KEYPRESS_RBRACKET";
        case SFML_event_type::KEYPRESS_SEMICOLON: return os << "KEYPRESS_SEMICOLON";
        case SFML_event_type::KEYPRESS_COMMA: return os << "KEYPRESS_COMMA";
        case SFML_event_type::KEYPRESS_PERIOD: return os << "KEYPRESS_PERIOD";
        case SFML_event_type::KEYPRESS_QUOTE: return os << "KEYPRESS_QUOTE";
        case SFML_event_type::KEYPRESS_SLASH: return os << "KEYPRESS_SLASH";
        case SFML_event_type::KEYPRESS_BACKSLASH: return os << "KEYPRESS_BACKSLASH";
        case SFML_event_type::KEYPRESS_TILDE: return os << "KEYPRESS_TILDE";
        case SFML_event_type::KEYPRESS_EQUAL: return os << "KEYPRESS_EQUAL";
        case SFML_event_type::KEYPRESS_DASH: return os << "KEYPRESS_DASH";
        case SFML_event_type::KEYPRESS_SPACE: return os << "KEYPRESS_SPACE";
        case SFML_event_type::KEYPRESS_RETURN: return os << "KEYPRESS_RETURN";
        case SFML_event_type::KEYPRESS_BACKSPACE: return os << "KEYPRESS_BACKSPACE";
        case SFML_event_type::KEYPRESS_TAB: return os << "KEYPRESS_TAB";
        case SFML_event_type::KEYPRESS_PAGEUP: return os << "KEYPRESS_PAGEUP";
        case SFML_event_type::KEYPRESS_PAGEDOWN: return os << "KEYPRESS_PAGEDOWN";
        case SFML_event_type::KEYPRESS_END: return os << "KEYPRESS_END";
        case SFML_event_type::KEYPRESS_HOME: return os << "KEYPRESS_HOME";
        case SFML_event_type::KEYPRESS_INSERT: return os << "KEYPRESS_INSERT";
        case SFML_event_type::KEYPRESS_DELETE: return os << "KEYPRESS_DELETE";
        case SFML_event_type::KEYPRESS_ADD: return os << "KEYPRESS_ADD";
        case SFML_event_type::KEYPRESS_SUBTRACT: return os << "KEYPRESS_SUBTRACT";
        case SFML_event_type::KEYPRESS_MULTIPLY: return os << "KEYPRESS_MULTIPLY";
        case SFML_event_type::KEYPRESS_DIVIDE: return os << "KEYPRESS_DIVIDE";

        // KEYPRESS arrows
        case SFML_event_type::KEYPRESS_LEFT: return os << "KEYPRESS_LEFT";
        case SFML_event_type::KEYPRESS_RIGHT: return os << "KEYPRESS_RIGHT";
        case SFML_event_type::KEYPRESS_UP: return os << "KEYPRESS_UP";
        case SFML_event_type::KEYPRESS_DOWN: return os << "KEYPRESS_DOWN";

        // KEYPRESS numpad
        case SFML_event_type::KEYPRESS_NUMPAD0: return os << "KEYPRESS_NUMPAD0";
        case SFML_event_type::KEYPRESS_NUMPAD1: return os << "KEYPRESS_NUMPAD1";
        case SFML_event_type::KEYPRESS_NUMPAD2: return os << "KEYPRESS_NUMPAD2";
        case SFML_event_type::KEYPRESS_NUMPAD3: return os << "KEYPRESS_NUMPAD3";
        case SFML_event_type::KEYPRESS_NUMPAD4: return os << "KEYPRESS_NUMPAD4";
        case SFML_event_type::KEYPRESS_NUMPAD5: return os << "KEYPRESS_NUMPAD5";
        case SFML_event_type::KEYPRESS_NUMPAD6: return os << "KEYPRESS_NUMPAD6";
        case SFML_event_type::KEYPRESS_NUMPAD7: return os << "KEYPRESS_NUMPAD7";
        case SFML_event_type::KEYPRESS_NUMPAD8: return os << "KEYPRESS_NUMPAD8";
        case SFML_event_type::KEYPRESS_NUMPAD9: return os << "KEYPRESS_NUMPAD9";

        // KEYPRESS F keys
        case SFML_event_type::KEYPRESS_F1: return os << "KEYPRESS_F1";
        case SFML_event_type::KEYPRESS_F2: return os << "KEYPRESS_F2";
        case SFML_event_type::KEYPRESS_F3: return os << "KEYPRESS_F3";
        case SFML_event_type::KEYPRESS_F4: return os << "KEYPRESS_F4";
        case SFML_event_type::KEYPRESS_F5: return os << "KEYPRESS_F5";
        case SFML_event_type::KEYPRESS_F6: return os << "KEYPRESS_F6";
        case SFML_event_type::KEYPRESS_F7: return os << "KEYPRESS_F7";
        case SFML_event_type::KEYPRESS_F8: return os << "KEYPRESS_F8";
        case SFML_event_type::KEYPRESS_F9: return os << "KEYPRESS_F9";
        case SFML_event_type::KEYPRESS_F10: return os << "KEYPRESS_F10";
        case SFML_event_type::KEYPRESS_F11: return os << "KEYPRESS_F11";
        case SFML_event_type::KEYPRESS_F12: return os << "KEYPRESS_F12";
        case SFML_event_type::KEYPRESS_F13: return os << "KEYPRESS_F13";
        case SFML_event_type::KEYPRESS_F14: return os << "KEYPRESS_F14";
        case SFML_event_type::KEYPRESS_F15: return os << "KEYPRESS_F15";
        case SFML_event_type::KEYPRESS_PAUSE: return os << "KEYPRESS_PAUSE";

        // KEYHELD A-Z
        case SFML_event_type::KEYHELD_A: return os << "KEYHELD_A";
        case SFML_event_type::KEYHELD_B: return os << "KEYHELD_B";
        case SFML_event_type::KEYHELD_C: return os << "KEYHELD_C";
        case SFML_event_type::KEYHELD_D: return os << "KEYHELD_D";
        case SFML_event_type::KEYHELD_E: return os << "KEYHELD_E";
        case SFML_event_type::KEYHELD_F: return os << "KEYHELD_F";
        case SFML_event_type::KEYHELD_G: return os << "KEYHELD_G";
        case SFML_event_type::KEYHELD_H: return os << "KEYHELD_H";
        case SFML_event_type::KEYHELD_I: return os << "KEYHELD_I";
        case SFML_event_type::KEYHELD_J: return os << "KEYHELD_J";
        case SFML_event_type::KEYHELD_K: return os << "KEYHELD_K";
        case SFML_event_type::KEYHELD_L: return os << "KEYHELD_L";
        case SFML_event_type::KEYHELD_M: return os << "KEYHELD_M";
        case SFML_event_type::KEYHELD_N: return os << "KEYHELD_N";
        case SFML_event_type::KEYHELD_O: return os << "KEYHELD_O";
        case SFML_event_type::KEYHELD_P: return os << "KEYHELD_P";
        case SFML_event_type::KEYHELD_Q: return os << "KEYHELD_Q";
        case SFML_event_type::KEYHELD_R: return os << "KEYHELD_R";
        case SFML_event_type::KEYHELD_S: return os << "KEYHELD_S";
        case SFML_event_type::KEYHELD_T: return os << "KEYHELD_T";
        case SFML_event_type::KEYHELD_U: return os << "KEYHELD_U";
        case SFML_event_type::KEYHELD_V: return os << "KEYHELD_V";
        case SFML_event_type::KEYHELD_W: return os << "KEYHELD_W";
        case SFML_event_type::KEYHELD_X: return os << "KEYHELD_X";
        case SFML_event_type::KEYHELD_Y: return os << "KEYHELD_Y";
        case SFML_event_type::KEYHELD_Z: return os << "KEYHELD_Z";

        // KEYHELD 0-9
        case SFML_event_type::KEYHELD_NUM0: return os << "KEYHELD_NUM0";
        case SFML_event_type::KEYHELD_NUM1: return os << "KEYHELD_NUM1";
        case SFML_event_type::KEYHELD_NUM2: return os << "KEYHELD_NUM2";
        case SFML_event_type::KEYHELD_NUM3: return os << "KEYHELD_NUM3";
        case SFML_event_type::KEYHELD_NUM4: return os << "KEYHELD_NUM4";
        case SFML_event_type::KEYHELD_NUM5: return os << "KEYHELD_NUM5";
        case SFML_event_type::KEYHELD_NUM6: return os << "KEYHELD_NUM6";
        case SFML_event_type::KEYHELD_NUM7: return os << "KEYHELD_NUM7";
        case SFML_event_type::KEYHELD_NUM8: return os << "KEYHELD_NUM8";
        case SFML_event_type::KEYHELD_NUM9: return os << "KEYHELD_NUM9";

        // KEYHELD special
        case SFML_event_type::KEYHELD_ESCAPE: return os << "KEYHELD_ESCAPE";
        case SFML_event_type::KEYHELD_LCONTROL: return os << "KEYHELD_LCONTROL";
        case SFML_event_type::KEYHELD_LSHIFT: return os << "KEYHELD_LSHIFT";
        case SFML_event_type::KEYHELD_LALT: return os << "KEYHELD_LALT";
        case SFML_event_type::KEYHELD_LSYSTEM: return os << "KEYHELD_LSYSTEM";
        case SFML_event_type::KEYHELD_RCONTROL: return os << "KEYHELD_RCONTROL";
        case SFML_event_type::KEYHELD_RSHIFT: return os << "KEYHELD_RSHIFT";
        case SFML_event_type::KEYHELD_RALT: return os << "KEYHELD_RALT";
        case SFML_event_type::KEYHELD_RSYSTEM: return os << "KEYHELD_RSYSTEM";
        case SFML_event_type::KEYHELD_MENU: return os << "KEYHELD_MENU";
        case SFML_event_type::KEYHELD_LBRACKET: return os << "KEYHELD_LBRACKET";
        case SFML_event_type::KEYHELD_RBRACKET: return os << "KEYHELD_RBRACKET";
        case SFML_event_type::KEYHELD_SEMICOLON: return os << "KEYHELD_SEMICOLON";
        case SFML_event_type::KEYHELD_COMMA: return os << "KEYHELD_COMMA";
        case SFML_event_type::KEYHELD_PERIOD: return os << "KEYHELD_PERIOD";
        case SFML_event_type::KEYHELD_QUOTE: return os << "KEYHELD_QUOTE";
        case SFML_event_type::KEYHELD_SLASH: return os << "KEYHELD_SLASH";
        case SFML_event_type::KEYHELD_BACKSLASH: return os << "KEYHELD_BACKSLASH";
        case SFML_event_type::KEYHELD_TILDE: return os << "KEYHELD_TILDE";
        case SFML_event_type::KEYHELD_EQUAL: return os << "KEYHELD_EQUAL";
        case SFML_event_type::KEYHELD_DASH: return os << "KEYHELD_DASH";
        case SFML_event_type::KEYHELD_SPACE: return os << "KEYHELD_SPACE";
        case SFML_event_type::KEYHELD_RETURN: return os << "KEYHELD_RETURN";
        case SFML_event_type::KEYHELD_BACKSPACE: return os << "KEYHELD_BACKSPACE";
        case SFML_event_type::KEYHELD_TAB: return os << "KEYHELD_TAB";
        case SFML_event_type::KEYHELD_PAGEUP: return os << "KEYHELD_PAGEUP";
        case SFML_event_type::KEYHELD_PAGEDOWN: return os << "KEYHELD_PAGEDOWN";
        case SFML_event_type::KEYHELD_END: return os << "KEYHELD_END";
        case SFML_event_type::KEYHELD_HOME: return os << "KEYHELD_HOME";
        case SFML_event_type::KEYHELD_INSERT: return os << "KEYHELD_INSERT";
        case SFML_event_type::KEYHELD_DELETE: return os << "KEYHELD_DELETE";
        case SFML_event_type::KEYHELD_ADD: return os << "KEYHELD_ADD";
        case SFML_event_type::KEYHELD_SUBTRACT: return os << "KEYHELD_SUBTRACT";
        case SFML_event_type::KEYHELD_MULTIPLY: return os << "KEYHELD_MULTIPLY";
        case SFML_event_type::KEYHELD_DIVIDE: return os << "KEYHELD_DIVIDE";

        // KEYHELD arrows
        case SFML_event_type::KEYHELD_LEFT: return os << "KEYHELD_LEFT";
        case SFML_event_type::KEYHELD_RIGHT: return os << "KEYHELD_RIGHT";
        case SFML_event_type::KEYHELD_UP: return os << "KEYHELD_UP";
        case SFML_event_type::KEYHELD_DOWN: return os << "KEYHELD_DOWN";

        // KEYHELD numpad
        case SFML_event_type::KEYHELD_NUMPAD0: return os << "KEYHELD_NUMPAD0";
        case SFML_event_type::KEYHELD_NUMPAD1: return os << "KEYHELD_NUMPAD1";
        case SFML_event_type::KEYHELD_NUMPAD2: return os << "KEYHELD_NUMPAD2";
        case SFML_event_type::KEYHELD_NUMPAD3: return os << "KEYHELD_NUMPAD3";
        case SFML_event_type::KEYHELD_NUMPAD4: return os << "KEYHELD_NUMPAD4";
        case SFML_event_type::KEYHELD_NUMPAD5: return os << "KEYHELD_NUMPAD5";
        case SFML_event_type::KEYHELD_NUMPAD6: return os << "KEYHELD_NUMPAD6";
        case SFML_event_type::KEYHELD_NUMPAD7: return os << "KEYHELD_NUMPAD7";
        case SFML_event_type::KEYHELD_NUMPAD8: return os << "KEYHELD_NUMPAD8";
        case SFML_event_type::KEYHELD_NUMPAD9: return os << "KEYHELD_NUMPAD9";

        // KEYHELD F keys
        case SFML_event_type::KEYHELD_F1: return os << "KEYHELD_F1";
        case SFML_event_type::KEYHELD_F2: return os << "KEYHELD_F2";
        case SFML_event_type::KEYHELD_F3: return os << "KEYHELD_F3";
        case SFML_event_type::KEYHELD_F4: return os << "KEYHELD_F4";
        case SFML_event_type::KEYHELD_F5: return os << "KEYHELD_F5";
        case SFML_event_type::KEYHELD_F6: return os << "KEYHELD_F6";
        case SFML_event_type::KEYHELD_F7: return os << "KEYHELD_F7";
        case SFML_event_type::KEYHELD_F8: return os << "KEYHELD_F8";
        case SFML_event_type::KEYHELD_F9: return os << "KEYHELD_F9";
        case SFML_event_type::KEYHELD_F10: return os << "KEYHELD_F10";
        case SFML_event_type::KEYHELD_F11: return os << "KEYHELD_F11";
        case SFML_event_type::KEYHELD_F12: return os << "KEYHELD_F12";
        case SFML_event_type::KEYHELD_F13: return os << "KEYHELD_F13";
        case SFML_event_type::KEYHELD_F14: return os << "KEYHELD_F14";
        case SFML_event_type::KEYHELD_F15: return os << "KEYHELD_F15";
        case SFML_event_type::KEYHELD_PAUSE: return os << "KEYHELD_PAUSE";

        // KEYRELEASE A-Z
        case SFML_event_type::KEYRELEASE_A: return os << "KEYRELEASE_A";
        case SFML_event_type::KEYRELEASE_B: return os << "KEYRELEASE_B";
        case SFML_event_type::KEYRELEASE_C: return os << "KEYRELEASE_C";
        case SFML_event_type::KEYRELEASE_D: return os << "KEYRELEASE_D";
        case SFML_event_type::KEYRELEASE_E: return os << "KEYRELEASE_E";
        case SFML_event_type::KEYRELEASE_F: return os << "KEYRELEASE_F";
        case SFML_event_type::KEYRELEASE_G: return os << "KEYRELEASE_G";
        case SFML_event_type::KEYRELEASE_H: return os << "KEYRELEASE_H";
        case SFML_event_type::KEYRELEASE_I: return os << "KEYRELEASE_I";
        case SFML_event_type::KEYRELEASE_J: return os << "KEYRELEASE_J";
        case SFML_event_type::KEYRELEASE_K: return os << "KEYRELEASE_K";
        case SFML_event_type::KEYRELEASE_L: return os << "KEYRELEASE_L";
        case SFML_event_type::KEYRELEASE_M: return os << "KEYRELEASE_M";
        case SFML_event_type::KEYRELEASE_N: return os << "KEYRELEASE_N";
        case SFML_event_type::KEYRELEASE_O: return os << "KEYRELEASE_O";
        case SFML_event_type::KEYRELEASE_P: return os << "KEYRELEASE_P";
        case SFML_event_type::KEYRELEASE_Q: return os << "KEYRELEASE_Q";
        case SFML_event_type::KEYRELEASE_R: return os << "KEYRELEASE_R";
        case SFML_event_type::KEYRELEASE_S: return os << "KEYRELEASE_S";
        case SFML_event_type::KEYRELEASE_T: return os << "KEYRELEASE_T";
        case SFML_event_type::KEYRELEASE_U: return os << "KEYRELEASE_U";
        case SFML_event_type::KEYRELEASE_V: return os << "KEYRELEASE_V";
        case SFML_event_type::KEYRELEASE_W: return os << "KEYRELEASE_W";
        case SFML_event_type::KEYRELEASE_X: return os << "KEYRELEASE_X";
        case SFML_event_type::KEYRELEASE_Y: return os << "KEYRELEASE_Y";
        case SFML_event_type::KEYRELEASE_Z: return os << "KEYRELEASE_Z";

        // KEYRELEASE 0-9
        case SFML_event_type::KEYRELEASE_NUM0: return os << "KEYRELEASE_NUM0";
        case SFML_event_type::KEYRELEASE_NUM1: return os << "KEYRELEASE_NUM1";
        case SFML_event_type::KEYRELEASE_NUM2: return os << "KEYRELEASE_NUM2";
        case SFML_event_type::KEYRELEASE_NUM3: return os << "KEYRELEASE_NUM3";
        case SFML_event_type::KEYRELEASE_NUM4: return os << "KEYRELEASE_NUM4";
        case SFML_event_type::KEYRELEASE_NUM5: return os << "KEYRELEASE_NUM5";
        case SFML_event_type::KEYRELEASE_NUM6: return os << "KEYRELEASE_NUM6";
        case SFML_event_type::KEYRELEASE_NUM7: return os << "KEYRELEASE_NUM7";
        case SFML_event_type::KEYRELEASE_NUM8: return os << "KEYRELEASE_NUM8";
        case SFML_event_type::KEYRELEASE_NUM9: return os << "KEYRELEASE_NUM9";

        // KEYRELEASE special
        case SFML_event_type::KEYRELEASE_ESCAPE: return os << "KEYRELEASE_ESCAPE";
        case SFML_event_type::KEYRELEASE_LCONTROL: return os << "KEYRELEASE_LCONTROL";
        case SFML_event_type::KEYRELEASE_LSHIFT: return os << "KEYRELEASE_LSHIFT";
        case SFML_event_type::KEYRELEASE_LALT: return os << "KEYRELEASE_LALT";
        case SFML_event_type::KEYRELEASE_LSYSTEM: return os << "KEYRELEASE_LSYSTEM";
        case SFML_event_type::KEYRELEASE_RCONTROL: return os << "KEYRELEASE_RCONTROL";
        case SFML_event_type::KEYRELEASE_RSHIFT: return os << "KEYRELEASE_RSHIFT";
        case SFML_event_type::KEYRELEASE_RALT: return os << "KEYRELEASE_RALT";
        case SFML_event_type::KEYRELEASE_RSYSTEM: return os << "KEYRELEASE_RSYSTEM";
        case SFML_event_type::KEYRELEASE_MENU: return os << "KEYRELEASE_MENU";
        case SFML_event_type::KEYRELEASE_LBRACKET: return os << "KEYRELEASE_LBRACKET";
        case SFML_event_type::KEYRELEASE_RBRACKET: return os << "KEYRELEASE_RBRACKET";
        case SFML_event_type::KEYRELEASE_SEMICOLON: return os << "KEYRELEASE_SEMICOLON";
        case SFML_event_type::KEYRELEASE_COMMA: return os << "KEYRELEASE_COMMA";
        case SFML_event_type::KEYRELEASE_PERIOD: return os << "KEYRELEASE_PERIOD";
        case SFML_event_type::KEYRELEASE_QUOTE: return os << "KEYRELEASE_QUOTE";
        case SFML_event_type::KEYRELEASE_SLASH: return os << "KEYRELEASE_SLASH";
        case SFML_event_type::KEYRELEASE_BACKSLASH: return os << "KEYRELEASE_BACKSLASH";
        case SFML_event_type::KEYRELEASE_TILDE: return os << "KEYRELEASE_TILDE";
        case SFML_event_type::KEYRELEASE_EQUAL: return os << "KEYRELEASE_EQUAL";
        case SFML_event_type::KEYRELEASE_DASH: return os << "KEYRELEASE_DASH";
        case SFML_event_type::KEYRELEASE_SPACE: return os << "KEYRELEASE_SPACE";
        case SFML_event_type::KEYRELEASE_RETURN: return os << "KEYRELEASE_RETURN";
        case SFML_event_type::KEYRELEASE_BACKSPACE: return os << "KEYRELEASE_BACKSPACE";
        case SFML_event_type::KEYRELEASE_TAB: return os << "KEYRELEASE_TAB";
        case SFML_event_type::KEYRELEASE_PAGEUP: return os << "KEYRELEASE_PAGEUP";
        case SFML_event_type::KEYRELEASE_PAGEDOWN: return os << "KEYRELEASE_PAGEDOWN";
        case SFML_event_type::KEYRELEASE_END: return os << "KEYRELEASE_END";
        case SFML_event_type::KEYRELEASE_HOME: return os << "KEYRELEASE_HOME";
        case SFML_event_type::KEYRELEASE_INSERT: return os << "KEYRELEASE_INSERT";
        case SFML_event_type::KEYRELEASE_DELETE: return os << "KEYRELEASE_DELETE";
        case SFML_event_type::KEYRELEASE_ADD: return os << "KEYRELEASE_ADD";
        case SFML_event_type::KEYRELEASE_SUBTRACT: return os << "KEYRELEASE_SUBTRACT";
        case SFML_event_type::KEYRELEASE_MULTIPLY: return os << "KEYRELEASE_MULTIPLY";
        case SFML_event_type::KEYRELEASE_DIVIDE: return os << "KEYRELEASE_DIVIDE";

        // KEYRELEASE arrows
        case SFML_event_type::KEYRELEASE_LEFT: return os << "KEYRELEASE_LEFT";
        case SFML_event_type::KEYRELEASE_RIGHT: return os << "KEYRELEASE_RIGHT";
        case SFML_event_type::KEYRELEASE_UP: return os << "KEYRELEASE_UP";
        case SFML_event_type::KEYRELEASE_DOWN: return os << "KEYRELEASE_DOWN";

        // KEYRELEASE numpad
        case SFML_event_type::KEYRELEASE_NUMPAD0: return os << "KEYRELEASE_NUMPAD0";
        case SFML_event_type::KEYRELEASE_NUMPAD1: return os << "KEYRELEASE_NUMPAD1";
        case SFML_event_type::KEYRELEASE_NUMPAD2: return os << "KEYRELEASE_NUMPAD2";
        case SFML_event_type::KEYRELEASE_NUMPAD3: return os << "KEYRELEASE_NUMPAD3";
        case SFML_event_type::KEYRELEASE_NUMPAD4: return os << "KEYRELEASE_NUMPAD4";
        case SFML_event_type::KEYRELEASE_NUMPAD5: return os << "KEYRELEASE_NUMPAD5";
        case SFML_event_type::KEYRELEASE_NUMPAD6: return os << "KEYRELEASE_NUMPAD6";
        case SFML_event_type::KEYRELEASE_NUMPAD7: return os << "KEYRELEASE_NUMPAD7";
        case SFML_event_type::KEYRELEASE_NUMPAD8: return os << "KEYRELEASE_NUMPAD8";
        case SFML_event_type::KEYRELEASE_NUMPAD9: return os << "KEYRELEASE_NUMPAD9";

        // KEYRELEASE F keys
        case SFML_event_type::KEYRELEASE_F1: return os << "KEYRELEASE_F1";
        case SFML_event_type::KEYRELEASE_F2: return os << "KEYRELEASE_F2";
        case SFML_event_type::KEYRELEASE_F3: return os << "KEYRELEASE_F3";
        case SFML_event_type::KEYRELEASE_F4: return os << "KEYRELEASE_F4";
        case SFML_event_type::KEYRELEASE_F5: return os << "KEYRELEASE_F5";
        case SFML_event_type::KEYRELEASE_F6: return os << "KEYRELEASE_F6";
        case SFML_event_type::KEYRELEASE_F7: return os << "KEYRELEASE_F7";
        case SFML_event_type::KEYRELEASE_F8: return os << "KEYRELEASE_F8";
        case SFML_event_type::KEYRELEASE_F9: return os << "KEYRELEASE_F9";
        case SFML_event_type::KEYRELEASE_F10: return os << "KEYRELEASE_F10";
        case SFML_event_type::KEYRELEASE_F11: return os << "KEYRELEASE_F11";
        case SFML_event_type::KEYRELEASE_F12: return os << "KEYRELEASE_F12";
        case SFML_event_type::KEYRELEASE_F13: return os << "KEYRELEASE_F13";
        case SFML_event_type::KEYRELEASE_F14: return os << "KEYRELEASE_F14";
        case SFML_event_type::KEYRELEASE_F15: return os << "KEYRELEASE_F15";
        case SFML_event_type::KEYRELEASE_PAUSE: return os << "KEYRELEASE_PAUSE";

        case SFML_event_type::UNKNOWN: [[fallthrough]];
        default: return os << "UNKNOWN SFML EVENT TYPE";
    }
}

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using Game_event_type = std::string;

} // namespace titan::events
