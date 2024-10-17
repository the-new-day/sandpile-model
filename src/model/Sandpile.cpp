#include "model/Sandpile.hpp"

#include <iostream>

Sandpile::Sandpile(Grid& grid, const Parameters& parameters) {
    max_iterations_ = parameters.max_iterations;
    state_saving_frequency_ = parameters.state_saving_frequency;
    output_directory_ = parameters.output_directory;
    grid_ = grid;
}

void Sandpile::ToppleCell(int16_t x, int16_t y) {
    if (grid_.GetSand(x, y) < kCriticalSandNumber) {
        return;
    }

    uint64_t add_to_neighbour = kCriticalSandNumber / 4;

    grid_.AddSand(x + 1, y, add_to_neighbour);
    grid_.AddSand(x, y + 1, add_to_neighbour);
    grid_.AddSand(x - 1, y, add_to_neighbour);
    grid_.AddSand(x, y - 1, add_to_neighbour);

    grid_.RemoveSand(x, y, kCriticalSandNumber);
}

void Sandpile::FullyToppleCell(int16_t x, int16_t y) {
    if (grid_.GetSand(x, y) < kCriticalSandNumber) {
        return;
    }

    uint64_t add_to_neighbour = (grid_.GetSand(x, y) / kCriticalSandNumber) * kCriticalSandNumber / 4;
    
    grid_.AddSand(x + 1, y, add_to_neighbour);
    grid_.AddSand(x, y + 1, add_to_neighbour);
    grid_.AddSand(x - 1, y, add_to_neighbour);
    grid_.AddSand(x, y - 1, add_to_neighbour);

    grid_.SetSand(x, y, grid_.GetSand(x, y) % kCriticalSandNumber);
}

void Sandpile::ToppleGrid() {
    int16_t min_y = grid_.GetMinY();
    int16_t min_x = grid_.GetMinX();
    int16_t max_y = grid_.GetMaxY();
    int16_t max_x = grid_.GetMaxX();

    for (int16_t y = min_y; y <= max_y; ++y) {
        for (int16_t x = min_x; x <= max_x; ++x) {
            ToppleCell(x, y);
        }
    }
}

void Sandpile::FullyToppleGrid() {
    int16_t min_y = grid_.GetMinY();
    int16_t min_x = grid_.GetMinX();
    int16_t max_y = grid_.GetMaxY();
    int16_t max_x = grid_.GetMaxX();

    for (int16_t y = min_y; y <= max_y; ++y) {
        for (int16_t x = min_x; x <= max_x; ++x) {
            FullyToppleCell(x, y);
        }
    }
}

bool Sandpile::IsGridStable() const {
    for (int16_t y = grid_.GetMinY(); y <= grid_.GetMaxY(); ++y) {
        for (int16_t x = grid_.GetMinX(); x <= grid_.GetMaxX(); ++x) {
            if (grid_.GetSand(x, y) >= kCriticalSandNumber) {
                return false;
            }
        }
    }

    return true;
}

std::optional<SandpileError> Sandpile::Run() {
    size_t amount_of_iterations = 0;
    
    while (!IsGridStable()) {
        if (state_saving_frequency_ == 0 && max_iterations_ != 0) {
            FullyToppleGrid();
        } else {
            if (amount_of_iterations % state_saving_frequency_ == 0) {
                // save state
            }
            
            ToppleGrid();
        }

        if (max_iterations_ != 0 && max_iterations_ == amount_of_iterations) {
            break;
        }
        
        ++amount_of_iterations;
    }

    std::cout << "final:\n";
    grid_.PrintGrid();

    std::cout << grid_.GetWidth() << 'x' << grid_.GetHeight();

    return std::nullopt;
}
