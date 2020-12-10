/** 
* @file BmpWriter.h 
* @brief A simple BMP writer designed to avoid size limitations.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/18/2012
*/

#pragma once
#ifndef __bmpWriter
#define __bmpWriter

#include <fstream>
#include <cstdint>

/**
* @struct BMPHeader
* @brief Header of the BMP file.
*/
struct BMPHeader
{
    uint16_t identifier;
    uint32_t size;
    uint16_t appSpecific1;
    uint16_t appSpecific2;
    uint32_t bitmapData;
};

/**
* @struct DIBHeader
* @brief Another header used by the BMP file that contains parameters.
*/
struct DIBHeader
{
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t nPlanes;
    uint16_t colorDepth;
    uint32_t compression;
    uint32_t bmpBytes;
    int32_t hRes;
    int32_t vRes;
    uint32_t nColors;
    uint32_t nImpColors;
};

/**
* @struct BMPPixel
* @brief A simple rgb pixel.
*/
struct BMPPixel
{
    uint8_t r, g, b;
};

/**
* @class BMPWriter
* @brief Class to write BMP files.
*
* Although SFML has it's own methods to save image files these have memory allocation problems if the
* user tries to save a oversized image (10000x10000 pixels for example). This class it's designed to avoid
* these problems by writing the BMP line by line.
*/
class BMPWriter
{
    BMPHeader* myBmpHdr;
    DIBHeader* myDibHdr;
    std::ofstream file;
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int paddingBytes;
    int dataSize;
    unsigned int indexHeight;

public:
    ///@brief Constructor.
    ///@param filepath Path of file to save.
    ///@param width Width of the image.
    ///@param height Height of the image.
    BMPWriter(const char* filepath, unsigned int width, unsigned int height);
    ~BMPWriter();

    ///@brief Writes BMP line.
    ///@param data Array of pixels to write.
    void WriteLine(BMPPixel* data);
    void CloseBMP();
};

#endif  // __bmpWriter