#pragma once

#include <expected>
#include <optional>
#include <string_view>
#include <charconv>

template<typename T>
std::expected<T, const char*> ParseNumber(std::string_view str) {
    T result;
    std::from_chars_result convertion_result = std::from_chars(str.data(), str.data() + str.size(), result);

    if (convertion_result.ec == std::errc::invalid_argument || convertion_result.ptr != str.end()) {
        return std::unexpected{"Cannot parse an integer from non-numeric data"};
    } else if (convertion_result.ec == std::errc::result_out_of_range) {
        return std::unexpected{"The number is too large"};
    }

    return result;
}
