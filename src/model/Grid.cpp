#include "model/Grid.hpp"
#include "parsing/utils.hpp"

#include <iostream>

void Grid::Expand(uint32_t to_left, uint32_t to_top, uint32_t to_right, uint32_t to_bottom) {
    if (to_left == 0 && to_top == 0 && to_right == 0 && to_bottom == 0) {
        return;
    }

    uint32_t new_width = width_ + to_left + to_right;
    uint32_t new_height = height_ + to_top + to_bottom;

    uint64_t** new_sand = new uint64_t*[new_height];
    for (size_t y = 0; y < new_height; ++y) {
        new_sand[y] = new uint64_t[new_width];
        std::fill(new_sand[y], new_sand[y] + new_width, 0);
    }

    for (size_t y = to_bottom; y < height_ + to_bottom; ++y) {
        for (size_t x = to_left; x < width_ + to_left; ++x) {
            new_sand[y][x] = sand_[y - to_bottom][x - to_left];
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

    return sand_[y - min_y_][x - min_x_];
}

bool Grid::IsEmpty() const {
    return width_ == 0;
}

void Grid::SetSand(int16_t x, int16_t y, uint64_t sand) {
    if (IsEmpty()) {
        min_x_ = x;
        min_y_ = y;
    }
    
    // expand
    uint32_t to_left = 0;
    uint32_t to_bottom = 0;
    uint32_t to_top = 0;
    uint32_t to_right = 0;

    if (x < min_x_) {
        to_left = min_x_ - x;
        min_x_ = x;
    }

    if (y < min_y_) {
        to_bottom = min_y_ - y;
        min_y_ = y;
    }

    y -= min_y_;
    x -= min_x_;

    if (x >= width_) {
        to_right = x - width_ + 1;
    }

    if (y >= height_) {
        to_top = y - height_ + 1;
    }
    
    Expand(to_left, to_top, to_right, to_bottom);
    sand_[y][x] = sand;
}

// TODO: remove (created for debug)
void Grid::PrintGrid() const {
    for (int16_t y = height_ - 1; y >= 0; --y) {
        for (int16_t x = 0; x < width_; ++x) {
            std::cout << sand_[y][x] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << '\n';
}

void Grid::AddSand(int16_t x, int16_t y, uint64_t sand) {
    SetSand(x, y, GetSand(x, y) + sand);
}

void Grid::RemoveSand(int16_t x, int16_t y, uint64_t sand) {
    if (GetSand(x, y) < sand) {
        SetSand(x, y, 0);
    } else {
        SetSand(x, y, GetSand(x, y) - sand);
    }
}

bool Grid::HasCell(int16_t x, int16_t y) const {
    if (IsEmpty()) {
        return false;
    }

    return (x >= min_x_ && x < min_x_ + static_cast<int16_t>(width_)) 
        && (y >= min_y_ && y < min_y_ + static_cast<int16_t>(height_));
}

Grid& Grid::operator=(const Grid& other) {
    DeleteGrid();

    width_ = other.width_;
    height_ = other.height_;
    min_x_ = other.min_x_;
    min_y_ = other.min_y_;

    uint64_t other_size = static_cast<uint64_t>(other.width_) * other.height_;

    sand_ = new uint64_t*[other.height_];
    for (size_t y = 0; y < other.height_; ++y) {
        sand_[y] = new uint64_t[other.width_];
        std::copy(other.sand_[y], other.sand_[y] + other.width_, sand_[y]);
    }

    return *this;
}

Grid::Grid(const Grid& other) {
    *this = other;
}

void Grid::DeleteGrid() {
    for (size_t y = 0; y < height_; ++y) {
        delete[] sand_[y];
    }

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

int16_t Grid::GetMinX() const {
    return min_x_;
}

int16_t Grid::GetMinY() const {
    return min_y_;
}

int16_t Grid::GetMaxX() const {
    return min_x_ + width_ - 1;
}

int16_t Grid::GetMaxY() const {
    return min_y_ + height_ - 1;
}
