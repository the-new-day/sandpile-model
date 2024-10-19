#include "parsing/tsv_parsing.hpp"
#include "parsing/utils.hpp"

#include <fstream>
#include <iostream>

const uint8_t kMaxUint64DecimalLength = 20;
const uint8_t kLineBufferSize = 64; // kMaxUint64DecimalLength * 3 + 3 '\t' + '\0'

std::optional<TsvParsingError> FillGrid(Grid& grid, const char* input_file_name) {
    std::ifstream file(input_file_name);
    if (!file.good()) {
        return TsvParsingError{"Unable to open the input file"};
    }

    char line_buffer[kLineBufferSize];

    while (file.good()) {
        file.getline(line_buffer, kLineBufferSize);

        if (file.fail()) {
            if (file.eof()) {
                return std::nullopt;
            }

            return TsvParsingError{"The line is too long and can't be processed"};
        }

        std::string_view line{line_buffer};

        if (std::count(line.data(), line.data() + line.length(), '\t') != 2) {
            return TsvParsingError{"The line doesn't contain exactly 2 tabs"};
        }

        std::string_view raw_x = line.substr(0, line.find('\t'));
        line = line.substr(line.find('\t') + 1);

        std::string_view raw_y = line.substr(0, line.find('\t'));
        std::string_view raw_sand = line.substr(line.find('\t') + 1);

        std::expected<int16_t, const char*> x = ParseNumber<int16_t>(raw_x);
        if (!x.has_value()) {
            return TsvParsingError{x.error()};
        }

        std::expected<int16_t, const char*> y = ParseNumber<int16_t>(raw_y);
        if (!y.has_value()) {
            return TsvParsingError{y.error()};
        }

        std::expected<uint64_t, const char*> sand = ParseNumber<uint64_t>(raw_sand);
        if (!sand.has_value()) {
            return TsvParsingError{sand.error()};
        }

        grid.SetSand(x.value(), y.value(), sand.value());
    }

    return std::nullopt;
}

