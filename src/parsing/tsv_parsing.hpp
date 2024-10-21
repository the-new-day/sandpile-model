#pragma once

#include "model/Grid.hpp"

#include <optional>

struct TsvParsingError {
    const char* message = nullptr;
    uint64_t line = 0;
};

std::optional<TsvParsingError> FillGrid(Grid& grid, const char* input_file_name);
