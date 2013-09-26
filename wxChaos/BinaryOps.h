/** 
* @file BinaryOps.h 
* @brief Some bitwise operations later required to perform tasks.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/18/2012
*/

#pragma once
#ifndef _BinaryOps
#define _BinaryOps

typedef short unsigned int uint16;
typedef unsigned int uint32;
typedef int int32;

uint16 SwitchEndianess16(uint16 data);
uint32 SwitchEndianess32(uint32 data);
char* ToByte(void* ptr);
void BoolToChar(char& target, bool in);
void CharToBool(bool& target, char in);
void UInt32ToChar(char* target, uint32 in);
void Int32ToChar(char* target, int32 in);
void CharToUInt32(uint32& target, char* in);
void CharToInt32(int32& target, char* in);

#endif