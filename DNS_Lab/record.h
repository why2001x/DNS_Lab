/**
 * @brief     记录格式
 * @author    王海昱
 * @version   0.0.1a
 * @date      2020.08.10-2020.08.10
 * @warning   宽字符未经测试
 */

#ifndef _RECORD_H
#define _RECORD_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "dnsdef.h"
    union RecordData
    {
        ipv4_t IPv4;
    };
    struct Record
    {
        uchar Type;
        union RecordData Data;
        char* Domain;
    };
    int RecordSortComp(void* LPtr, void* RPtr);
    int RecordFindComp(void* KeyPtr, void* ParPtr);
    int RecordFindCompU(void* KeyPtr, void* ParPtr);
    int RecordNodeInit(struct Record* const Object, const uchar Type, const union RecordData Data, const char* const Domain);
    struct Record* RecordNode(const uchar Type, const union RecordData Data, const char* const Domain);
    void RecordNodeFree(struct Record* const Object);

#ifdef __cplusplus
}
#endif

#endif // _RECORD_H