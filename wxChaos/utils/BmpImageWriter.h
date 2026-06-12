#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

struct BmpPixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class BmpImageWriter
{
    static constexpr uint32_t BmpHeaderSize = 14;
    static constexpr uint32_t DibHeaderSize = 40;
    static constexpr uint32_t PixelDataOffset = BmpHeaderSize + DibHeaderSize;
    static constexpr int32_t PixelsPerMeter72Dpi = 2835;

    std::ofstream _file;
    unsigned int _width;
    unsigned int _height;
    unsigned int _paddingBytes;
    unsigned int _rowsWritten;
    bool _ok;

    void WriteUInt16Le(uint16_t value);
    void WriteUInt32Le(uint32_t value);
    void WriteInt32Le(int32_t value);
    void MarkFailedIfNeeded();
    static bool TryCalculateLayout(unsigned int width, unsigned int height, unsigned int& paddingBytes, uint32_t& imageBytes, uint32_t& fileBytes);

public:
    BmpImageWriter(const std::string& filepath, unsigned int width, unsigned int height);
    ~BmpImageWriter();

    BmpImageWriter(const BmpImageWriter&) = delete;
    BmpImageWriter& operator=(const BmpImageWriter&) = delete;

    [[nodiscard]] bool IsOpen() const;
    bool WriteRow(const std::vector<BmpPixel>& row);
    bool Close();
};
