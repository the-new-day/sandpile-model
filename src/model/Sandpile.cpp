#include "model/Sandpile.hpp"

Sandpile::Sandpile(Grid& grid, const Parameters& parameters) {
    max_iterations_ = parameters.max_iterations;
    state_saving_frequency_ = parameters.state_saving_frequency;
    output_directory_ = parameters.output_directory;
    grid_ = grid;
}

std::optional<const char*> Sandpile::Run() {
    return std::nullopt;
}
