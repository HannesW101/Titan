#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <algorithm>
#include <cctype>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Split
// ----------------------------------------------------------------------------

/**
 * @brief Split string by single character delimiter.
 * @param input String to split
 * @param delimiter Character to split on
 * @return Vector of substrings
 */
[[nodiscard]] inline std::vector<std::string> split(
    std::string_view const input,
    char const delimiter
    ) {

    std::vector<std::string> result;
    std::size_t start = 0u;
    std::size_t end   = 0u;

    while ((end = input.find(delimiter, start)) != std::string_view::npos) {

        result.emplace_back(input.substr(start, end - start));
        start = end + 1u;
    }

    result.emplace_back(input.substr(start));

    return result;
}

/**
 * @brief Split string by multi-character delimiter.
 * @param input String to split
 * @param delimiter String to split on
 * @return Vector of substrings
 */
[[nodiscard]] inline std::vector<std::string> split(
    std::string_view const input,
    std::string_view const delimiter
    ) {

    std::vector<std::string> result;
    std::size_t start = 0u;
    std::size_t end   = 0u;

    while ((end = input.find(delimiter, start)) != std::string_view::npos) {

        result.emplace_back(input.substr(start, end - start));
        start = end + delimiter.length();
    }

    result.emplace_back(input.substr(start));

    return result;
}

// ============================================================================
// Trim
// ----------------------------------------------------------------------------

/**
 * @brief Remove leading whitespace from string.
 * @param s String to trim
 * @return View of string without leading whitespace
 */
[[nodiscard]] inline std::string_view trim_left(
    std::string_view const s
    ) {

    std::size_t const start = s.find_first_not_of(" \t\n\r\f\v");

    return (start == std::string_view::npos) ? "" : s.substr(start);
}

/**
 * @brief Remove trailing whitespace from string.
 * @param s String to trim
 * @return View of string without trailing whitespace
 */
[[nodiscard]] inline std::string_view trim_right(
    std::string_view const s
    ) {

    std::size_t const end = s.find_last_not_of(" \t\n\r\f\v");

    return (end == std::string_view::npos) ? "" : s.substr(0u, end + 1u);
}

/**
 * @brief Remove leading and trailing whitespace from string.
 * @param s String to trim
 * @return View of string without leading or trailing whitespace
 */
[[nodiscard]] inline std::string_view trim(
    std::string_view const s
    ) {

    return trim_left(trim_right(s));
}

// ============================================================================
// String matching
// ----------------------------------------------------------------------------

/**
 * @brief Check if string starts with given prefix.
 * @param s String to check
 * @param prefix Prefix to search for
 * @return True if s starts with prefix
 */
[[nodiscard]] inline bool starts_with(
    std::string_view const s,
    std::string_view const prefix
    ) {

    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

/**
 * @brief Check if string ends with given suffix.
 * @param s String to check
 * @param suffix Suffix to search for
 * @return True if s ends with suffix
 */
[[nodiscard]] inline bool ends_with(
    std::string_view const s,
    std::string_view const suffix
    ) {

    return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

/**
 * @brief Check if string contains substring.
 * @param s String to search in
 * @param needle Substring to search for
 * @return True if needle is found in s
 */
[[nodiscard]] inline bool contains(
    std::string_view const s,
    std::string_view const needle
    ) {

    return s.find(needle) != std::string_view::npos;
}

// ============================================================================
// Case conversion
// ----------------------------------------------------------------------------

/**
 * @brief Convert string to lowercase.
 * @param s String to convert
 * @return New string with all characters lowercase
 */
[[nodiscard]] inline std::string to_lower(
    std::string_view const s
    ) {

    std::string result(s);

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char const c) { return std::tolower(c); }
        );

    return result;
}

/**
 * @brief Convert string to uppercase.
 * @param s String to convert
 * @return New string with all characters uppercase
 */
[[nodiscard]] inline std::string to_upper(
    std::string_view const s
    ) {

    std::string result(s);

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char const c) { return std::toupper(c); }
        );

    return result;
}

// ============================================================================
// Join
// ----------------------------------------------------------------------------

/**
 * @brief Join strings with separator.
 * @param parts Strings to join
 * @param separator String inserted between parts
 * @return Single joined string
 */
[[nodiscard]] inline std::string join(
    std::span<std::string const> const parts,
    std::string_view const separator
    ) {

    if (parts.empty()) { return ""; }

    std::string result;
    std::size_t total_size = 0u;

    for (auto const& part : parts) {

        total_size += part.size();
    }

    total_size += separator.size() * (parts.size() - 1u);
    result.reserve(total_size);

    result += parts[0];

    for (std::size_t i = 1u; i < parts.size(); ++i) {

        result += separator;
        result += parts[i];
    }

    return result;
}

// ============================================================================
// Replace
// ----------------------------------------------------------------------------

/**
 * @brief Replace all occurrences of substring with another string.
 * @param s String to perform replacements in
 * @param from Substring to find
 * @param to Replacement string
 * @return New string with all replacements applied
 */
[[nodiscard]] inline std::string replace_all(
    std::string s,
    std::string_view const from,
    std::string_view const to
    ) {

    if (from.empty()) { return s; }

    std::size_t pos = 0u;

    while ((pos = s.find(from, pos)) != std::string::npos) {

        s.replace(pos, from.length(), to);
        pos += to.length();
    }

    return s;
}

} // namespace titan::utils
