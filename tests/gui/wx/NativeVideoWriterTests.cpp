#include <doctest/doctest.h>
#include "export/NativeVideoWriter.h"

#ifdef _WIN32
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
TEST_CASE("Windows native video writer accepts quality-based VBR settings")
{
    const std::filesystem::path outputPath =
        std::filesystem::temp_directory_path() / "wxchaos-quality-vbr-test.mp4";
    std::error_code fileError;
    std::filesystem::remove(outputPath, fileError);

    NativeVideoWriter writer;
    const bool opened = writer.Open(outputPath.string(), 320U, 240U, 30U, {});
    INFO(writer.GetError());
    REQUIRE(opened);

    sf::Image frame;
    frame.create(320U, 240U, sf::Color::Black);
    for (unsigned int y = 0; y < 240U; ++y)
    {
        for (unsigned int x = 0; x < 320U; ++x)
            frame.setPixel(x, y, y < 120U ? sf::Color::Red : sf::Color::Blue);
    }

    REQUIRE(writer.WriteFrame(frame));
    REQUIRE(writer.WriteFrame(frame));
    REQUIRE(writer.Close());
    CHECK(std::filesystem::file_size(outputPath) > 0U);
    std::filesystem::remove(outputPath, fileError);
}
#endif
