#include "BmpWriter.hpp"

#include <algorithm>
#include <fstream>
#include <cmath>

#include <iostream>

// "Magic word" to be written in the bf_type field ("BM")
const uint16_t kBfType = 0x4D42;

std::optional<BmpWriterError> BmpWriter::Save(const char* filename) const {
    if (color_table_size_ < 2) {
        return BmpWriterError{"Unable to create a .bmp file: color table size must be at least 2"};
    }

    std::ofstream file{filename};

    if (file.fail()) {
        return BmpWriterError{"Unable to open the output file"};
    }

    BitmapHeader header;
    header.bf_type = kBfType;
    header.bf_size = GetFileSize();
    header.bf_reserved1 = 0;
    header.bf_reserved2 = 0;
    header.bf_off_bits = sizeof(BitmapHeader) + GetColorTableByteSize();

    header.bi_size = 40;
    header.bi_width = width_;
    header.bi_height = height_;
    header.bi_planes = 1;
    header.bi_bit_count = GetBitCount();
    header.bi_compression = 0;
    header.bi_size_image = GetRowByteSize() * height_;
    header.bi_x_pels_per_meter = 0;
    header.bi_y_pels_per_meter = 0;
    header.bi_clr_used = color_table_size_;
    header.bi_clr_important = 0;

    file.write(reinterpret_cast<const char*>(&header), sizeof(BitmapHeader));

    for (size_t i = 0; i < color_table_size_; ++i) {
        file << color_table_[i].red << color_table_[i].green << color_table_[i].blue << static_cast<uint8_t>(0);
    }

    char* current_row = new char[GetRowByteSize()];
    uint8_t current_bit_in_row = 0;

    for (size_t y = 0; y < height_; ++y) {
        std::fill(current_row, current_row + GetRowByteSize(), 0);

        for (size_t x = 0; x < width_; ++x) {
            uint32_t color_table_index = pixels_[y * width_ + x];

            for (int32_t shift = GetBitCount() - 1; shift >= 0; --shift) {
                uint8_t current_byte_in_row = current_bit_in_row / 8;
                uint8_t current_color_index_bit = (color_table_index >> shift) & 1;

                current_row[current_byte_in_row] |= current_color_index_bit << (7 - current_bit_in_row % 8);
                ++current_bit_in_row;
            }
        }

        file.write(current_row, GetRowByteSize());
    }

    delete[] current_row;
    return std::nullopt;
}

uint64_t BmpWriter::GetColorTableByteSize() const {
    // 4 bytes for each color (32-bit RGBQUAD is used)
    return color_table_size_ * 4;
}

uint8_t BmpWriter::GetRowPadding() const {
    return 4 - (width_ * GetBitCount() / 8) % 4;
}

uint64_t BmpWriter::GetRowByteSize() const {
    uint64_t result = width_ * GetBitCount() / 8;
    result += 4 - result % 4;

    return result;
}

uint16_t BmpWriter::GetBitCount() const {
    if (color_table_size_ == 2) {
        // monochrome image, 1 bit is enough
        return 1;
    }

    uint16_t result = std::ceil(std::log2(color_table_size_));
    return result + (4 - result % 4);
}

uint64_t BmpWriter::GetFileSize() const {
    return sizeof(BitmapHeader) + GetRowByteSize() * height_;
}

std::optional<BmpWriterError> BmpWriter::SetPixel(uint32_t x, uint32_t y, uint32_t color_table_index) {
    uint64_t pixel_index = static_cast<uint64_t>(y) * width_ + x;

    if (pixel_index >= GetPixelDataSize()) {
        return BmpWriterError{"Pixel index is out of bounds"};
    } else if (color_table_index >= color_table_size_) {
        return BmpWriterError{"Color index is out of bounds"};
    }

    pixels_[pixel_index] = color_table_index;

    return std::nullopt;
}

std::optional<BmpWriterError> BmpWriter::SetColor(uint32_t table_index, Color color) {
    if (table_index >= color_table_size_) {
        return BmpWriterError{"Color index is out of bounds"};
    }

    color_table_[table_index] = color;

    return std::nullopt;
}

uint64_t BmpWriter::GetPixelDataSize() const {
    return static_cast<uint64_t>(width_) * height_;
}

BmpWriter::BmpWriter(uint32_t width, uint32_t height, uint32_t color_table_size) {
    width_ = width;
    height_ = height;
    color_table_size_ = color_table_size;

    color_table_ = new Color[color_table_size];
    pixels_ = new uint32_t[GetPixelDataSize()];
    std::fill(pixels_, pixels_ + GetPixelDataSize(), 0);
}

BmpWriter::BmpWriter(const BmpWriter& other) {
    *this = other;
}

BmpWriter& BmpWriter::operator=(const BmpWriter& other) {
    delete[] color_table_;
    delete[] pixels_;

    width_ = other.width_;
    height_ = other.height_;
    color_table_size_ = other.color_table_size_;

    color_table_ = new Color[color_table_size_];
    std::copy(other.color_table_, other.color_table_ + color_table_size_, color_table_);

    uint64_t pixel_data_size = static_cast<uint64_t>(width_) * height_;
    pixels_ = new uint32_t[pixel_data_size];
    std::copy(other.pixels_, other.pixels_ + pixel_data_size, pixels_);

    return *this;
}

BmpWriter::~BmpWriter() {
    delete[] color_table_;
    delete[] pixels_;
}
