#include "BinaryOps.h"

char* ToByte(void* ptr)
{
    return static_cast<char*>(ptr);
}

uint16_t SwitchEndianess16(uint16_t data)
{
    uint16_t temp;
    uint16_t out = 0x0000;
    uint16_t pos = 0x00FF;

    temp = data & pos;
    temp <<= 8;
    out |= temp;
    pos <<= 8;

    temp = data & pos;
    temp >>= 8;
    out |= temp;

    return out;
}

uint32_t SwitchEndianess32(uint32_t data)
{
    uint32_t temp;
    uint32_t out = 0x00000000;
    uint32_t pos = 0x000000FF;

    temp = data & pos;
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
void BoolToChar(char& target, bool in)
{
    if(in)
        target = 0x01;
    else
        target = 0x00;
}

void CharToBool(bool& target, char in)
{
    if(in == 0x00)
        target = false;
    else
        target = true;
}

void UInt32ToChar(char* target, uint32_t in)
{
    in = SwitchEndianess32(in);
    uint32_t temp = 0x000000FF;
    for(unsigned int i=0; i<sizeof(uint32_t); i++)
    {
        target[i] = in & temp;
        in >>= 8;
    }
}

void Int32ToChar(char* target, int32_t in)
{
    in = SwitchEndianess32(in);
    int32_t temp = 0x000000FF;
    for(unsigned int i=0; i<sizeof(int32_t); i++)
    {
        target[i] = in & temp;
        in >>= 8;
    }
}

void CharToUInt32(uint32_t& target, char* in)
{
    target = 0x00000000;
    uint32_t temp;
    for(unsigned int i=0; i<sizeof(uint32_t); i++)
    {
        temp = in[i] & 0x000000FF;
        target |= temp;
        if(i != 3)
            target <<= 8;
    }
}

void CharToInt32(int32_t& target, char* in)
{
    target = 0x00000000;
    int32_t temp;
    for(unsigned int i=0; i<sizeof(int32_t); i++)
    {
        temp = in[i] & 0x000000FF;
        target |= temp;
        if(i != 3) target <<= 8;
    }
}