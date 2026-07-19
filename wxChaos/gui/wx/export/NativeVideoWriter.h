#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics/Image.hpp>

enum class NativeVideoEncodingSpeed
{
    Fast,
    Balanced,
    Slow
};

struct NativeVideoEncodingOptions
{
    unsigned int quality{80};
    NativeVideoEncodingSpeed speed{NativeVideoEncodingSpeed::Balanced};
};

/**
* @class NativeVideoWriter
* @brief Platform-neutral wrapper around the operating system's video encoder.
*/
class NativeVideoWriter
{
    class Impl;
    std::unique_ptr<Impl> _impl;

public:
    NativeVideoWriter();
    ~NativeVideoWriter();

    [[nodiscard]] bool Open(const std::string& outputPath, unsigned int width, unsigned int height, unsigned int fps,
                            const NativeVideoEncodingOptions& options) const;
    [[nodiscard]] bool WriteFrame(const sf::Image& frame) const;
    [[nodiscard]] bool Close() const;
    [[nodiscard]] std::string GetError() const;
    [[nodiscard]] static unsigned int GetH264Quantizer(unsigned int quality);
};
