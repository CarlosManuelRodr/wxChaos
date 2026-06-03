#include "BmpImageWriter.h"

#include <limits>

namespace
{
    constexpr uint32_t BmpHeaderSize = 14;
    constexpr uint32_t DibHeaderSize = 40;
    constexpr uint32_t PixelDataOffset = BmpHeaderSize + DibHeaderSize;
    constexpr int32_t PixelsPerMeter72Dpi = 2835;

    bool TryCalculateLayout(const unsigned int width, const unsigned int height, unsigned int& paddingBytes, uint32_t& imageBytes, uint32_t& fileBytes)
    {
        if (width == 0 || height == 0)
            return false;

        if (width > static_cast<unsigned int>(std::numeric_limits<int32_t>::max()) ||
            height > static_cast<unsigned int>(std::numeric_limits<int32_t>::max()))
            return false;

        const uint64_t unpaddedRowBytes = static_cast<uint64_t>(width) * 3;
        paddingBytes = static_cast<unsigned int>((4 - (unpaddedRowBytes % 4)) % 4);
        const uint64_t rowBytes = unpaddedRowBytes + paddingBytes;
        const uint64_t totalImageBytes = rowBytes * height;
        const uint64_t totalFileBytes = PixelDataOffset + totalImageBytes;

        if (totalImageBytes > std::numeric_limits<uint32_t>::max() ||
            totalFileBytes > std::numeric_limits<uint32_t>::max())
            return false;

        imageBytes = static_cast<uint32_t>(totalImageBytes);
        fileBytes = static_cast<uint32_t>(totalFileBytes);
        return true;
    }
}

BmpImageWriter::BmpImageWriter(const std::string& filepath, const unsigned int width, const unsigned int height)
    : _width(width),
      _height(height),
      _paddingBytes(0),
      _rowsWritten(0),
      _ok(false)
{
    uint32_t imageBytes = 0;
    uint32_t fileBytes = 0;
    if (!TryCalculateLayout(_width, _height, _paddingBytes, imageBytes, fileBytes))
        return;

    _file.open(filepath.c_str(), std::ios::out | std::ios::binary);
    if (!_file.is_open())
        return;

    WriteUInt16Le(0x4D42);
    WriteUInt32Le(fileBytes);
    WriteUInt16Le(0);
    WriteUInt16Le(0);
    WriteUInt32Le(PixelDataOffset);

    WriteUInt32Le(DibHeaderSize);
    WriteInt32Le(static_cast<int32_t>(_width));
    WriteInt32Le(-static_cast<int32_t>(_height));
    WriteUInt16Le(1);
    WriteUInt16Le(24);
    WriteUInt32Le(0);
    WriteUInt32Le(imageBytes);
    WriteInt32Le(PixelsPerMeter72Dpi);
    WriteInt32Le(PixelsPerMeter72Dpi);
    WriteUInt32Le(0);
    WriteUInt32Le(0);

    _ok = _file.good();
}

BmpImageWriter::~BmpImageWriter()
{
    Close();
}

bool BmpImageWriter::IsOpen() const
{
    return _ok && _file.is_open();
}

bool BmpImageWriter::WriteRow(const std::vector<BmpPixel>& row)
{
    if (!IsOpen() || row.size() != _width || _rowsWritten >= _height)
        return false;

    for (const BmpPixel& pixel : row)
    {
        _file.put(static_cast<char>(pixel.b));
        _file.put(static_cast<char>(pixel.g));
        _file.put(static_cast<char>(pixel.r));
    }

    for (unsigned int i = 0; i < _paddingBytes; i++)
        _file.put(0);

    _rowsWritten++;
    MarkFailedIfNeeded();
    return _ok;
}

bool BmpImageWriter::Close()
{
    if (_file.is_open())
    {
        if (_rowsWritten != _height)
            _ok = false;

        _file.close();
        if (_file.fail())
            _ok = false;
    }

    return _ok && _rowsWritten == _height;
}

void BmpImageWriter::WriteUInt16Le(const uint16_t value)
{
    _file.put(static_cast<char>(value & 0xFF));
    _file.put(static_cast<char>((value >> 8) & 0xFF));
}

void BmpImageWriter::WriteUInt32Le(const uint32_t value)
{
    _file.put(static_cast<char>(value & 0xFF));
    _file.put(static_cast<char>((value >> 8) & 0xFF));
    _file.put(static_cast<char>((value >> 16) & 0xFF));
    _file.put(static_cast<char>((value >> 24) & 0xFF));
}

void BmpImageWriter::WriteInt32Le(const int32_t value)
{
    WriteUInt32Le(static_cast<uint32_t>(value));
}

void BmpImageWriter::MarkFailedIfNeeded()
{
    if (!_file.good())
        _ok = false;
}
