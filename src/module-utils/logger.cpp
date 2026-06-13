// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-utils/include/logger.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Definitions for static members
// ----------------------------------------------------------------------------

inline std::mutex Logger::s_mutex;
inline std::uint8_t Logger::ENABLED_LOG_LVLS = 0xFF;

// ============================================================================
// Class Logger
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Logger::Logger(
    Log_lvl const level,
    std::source_location const loc
    )
    : _level(level)
    , _location(loc)
{}

// ----------------------------------------------------------------------------
Logger::~Logger() {

    bool const is_buffer_empty = _buffer.str().empty();

    if (is_buffer_empty) {

        // No need to flush since buffer is empty.
    } else {

        _flush();
    }
}

// ----------------------------------------------------------------------------
void Logger::_flush() {

    // Lock the mutex so no other Logger objects can log till this is finished.
    // Unlocks automatically when this function is out of scope.
    std::lock_guard<std::mutex> lock(s_mutex);

    // Only flush if the log level is enabled right now.
    if (_is_log_lvl_enabled(_level)) {

        char const* const reset_color = "\033[0m"; // Reset color

        // Output the buffer log to the console.
        std::cout << _get_color()   << /*---------------------*/     '\n'
                  << '['            << _level                    << "]\n"
                  << "[Time: "      << _curr_time()              << "]\n"
                  << "[File: "      << _get_file()               << "]\n"
                  << "[Line: "      << _location.line()          << "]\n"
                  << "[Function: "  << _location.function_name() << "]\n"
                  << "[Logger: "    << _buffer.str()             << "]\n"
                  << reset_color    << std::endl;

        // Reset the buffer (not really needed since object gets deleted).
        _buffer.str("");
        _buffer.clear();
    }
}

// ----------------------------------------------------------------------------
char const* Logger::_get_color() const {

    switch (_level) {

    case Log_lvl::TRACE: return "\033[90m"; // Gray
    case Log_lvl::DEBUG: return "\033[36m"; // Cyan
    case Log_lvl::INFO: return "\033[32m"; // Green
    case Log_lvl::WARN: return "\033[33m"; // Yellow
    case Log_lvl::ERR: return "\033[31m"; // Red

    default: return "\033[0m";  // Reset color
    }
}

// ----------------------------------------------------------------------------
std::string Logger::_get_file() const {

    std::string const full_filepath = _location.file_name();

    // Find the last slash (handles both Windows and Unix paths)
    size_t last_slash = full_filepath.find_last_of("/\\");

    // If no slash found, use the entire path as filename
    if (last_slash == std::string::npos) {

        last_slash = 0u;
    } else {

        // Move past the slash
        ++last_slash;
    }

    std::string const file = full_filepath.substr(last_slash);

    return file;
}

// ----------------------------------------------------------------------------
std::string Logger::_curr_time() const {

    auto const time_point_now  = std::chrono::system_clock::now();
    auto const time_now        = std::chrono::system_clock::to_time_t(time_point_now);
    std::ostringstream time_stream{};

    // If we are using windows then use the non standard windows safer localtime_s function.
#ifdef _WIN32

        std::tm tm_info;
        localtime_s(&tm_info, &time_now);
        time_stream << std::put_time(&tm_info, "%H:%M:%S");

    // Otherwise use the standard localtime function.
#else

        time_stream << std::put_time(std::localtime(&time_now), "%H:%M:%S");
#endif

    return time_stream.str();
}

// ----------------------------------------------------------------------------
bool Logger::_is_log_lvl_enabled(Log_lvl const log_lvl) const {

    bool const is_enabled = (ENABLED_LOG_LVLS & log_lvl) == log_lvl;
    return     is_enabled;
}

} // namespace titan::utils
