#include <doctest/doctest.h>
#include "export/NativeVideoWriter.h"

#ifdef _WIN32
#include <cstdint>
#include <filesystem>
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
#endif
