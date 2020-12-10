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

#include <cstdint>

uint16_t SwitchEndianess16(uint16_t data);
uint32_t SwitchEndianess32(uint32_t data);
char* ToByte(void* ptr);
void BoolToChar(char& target, bool in);
void CharToBool(bool& target, char in);
void UInt32ToChar(char* target, uint32_t in);
void Int32ToChar(char* target, int32_t in);
void CharToUInt32(uint32_t& target, char* in);
void CharToInt32(int32_t& target, char* in);

#endif