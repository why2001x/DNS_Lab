#ifndef _CRC16_H
#define _CRC16_H
#include "stdafx.h"
#ifdef __cplusplus
extern "C"
{
#endif
    uint16_t CRC16(const char* Buffer, int Len);
    uint16_t CRC16Append(uint16_t* const pCRC, const char Src);
#ifdef __cplusplus
}
#endif
#endif // _CRC16_H