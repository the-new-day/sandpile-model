#pragma once

#include "parsing/argparsing.hpp"
#include "model/Grid.hpp"
#include "bmp/BmpWriter.hpp"

const size_t kColorsUsed = 5;

enum SandColor {
    kWhite = 0,
    kGreen = 1,
    kPurple = 2,
    kYellow = 3,
    kBlack = 4
};

const Color kWhiteRGB{255, 255, 255};
const Color kGreenRGB{20, 181, 31};
const Color kPurpleRGB{146, 42, 191};
const Color kYellowRGB{186, 186, 34};
const Color kBlackRGB{0, 0, 0};

struct SandpileError {
    const char* message = nullptr;
};

class Sandpile {
public:
    static const uint8_t kCriticalSandNumber = 4;

    Sandpile(const Grid& initial_grid);

    void SetOutputDirectory(const char* path);
    void SetOutputFilePrefix(const char* prefix);
    void SetOutputFileExtension(const char* extension);
    void SetCriticalSandNumber(uint64_t number);
    
    std::expected<uint64_t, SandpileError> Run(uint64_t max_iterations = 0, uint64_t state_saving_frequency = 0);

    void ToppleGrid();
    void ToppleCell(int16_t x, int16_t y);
    void ToppleCell(int16_t x, int16_t y, uint64_t amount);

    /** Saves current state to a bmp file */
    std::optional<SandpileError> SaveCurrentState(const char* filename) const;

    void SaveStateToGrid(Grid& grid) const;

    bool IsGridStable() const;

private:
    Grid grid_;

    void FullyToppleCell(int16_t x, int16_t y);
    void FullyToppleGrid();

    uint64_t critical_sand_number_ = 4;

    const char* output_file_prefix_ = "sandpile_";
    const char* output_file_extension_ = ".bmp";

    const char* output_directory_ = nullptr;
};
