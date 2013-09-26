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
typedef short unsigned int uint16;
typedef unsigned int uint32;
typedef int int32;

/**
* @struct BMPHeader
* @brief Header of the BMP file.
*/
struct BMPHeader
{
	uint16 identifier;
	uint32 size;
	uint16 appSpecific1;
	uint16 appSpecific2;
	uint32 bitmapData;
};

/**
* @struct DIBHeader
* @brief Another header used by the BMP file that contains parameters.
*/
struct DIBHeader
{
	uint32 headerSize;
	int32 width;
	int32 height;
	uint16 nPlanes;
	uint16 colorDepth;
	uint32 compression;
	uint32 bmpBytes;
	int32 hRes;
	int32 vRes;
	uint32 nColors;
	uint32 nImpColors;
};

/**
* @struct BMPPixel
* @brief A simple rgb pixel.
*/
struct BMPPixel
{
	char r, g, b;	
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