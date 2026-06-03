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

public:
    BmpImageWriter(const std::string& filepath, unsigned int width, unsigned int height);
    ~BmpImageWriter();

    BmpImageWriter(const BmpImageWriter&) = delete;
    BmpImageWriter& operator=(const BmpImageWriter&) = delete;

    bool IsOpen() const;
    bool WriteRow(const std::vector<BmpPixel>& row);
    bool Close();
};
