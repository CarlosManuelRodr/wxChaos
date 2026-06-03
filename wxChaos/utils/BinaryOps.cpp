#include "BinaryOps.h"

char* ToByte(void* ptr)
{
    return static_cast<char*>(ptr);
}

uint16_t SwitchEndianness16(uint16_t data)
{
    uint16_t out = 0x0000;
    uint16_t pos = 0x00FF;

    uint16_t temp = data & pos;
    temp <<= 8;
    out |= temp;
    pos <<= 8;

    temp = data & pos;
    temp >>= 8;
    out |= temp;

    return out;
}

uint32_t SwitchEndianness32(uint32_t data)
{
    uint32_t out = 0x00000000;
    uint32_t pos = 0x000000FF;

    uint32_t temp = data & pos;
    temp <<= 24;
    out |= temp;
    pos <<= 8;
    temp = data & pos;
    temp <<= 8;
    out |= temp;
    pos <<= 8;
    temp = data & pos;
    temp >>= 8;
    out |= temp;
    pos <<= 8;
    temp = data & pos;
    temp >>= 24;
    out |= temp;

    return out;
}

// Conversion operations.
void BoolToChar(char& target, const bool in)
{
    target = in ? 0x01 : 0x00;
}

void CharToBool(bool& target, const char in)
{
    target = in != 0x00;
}

void UInt32ToChar(char* target, uint32_t in)
{
    in = SwitchEndianness32(in);
    for (unsigned int i=0; i<sizeof(uint32_t); i++)
    {
        constexpr uint32_t temp = 0x000000FF;
        target[i] = in & temp;
        in >>= 8;
    }
}

void Int32ToChar(char* target, int32_t in)
{
    in = SwitchEndianness32(in);
    for (unsigned int i=0; i<sizeof(int32_t); i++)
    {
        constexpr int32_t temp = 0x000000FF;
        target[i] = in & temp;
        in >>= 8;
    }
}

void CharToUInt32(uint32_t& target, const char* in)
{
    target = 0x00000000;
    for (unsigned int i=0; i<sizeof(uint32_t); i++)
    {
        const uint32_t temp = in[i] & 0x000000FF;
        target |= temp;
        if (i != 3)
            target <<= 8;
    }
}

void CharToInt32(int32_t& target, const char* in)
{
    target = 0x00000000;
    for (unsigned int i=0; i<sizeof(int32_t); i++)
    {
        const int32_t temp = in[i] & 0x000000FF;
        target |= temp;
        if (i != 3) target <<= 8;
    }
}