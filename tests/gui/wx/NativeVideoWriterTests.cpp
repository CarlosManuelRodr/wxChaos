#include <doctest/doctest.h>
#include "export/NativeVideoWriter.h"

#ifdef _WIN32
#include <windows.h>
#include <cstdint>
#include <filesystem>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>
#endif

TEST_CASE("Native video encoding defaults favor high visual quality at balanced speed")
{
    const NativeVideoEncodingOptions options;
    CHECK(options.quality == 80U);
    CHECK(options.speed == NativeVideoEncodingSpeed::Balanced);
}

TEST_CASE("Native video encoding speed values match the Zoom Recorder choice order")
{
    CHECK(static_cast<unsigned int>(NativeVideoEncodingSpeed::Fast) == 0U);
    CHECK(static_cast<unsigned int>(NativeVideoEncodingSpeed::Balanced) == 1U);
    CHECK(static_cast<unsigned int>(NativeVideoEncodingSpeed::Slow) == 2U);
}

TEST_CASE("Native video quality maps deterministically to bounded H.264 quantizers")
{
    CHECK(NativeVideoWriter::GetH264Quantizer(0U) == 51U);
    CHECK(NativeVideoWriter::GetH264Quantizer(1U) == 51U);
    CHECK(NativeVideoWriter::GetH264Quantizer(70U) == 24U);
    CHECK(NativeVideoWriter::GetH264Quantizer(80U) == 20U);
    CHECK(NativeVideoWriter::GetH264Quantizer(100U) == 12U);
    CHECK(NativeVideoWriter::GetH264Quantizer(101U) == 12U);
}

#ifdef _WIN32
TEST_CASE("Windows native video writer applies quality before encoder negotiation")
{
    const std::filesystem::path lowQualityPath =
        std::filesystem::temp_directory_path() / "wxchaos-low-quality-vbr-test.mp4";
    const std::filesystem::path highQualityPath =
        std::filesystem::temp_directory_path() / "wxchaos-high-quality-vbr-test.mp4";
    std::error_code fileError;
    std::filesystem::remove(lowQualityPath, fileError);
    std::filesystem::remove(highQualityPath, fileError);

    sf::Image frame;
    frame.create(320U, 240U);
    const auto encode = [&frame](const std::filesystem::path& outputPath, const unsigned int quality)
    {
        NativeVideoWriter writer;
        const bool opened = writer.Open(
            outputPath.string(), 320U, 240U, 30U, {quality, NativeVideoEncodingSpeed::Slow});
        INFO(writer.GetError());
        REQUIRE(opened);

        for (unsigned int frameIndex = 0; frameIndex < 12U; ++frameIndex)
        {
            for (unsigned int y = 0; y < 240U; ++y)
            {
                for (unsigned int x = 0; x < 320U; ++x)
                {
                    const unsigned int noise = x * 73U + y * 151U + frameIndex * 199U;
                    frame.setPixel(x, y, {
                        static_cast<uint8_t>((x + frameIndex * 3U) % 256U),
                        static_cast<uint8_t>((y + noise / 17U) % 256U),
                        static_cast<uint8_t>(noise % 256U)
                    });
                }
            }

            REQUIRE(writer.WriteFrame(frame));
        }

        REQUIRE(writer.Close());
    };

    encode(lowQualityPath, 1U);
    encode(highQualityPath, 100U);

    const uintmax_t lowQualitySize = std::filesystem::file_size(lowQualityPath);
    const uintmax_t highQualitySize = std::filesystem::file_size(highQualityPath);
    CHECK(highQualitySize > lowQualitySize * 2U);

    std::filesystem::remove(lowQualityPath, fileError);
    std::filesystem::remove(highQualityPath, fileError);
}

TEST_CASE("Windows native video writer preserves top-down frame orientation")
{
    constexpr unsigned int width = 320U;
    constexpr unsigned int height = 240U;
    const std::filesystem::path outputPath =
        std::filesystem::temp_directory_path() / "wxchaos-orientation-test.mp4";
    std::error_code fileError;
    std::filesystem::remove(outputPath, fileError);

    sf::Image frame;
    frame.create(width, height);
    for (unsigned int y = 0; y < height; ++y)
    {
        const sf::Color color = y < height / 2U ? sf::Color::White : sf::Color::Black;
        for (unsigned int x = 0; x < width; ++x)
            frame.setPixel(x, y, color);
    }

    NativeVideoWriter writer;
    REQUIRE(writer.Open(outputPath.string(), width, height, 30U, {100U, NativeVideoEncodingSpeed::Slow}));
    for (unsigned int frameIndex = 0; frameIndex < 3U; ++frameIndex)
        REQUIRE(writer.WriteFrame(frame));
    REQUIRE(writer.Close());

    const HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool comReady = SUCCEEDED(comResult) || comResult == RPC_E_CHANGED_MODE;
    REQUIRE(comReady);
    const bool comInitialized = SUCCEEDED(comResult);
    REQUIRE(SUCCEEDED(MFStartup(MF_VERSION)));

    Microsoft::WRL::ComPtr<IMFAttributes> readerAttributes;
    REQUIRE(SUCCEEDED(MFCreateAttributes(&readerAttributes, 1)));
    REQUIRE(SUCCEEDED(readerAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE)));

    Microsoft::WRL::ComPtr<IMFSourceReader> reader;
    REQUIRE(SUCCEEDED(MFCreateSourceReaderFromURL(outputPath.wstring().c_str(), readerAttributes.Get(), &reader)));

    Microsoft::WRL::ComPtr<IMFMediaType> decodedType;
    REQUIRE(SUCCEEDED(MFCreateMediaType(&decodedType)));
    REQUIRE(SUCCEEDED(decodedType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video)));
    REQUIRE(SUCCEEDED(decodedType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12)));
    REQUIRE(SUCCEEDED(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, decodedType.Get())));

    Microsoft::WRL::ComPtr<IMFSample> sample;
    DWORD streamFlags = 0;
    REQUIRE(SUCCEEDED(reader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, &streamFlags, nullptr, &sample)));
    REQUIRE((streamFlags & MF_SOURCE_READERF_ERROR) == 0);
    REQUIRE(sample != nullptr);

    Microsoft::WRL::ComPtr<IMFMediaBuffer> decodedBuffer;
    REQUIRE(SUCCEEDED(sample->ConvertToContiguousBuffer(&decodedBuffer)));
    BYTE* decodedPixels = nullptr;
    DWORD decodedLength = 0;
    REQUIRE(SUCCEEDED(decodedBuffer->Lock(&decodedPixels, nullptr, &decodedLength)));
    REQUIRE(decodedLength >= width * height);

    uint64_t topLuma = 0;
    uint64_t bottomLuma = 0;
    for (unsigned int y = 0; y < height / 4U; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            topLuma += decodedPixels[y * width + x];
            bottomLuma += decodedPixels[(height - 1U - y) * width + x];
        }
    }
    decodedBuffer->Unlock();

    CHECK(topLuma > bottomLuma * 2U);

    reader.Reset();
    MFShutdown();
    if (comInitialized)
        CoUninitialize();
    std::filesystem::remove(outputPath, fileError);
}
#endif
