#include "BinaryOps.h"

char* ToByte(void* ptr)
{
	return static_cast<char*>(ptr);
}

uint16 SwitchEndianess16(uint16 data)
{
	uint16 temp;
	uint16 out = 0x0000;
	uint16 pos = 0x00FF;

	temp = data & pos;
	temp <<= 8;
	out |= temp;
	pos <<= 8;

	temp = data & pos;
	temp >>= 8;
	out |= temp;

	return out;
}

uint32 SwitchEndianess32(uint32 data)
{
	uint32 temp;
	uint32 out = 0x00000000;
	uint32 pos = 0x000000FF;

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
	if(in) target = 0x01;
	else target = 0x00;
}

void CharToBool(bool& target, char in)
{
	if(in == 0x00) target = false;
	else target = true;
}

void UInt32ToChar(char* target, uint32 in)
{
	in = SwitchEndianess32(in);
	uint32 temp = 0x000000FF;
	for(unsigned int i=0; i<sizeof(uint32); i++)
	{
		target[i] = in & temp;
		in >>= 8;
	}
}

void Int32ToChar(char* target, int32 in)
{
	in = SwitchEndianess32(in);
	uint32 temp = 0x000000FF;
	for(unsigned int i=0; i<sizeof(int32); i++)
	{
		target[i] = in & temp;
		in >>= 8;
	}
}

void CharToUInt32(uint32& target, char* in)
{
	target = 0x00000000;
	uint32 temp;
	for(unsigned int i=0; i<sizeof(uint32); i++)
	{
		temp = in[i] & 0x000000FF;
		target |= temp;
		if(i != 3) target <<= 8;
	}
}

void CharToInt32(int32& target, char* in)
{
	target = 0x00000000;
	uint32 temp;
	for(unsigned int i=0; i<sizeof(int32); i++)
	{
		temp = in[i] & 0x000000FF;
		target |= temp;
		if(i != 3) target <<= 8;
	}
}
