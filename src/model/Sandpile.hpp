#pragma once

#include "parsing/argparsing.hpp"
#include "model/Grid.hpp"
#include "bmp/BmpWriter.hpp"

#include <cstddef>

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
    explicit Sandpile(Grid& grid);

    void SetOutputDirectory(const char* path);
    void SetOutputFilePrefix(const char* prefix);
    void SetOutputFileExtension(const char* extension);
    void SetCriticalSandNumber(uint64_t number);
    
    /**
     * Runs the model: topples all cells until either 
     * the grid is stable or max_iterations is reached (if not 0).
     * 
     * If intermediate states have to be calculated, only a %critical_sand_number% grains of sand
     * gets toppled on each iteration.
     * 
     * If no intermediate states are needed (max_iteration == state_saving_frequency == 0),
     * a more efficient algorithm is used, so the number of iterations is much less
     * 
     * @param max_iterations Maximum number of iterations
     * @param state_saving_frequency Frequency of saving intermediate states to a file.
     * For saving to a file, the output directory must be set
     * @return Amount of iterations if the run was successful, error otherwise
     */
    std::expected<uint64_t, SandpileError> Run(
        uint64_t max_iterations = 0,
        uint64_t state_saving_frequency = 0);

    /** Performs one iteration of running the model: critical amount of sand is toppled from each cell */
    void ToppleGrid();

    void ToppleCell(int16_t x, int16_t y);
    void ToppleCell(int16_t x, int16_t y, uint64_t amount);

    /** Saves current state to a bmp file */
    std::optional<SandpileError> SaveCurrentState(const char* filename) const;

    void SaveStateToGrid(Grid& grid) const;

    /** Checks if each grid cell contains less grains of sand than a critical amount of sand */
    bool IsGridStable() const;

private:
    Grid& grid_;

    void FullyToppleCell(int16_t x, int16_t y);
    void FullyToppleGrid();

    uint64_t critical_sand_number_ = 4;

    const char* output_file_prefix_ = "sandpile_";
    const char* output_file_extension_ = ".bmp";

    const char* output_directory_ = nullptr;
};