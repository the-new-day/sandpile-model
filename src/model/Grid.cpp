#include "model/Grid.hpp"
#include "parsing/utils.hpp"

Grid::Grid(uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;

    uint64_t size = static_cast<uint64_t>(width) * height;

    uint64_t* sand_ = new uint64_t[size];
    std::fill(sand_, sand_ + size, 0);
}

void Grid::Expand(uint32_t to_left, uint32_t to_top, uint32_t to_right, uint32_t to_bottom) {
    uint32_t new_width = width_ + to_left + to_right;
    uint32_t new_height = height_ + to_top + to_bottom;
    uint64_t new_size = static_cast<uint64_t>(new_width) * new_height;

    uint64_t* new_sand = new uint64_t[new_size];
    std::fill(new_sand, new_sand + new_size, 0);

    for (size_t y = to_bottom; y < height_ + to_bottom; ++y) {
        for (size_t x = to_left; x < width_ + to_left; ++x) {
            uint64_t new_index = static_cast<uint64_t>(width_) * y + x;
            uint64_t old_index = static_cast<uint64_t>(y - to_bottom) * width_ + (x - to_left);

            new_sand[new_index] = sand_[old_index];
        }
    }

    DeleteGrid();
    width_ = new_width;
    height_ = new_height;
    sand_ = new_sand;
}

uint64_t Grid::GetSand(int16_t x, int16_t y) const {
    if (!HasCell(x, y)) {
        return 0;
    }

    return sand_[CoordsToIndex(x, y)];
}

void Grid::SetSand(int16_t x, int16_t y, uint64_t sand) {
    if (!HasCell(x, y)) {
        uint32_t to_left = (x < min_x) ? (min_x - x) : 0;
        uint32_t to_right = (x > max_x) ? (x - max_x) : 0;
        uint32_t to_top = (y > max_y) ? (y - max_y) : 0;
        uint32_t to_bottom = (y < min_y) ? (min_y - y) : 0;

        Expand(to_left, to_right, to_top, to_bottom);

        min_x = std::min(x, min_x);
        min_y = std::min(y, min_y);
        max_x = std::max(x, max_x);
        max_y = std::max(y, max_y);
    }

    sand_[CoordsToIndex(x, y)] = sand;
}

void Grid::AddSand(int16_t x, int16_t y, uint64_t sand) {
    SetSand(x, y, GetSand(x, y) + sand);
}

bool Grid::HasCell(int16_t x, int16_t y) const {
    if (width_ == 0 || height_ == 0) {
        return false;
    }

    return (x >= min_x && x <= max_x) && (y >= min_y && y <= max_y);
}

uint64_t Grid::CoordsToIndex(int16_t x, int16_t y) const {
    x -= min_x;
    y -= min_y;

    return y * width_ + x;
}

Grid& Grid::operator=(const Grid& other) {
    DeleteGrid();
    width_ = other.width_;
    height_ = other.height_;

    uint64_t new_size = static_cast<uint64_t>(width_) * height_;

    sand_ = new uint64_t[new_size];

    std::copy(sand_, sand_ + new_size, other.sand_);
    return *this;
}

Grid::Grid(const Grid& other) {
    DeleteGrid();
    width_ = other.width_;
    height_ = other.height_;

    uint64_t new_size = static_cast<uint64_t>(width_) * height_;

    sand_ = new uint64_t[new_size];

    std::copy(sand_, sand_ + new_size, other.sand_);
}

void Grid::DeleteGrid() {
    delete[] sand_;
    width_ = 0;
    height_ = 0;
}

Grid::~Grid() {
    DeleteGrid();
}

uint32_t Grid::GetWidth() const {
    return width_;
}

uint32_t Grid::GetHeight() const {
    return height_;
}
