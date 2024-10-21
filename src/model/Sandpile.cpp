#include "model/Sandpile.hpp"

#include <cstring>
#include <cstddef>

Sandpile::Sandpile(Grid& grid) : grid_(grid) {}

std::optional<SandpileError> Sandpile::SaveCurrentState(const char* filename) const {
    if (output_directory_ == nullptr) {
        return SandpileError{"Cannot save current state to a file: no output directory is specified"};
    }

    BmpWriter bmp_writer{grid_.GetWidth(), grid_.GetHeight(), kColorsUsed};
    bmp_writer.SetColor(SandColor::kWhite, kWhiteRGB);
    bmp_writer.SetColor(SandColor::kBlack, kBlackRGB);
    bmp_writer.SetColor(SandColor::kGreen, kGreenRGB);
    bmp_writer.SetColor(SandColor::kPurple, kPurpleRGB);
    bmp_writer.SetColor(SandColor::kYellow, kYellowRGB);

    for (int16_t y = grid_.GetMinY(); y <= grid_.GetMaxY(); ++y) {
        for (int16_t x = grid_.GetMinX(); x <= grid_.GetMaxX(); ++x) {
            uint64_t sand = grid_.GetSand(x, y);
            SandColor color = (sand >= critical_sand_number_) ? kBlack : SandColor(sand % critical_sand_number_);

            bmp_writer.SetPixel(x - grid_.GetMinX(), y - grid_.GetMinY(), color);
        }
    }

    size_t output_directory_length = std::strlen(output_directory_);
    size_t filename_length = std::strlen(filename);

    char* path = new char[output_directory_length + filename_length + 1];
    std::strcpy(path, output_directory_);
    std::strcat(path, filename);

    std::optional<BmpWriterError> saving_result = bmp_writer.Save(path);
    delete[] path;

    if (saving_result.has_value()) {
        return SandpileError{saving_result.value().message};
    }

    return std::nullopt;
}

void Sandpile::ToppleCell(int16_t x, int16_t y, uint64_t amount) {
    if (amount < critical_sand_number_ || grid_.GetSand(x, y) < critical_sand_number_) {
        return;
    }

    amount -= amount % 4;

    uint64_t add_to_neighbour = amount / 4;

    grid_.AddSand(x + 1, y, add_to_neighbour);
    grid_.AddSand(x, y + 1, add_to_neighbour);
    grid_.AddSand(x - 1, y, add_to_neighbour);
    grid_.AddSand(x, y - 1, add_to_neighbour);

    grid_.RemoveSand(x, y, amount);
}

void Sandpile::ToppleCell(int16_t x, int16_t y) {
    ToppleCell(x, y, critical_sand_number_);
}

void Sandpile::FullyToppleCell(int16_t x, int16_t y) {
    ToppleCell(x, y, grid_.GetSand(x, y) - (grid_.GetSand(x, y) % critical_sand_number_));
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
            if (grid_.GetSand(x, y) >= critical_sand_number_) {
                return false;
            }
        }
    }

    return true;
}

std::expected<uint64_t, SandpileError> Sandpile::Run(uint64_t max_iterations, uint64_t state_saving_frequency) {
    uint64_t amount_of_iterations = 0;
    
    while (!IsGridStable()) {
        if (max_iterations != 0 && max_iterations == amount_of_iterations) {
            break;
        }

        if (state_saving_frequency == 0 && max_iterations == 0) {
            FullyToppleGrid();
        } else if (output_directory_ == nullptr || state_saving_frequency == 0) {
            ToppleGrid();
        } else {
            if (amount_of_iterations % state_saving_frequency == 0) {
                // max length of size_t (decimal) is 20
                size_t filename_length = std::strlen(output_file_prefix_) + 20 + std::strlen(output_file_extension_) + 1;
                char* filename = new char[filename_length];

                std::sprintf(filename, "%s%u%s", output_file_prefix_, amount_of_iterations, output_file_extension_);

                std::optional<SandpileError> saving_result = SaveCurrentState(filename);
                delete[] filename;

                if (saving_result.has_value()) {
                    return std::unexpected{saving_result.value()};
                }
            }
            
            ToppleGrid();
        }
        
        ++amount_of_iterations;
    }

    if (output_directory_ != nullptr) {
        size_t filename_length = std::strlen(output_file_prefix_) + 5 + std::strlen(output_file_extension_) + 1;
        char* filename = new char[filename_length];
        std::sprintf(filename, "%sfinal%s", output_file_prefix_, output_file_extension_);

        std::optional<SandpileError> saving_result = SaveCurrentState(filename);
        delete[] filename;

        if (saving_result.has_value()) {
            return std::unexpected{saving_result.value()};
        }
    }

    return amount_of_iterations;
}

void Sandpile::SetOutputDirectory(const char* path) {
    output_directory_ = path;
}

void Sandpile::SetOutputFilePrefix(const char* prefix) {
    output_file_prefix_ = prefix;
}

void Sandpile::SetOutputFileExtension(const char* extension) {
    output_file_extension_ = extension;
}

void Sandpile::SaveStateToGrid(Grid& grid) const {
    grid = grid_;
}

void Sandpile::SetCriticalSandNumber(uint64_t number) {
    critical_sand_number_ = number;
}