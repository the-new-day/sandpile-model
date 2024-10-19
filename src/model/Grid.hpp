#pragma once

#include <cstdint>

class Grid {
public:
    Grid() = default;

    Grid(const Grid& other);
    Grid& operator=(const Grid& other);
    
    ~Grid();

    uint64_t GetSand(int16_t x, int16_t y) const;
    void SetSand(int16_t x, int16_t y, uint64_t sand);
    void AddSand(int16_t x, int16_t y, uint64_t sand);
    void RemoveSand(int16_t x, int16_t y, uint64_t sand);

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

    int16_t GetMinX() const;
    int16_t GetMinY() const;
    int16_t GetMaxX() const;
    int16_t GetMaxY() const;

    bool IsEmpty() const;

    bool HasCell(int16_t x, int16_t y) const;

private:
    uint64_t** sand_ = nullptr;

    uint32_t width_ = 0;
    uint32_t height_ = 0;

    int16_t min_x_ = 0;
    int16_t min_y_ = 0;

    void Expand(uint32_t to_left, uint32_t to_top, uint32_t to_right, uint32_t to_bottom);
    void Reset();
};
