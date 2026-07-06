#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics/Image.hpp>

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

    bool Open(const std::string& outputPath, unsigned int width, unsigned int height, unsigned int fps) const;
    bool WriteFrame(const sf::Image& frame) const;
    bool Close() const;

    [[nodiscard]] std::string GetError() const;
};
