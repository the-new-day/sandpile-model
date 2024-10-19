#include "BmpWriter.hpp"

#include <fstream>

#include <iostream>

std::optional<BmpWriterError> BmpWriter::Save(const char* path) const {
    if (color_table_size_ < 2) {
        return BmpWriterError{"Unable to create a bmp file: color table size must be at least 2"};
    }

    std::ofstream file{path, std::ios::binary};

    if (file.fail()) {
        return BmpWriterError{"Unable to open the output file"};
    }

    BitmapHeader header;
    header.bf_type = 0x4D42; // Magic constant indicating the file format
    header.bf_size = GetFileSize();
    header.bf_reserved1 = 0;
    header.bf_reserved2 = 0;
    header.bf_off_bits = GetColorTableByteSize() + 54; // 54 == 14 (BITMAPFILEHEADER) + 40 (BITMAPINFO)

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

    WriteHeader(file, header);

    for (size_t i = 0; i < color_table_size_; ++i) {
        file << color_table_[i].blue << color_table_[i].green 
            << color_table_[i].red << uint8_t{0};
    }

    char* current_row = new char[GetRowByteSize()];
    uint64_t current_bit_in_row = 0;
    
    for (size_t y = 0; y < height_; ++y) {
        std::fill(current_row, current_row + GetRowByteSize(), 0);

        for (size_t x = 0; x < width_; ++x) {
            uint32_t color_table_index = pixel_table_indeces_[y * width_ + x];
            
            

            for (int32_t shift = GetBitCount() - 1; shift >= 0; --shift) {
                uint64_t current_byte_in_row = current_bit_in_row / 8;
                uint8_t current_color_index_bit = (color_table_index >> shift) & 1;

                current_row[current_byte_in_row] |= current_color_index_bit << (7 - current_bit_in_row % 8);
                ++current_bit_in_row;
            }
        }

        current_bit_in_row = 0;
        file.write(current_row, GetRowByteSize());
    }

    delete[] current_row;
    return std::nullopt;
}

void BmpWriter::WriteBytes(std::ofstream& file, uint32_t bytes) const {
    char byte_data[sizeof(bytes)];
    byte_data[0] = bytes & 0xff;
    byte_data[1] = (bytes >> 8) & 0xff;
    byte_data[2] = (bytes >> 16) & 0xff;
    byte_data[3] = (bytes >> 24) & 0xff;

    file.write(byte_data, sizeof(bytes));
}

void BmpWriter::WriteBytes(std::ofstream& file, uint16_t bytes) const {
    char byte_data[sizeof(bytes)];
    byte_data[0] = bytes & 0xff;
    byte_data[1] = (bytes >> 8) & 0xff;

    file.write(byte_data, sizeof(bytes));
}

void BmpWriter::WriteHeader(std::ofstream& file, const BitmapHeader& header) const {
    WriteBytes(file, header.bf_type);
    WriteBytes(file, header.bf_size);
    WriteBytes(file, header.bf_reserved1);
    WriteBytes(file, header.bf_reserved2);
    WriteBytes(file, header.bf_off_bits);
    WriteBytes(file, header.bi_size);
    WriteBytes(file, header.bi_width);
    WriteBytes(file, header.bi_height);
    WriteBytes(file, header.bi_planes);
    WriteBytes(file, header.bi_bit_count);
    WriteBytes(file, header.bi_compression);
    WriteBytes(file, header.bi_size_image);
    WriteBytes(file, header.bi_x_pels_per_meter);
    WriteBytes(file, header.bi_y_pels_per_meter);
    WriteBytes(file, header.bi_clr_used);
    WriteBytes(file, header.bi_clr_important);
}

uint32_t BmpWriter::GetColorTableByteSize() const {
    // 4 bytes for each color (32-bit RGBQUAD is used)
    return color_table_size_ * 4;
}

uint32_t BmpWriter::GetRowByteSize() const {
    uint32_t result = (width_ * GetBitCount() + 7) / 8;
    uint8_t padding = (4 - (result % 4)) % 4;

    return result + padding;
}

uint16_t BmpWriter::GetBitCount() const {
    if (color_table_size_ == 2) {
        return 1;
    } else if (color_table_size_ < 4) {
        return 2;
    } else if (color_table_size_ < 16) {
        return 4;
    }

    return 8;
}

uint64_t BmpWriter::GetFileSize() const {
    return sizeof(BitmapHeader) + GetRowByteSize() * height_;
}

std::optional<BmpWriterError> BmpWriter::SetPixel(uint32_t x, uint32_t y, uint8_t color_table_index) {
    uint64_t pixel_index = static_cast<uint64_t>(y) * width_ + x;

    if (pixel_index >= GetPixelDataSize()) {
        return BmpWriterError{"Pixel index is out of bounds"};
    } else if (color_table_index >= color_table_size_) {
        return BmpWriterError{"Color index is out of bounds"};
    }

    pixel_table_indeces_[pixel_index] = color_table_index;
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

BmpWriter::BmpWriter(uint32_t width, uint32_t height, uint8_t color_table_size) {
    width_ = width;
    height_ = height;
    color_table_size_ = color_table_size;

    color_table_ = new Color[color_table_size];
    pixel_table_indeces_ = new uint32_t[GetPixelDataSize()];
    std::fill(pixel_table_indeces_, pixel_table_indeces_ + GetPixelDataSize(), 0);
}

BmpWriter::BmpWriter(const BmpWriter& other) 
    : width_(other.width_), 
      height_(other.height_), 
      color_table_size_(other.color_table_size_),
      color_table_(new Color[other.color_table_size_]) {
    std::copy(other.color_table_, other.color_table_ + color_table_size_, color_table_);

    uint64_t pixel_data_size = static_cast<uint64_t>(width_) * height_;
    pixel_table_indeces_ = new uint32_t[pixel_data_size];
    std::copy(other.pixel_table_indeces_, other.pixel_table_indeces_ + pixel_data_size, pixel_table_indeces_);
}

BmpWriter& BmpWriter::operator=(const BmpWriter& other) {
    if (this == &other) {
        return *this;
    }

    Color* new_color_table = new Color[other.color_table_size_];
    std::copy(other.color_table_, other.color_table_ + other.color_table_size_, new_color_table);

    uint64_t pixel_data_size = static_cast<uint64_t>(other.width_) * other.height_;
    uint32_t* new_pixel_table_indeces = new uint32_t[pixel_data_size];
    std::copy(other.pixel_table_indeces_, other.pixel_table_indeces_ + pixel_data_size, new_pixel_table_indeces);

    delete[] color_table_;
    delete[] pixel_table_indeces_;

    width_ = other.width_;
    height_ = other.height_;
    color_table_size_ = other.color_table_size_;
    color_table_ = new_color_table;
    pixel_table_indeces_ = new_pixel_table_indeces;

    return *this;
}

BmpWriter::~BmpWriter() {
    delete[] color_table_;
    delete[] pixel_table_indeces_;
}
