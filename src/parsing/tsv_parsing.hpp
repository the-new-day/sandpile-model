#pragma once

#include "model/Grid.hpp"

#include <optional>

struct TsvParsingError {
    const char* message = nullptr;
};

std::optional<TsvParsingError> FillGrid(Grid& grid, const char* input_file_name);
