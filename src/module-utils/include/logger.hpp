#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <ostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Enums
// ----------------------------------------------------------------------------

/**
 * @brief Logging severity levels.
 *
 * Each level is a bit flag, allowing bitwise combinations for filtering.
 * Levels are ordered from most verbose (TRACE) to most critical (ERROR).
 */
enum class Log_lvl : std::uint8_t {

    TRACE = 1u << 0u,  ///< Detailed trace information for debugging
    DEBUG = 1u << 1u,  ///< Debug-level messages for development
    INFO  = 1u << 2u,  ///< Informational messages about program flow
    WARN  = 1u << 3u,  ///< Warning messages for potentially problematic situations
    ERR   = 1u << 4u,  ///< Error messages for failures and exceptions
};

/**
 * @brief Stream insertion operator for Log_lvl.
 * @param os Output stream
 * @param log_lvl Log level to output
 * @return Reference to the output stream
 */
inline std::ostream& operator<<(
    std::ostream& os,
    Log_lvl const log_lvl
    ) {

    switch (log_lvl) {

        case Log_lvl::TRACE: return os << "TRACE";
        case Log_lvl::DEBUG: return os << "DEBUG";
        case Log_lvl::INFO:  return os << "INFO";
        case Log_lvl::WARN:  return os << "WARN";
        case Log_lvl::ERR: return os << "ERROR";

        default: return os << "UNKNOWN LOG LVL";
    }
}

/**
 * @brief Bitwise OR operator for combining log levels.
 * @param left First log level
 * @param right Second log level
 * @return Combined bit mask
 */
inline std::uint8_t operator|(
    Log_lvl const left,
    Log_lvl const right
    ) {

    return static_cast<std::uint8_t>(left) | static_cast<std::uint8_t>(right);
}

/**
 * @brief Bitwise AND operator for testing log levels.
 * @param left First log level
 * @param right Second log level
 * @return Intersected bit mask
 */
inline std::uint8_t operator&(
    Log_lvl const left,
    Log_lvl const right
    ) {

    return static_cast<std::uint8_t>(left) & static_cast<std::uint8_t>(right);
}

/**
 * @brief Bitwise OR operator for combining bit mask with log level.
 * @param left Existing bit mask
 * @param right Log level to add
 * @return Combined bit mask
 */
inline std::uint8_t operator|(
    std::uint8_t const left,
    Log_lvl const right
    ) {

    return left | static_cast<std::uint8_t>(right);
}

/**
 * @brief Bitwise AND operator for testing bit mask against log level.
 * @param left Bit mask to test
 * @param right Log level to check
 * @return Intersected bit mask
 */
inline std::uint8_t operator&(
    std::uint8_t const left,
    Log_lvl const right
    ) {

    return left & static_cast<std::uint8_t>(right);
}

/**
 * @brief Equality operator for comparing bit mask with log level.
 * @param left Bit mask
 * @param right Log level
 * @return True if the mask equals the log level's bit value
 */
inline bool operator==(
    std::uint8_t const left,
    Log_lvl const right
    ) {

    return left == static_cast<std::uint8_t>(right);
}

// ============================================================================
// Class Logger
// ----------------------------------------------------------------------------

/**
 * @brief Thread-safe logger with source location tracking and colored output.
 *
 * Logger uses RAII to buffer messages and flush them atomically on destruction.
 * Each log message includes timestamp, severity, file location, and supports
 * stream-style formatting via operator<<.
 */
class Logger final {

public:
    /**
     * @brief Construct a logger with the given severity level.
     * @param level Severity level of this log message
     * @param loc Source location (automatically captured by LOG macro)
     */
    Logger(Log_lvl const level, std::source_location const loc);

    /**
     * @brief Destructor flushes buffered message to output.
     */
    ~Logger();

    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * @brief Bit mask controlling which log levels are enabled.
     */
    static std::uint8_t ENABLED_LOG_LVLS;

private:
    /**
     * @brief Flush buffered content to stderr with formatting.
     */
    void _flush();

    /**
     * @brief Get ANSI color code for current log level.
     * @return Color escape sequence string
     */
    const char* _get_color() const;

    /**
     * @brief Extract filename from full source path.
     * @return Basename of the source file
     */
    std::string _get_file() const;

    /**
     * @brief Get current time as formatted string.
     * @return Timestamp in format "YYYY-MM-DD HH:MM:SS"
     */
    std::string _curr_time() const;

    /**
     * @brief Check if a given log level is currently enabled.
     * @param log_lvl Level to check
     * @return True if level is enabled in ENABLED_LOG_LVLS mask
     */
    bool _is_log_lvl_enabled(Log_lvl const log_lvl) const;

    Log_lvl _level;                    ///< Severity level of this log message
    std::source_location _location;    ///< Source code location of log call
    std::ostringstream _buffer;        ///< Buffered message content
    static std::mutex s_mutex;         ///< Mutex for thread-safe output

public:
    /**
     * @brief Stream insertion operator for building log messages.
     * @tparam Input_type Type of value to log (must support operator<<)
     * @param value Value to append to log message
     * @return Reference to this Logger for chaining
     */
    template<typename Input_type>
    Logger& operator<<(Input_type const& value) {

        _buffer << value;
        return *this;
    }
};

} // namespace titan::utils

// ============================================================================
// Macros
// ----------------------------------------------------------------------------

/**
 * @brief Create a Logger instance with automatic source location capture.
 * @param level Log severity level (Log_lvl::TRACE, DEBUG, INFO, WARN, ERROR)
 */
#define LOG(level) titan::utils::Logger(level, std::source_location::current())

/**
 * @brief Set which log levels are enabled at runtime.
 * @param level_mask Bit mask of enabled levels (combine with | operator)
 */
#define LOG_LVLS_ENABLED(level_mask) titan::utils::Logger::ENABLED_LOG_LVLS = level_mask
