#pragma once

#include <cstdint>
#include <optional>
#include <fstream>

// RGB color
struct Color {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct BmpWriterError {
    const char* message = nullptr;
};

/**
 * Class to generate .bmp files.
 * Uses color table, so only supports 1, 2, 4 and 8 bits per pixel
 */
class BmpWriter {
public:
    BmpWriter(uint32_t width, uint32_t height, uint8_t color_table_size);

    ~BmpWriter();
    BmpWriter(const BmpWriter& other);
    BmpWriter& operator=(const BmpWriter& other);

    /** Adds color to the color table  */
    std::optional<BmpWriterError> SetColor(uint32_t table_index, Color color);
    std::optional<BmpWriterError> SetPixel(uint32_t x, uint32_t y, uint8_t color_table_index);

    std::optional<BmpWriterError> Save(const char* path) const;

    uint64_t GetPixelDataSize() const;
    uint64_t GetFileSize() const;
    uint32_t GetRowByteSize() const;
    uint32_t GetColorTableByteSize() const;
    uint16_t GetBitCount() const;

private:
    Color* color_table_ = nullptr;
    uint32_t* pixel_table_indeces_ = nullptr;

    uint32_t width_;
    uint32_t height_;
    uint8_t color_table_size_;

    #pragma pack(push, 2)
    struct BitmapHeader {
        /* BITMAPFILEHEADER */
        uint16_t bf_type;
        uint32_t bf_size;
        uint16_t bf_reserved1;
        uint16_t bf_reserved2;
        uint32_t bf_off_bits;

        /* BITMAPINFO */
        uint32_t bi_size;
        uint32_t bi_width;
        uint32_t bi_height;
        uint16_t bi_planes;
        uint16_t bi_bit_count;
        uint32_t bi_compression;
        uint32_t bi_size_image;
        uint32_t bi_x_pels_per_meter;
        uint32_t bi_y_pels_per_meter;
        uint32_t bi_clr_used;
        uint32_t bi_clr_important;
    };
    #pragma pack(pop)

    void WriteHeader(std::ofstream& file, const BitmapHeader& header) const;

    void WriteBytes(std::ofstream& file, uint32_t bytes) const;
    void WriteBytes(std::ofstream& file, uint16_t bytes) const;
};
