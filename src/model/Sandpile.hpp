#pragma once

#include "parsing/argparsing.hpp"
#include "model/Grid.hpp"

class Sandpile {
public:
    Sandpile(Grid& grid, const Parameters& parameters);
    
    std::optional<const char*> Run();


private:
    Grid grid_;
    uint64_t state_saving_frequency_;
    uint64_t max_iterations_;
    const char* output_directory_;
};
