#pragma once

#include "parsing/argparsing.hpp"
#include "model/Grid.hpp"

struct SandpileError {
    const char* message = nullptr;
};

class Sandpile {
public:
    static const uint8_t kCriticalSandNumber = 4;

    Sandpile(Grid& grid, const Parameters& parameters);
    
    std::optional<SandpileError> Run();

    void ToppleGrid();
    void ToppleCell(int16_t x, int16_t y);

    void SaveCurrentState() const;

    bool IsGridStable() const;

private:
    Grid grid_;

    void FullyToppleCell(int16_t x, int16_t y);
    void FullyToppleGrid();

    uint64_t state_saving_frequency_;
    uint64_t max_iterations_;
    const char* output_directory_;
};
