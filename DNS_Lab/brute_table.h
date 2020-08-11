/**
 * @brief     基本记录表
 * @details   升序记录，二分查找，支持同一关键字多个键值
 * @author    王海昱
 * @version   0.0.1a
 * @date      2020.08.09-2020.08.09
 * @warning   仅支持至多$8*10^6$条记录
 * @warning   宽字符未经测试
 */

#ifndef _BRUTE_TABLE_H
#define _BRUTE_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "record.h"

    struct BruteTable
    {
        bool Ready;
        struct Record** Data, ** End;
        size_t Size;
    };

    int BruteTableAppend(struct BruteTable* const, struct Record* const);
    void BruteTableClear(struct BruteTable* const);
    struct Record* const BruteTableCheck(struct BruteTable* const, struct Record* const);
#ifdef _DEBUG
    void BruteTablePrint(struct BruteTable* const Object);
#endif

#ifdef __cplusplus
}
#endif

#endif // _URL_FILTER_H
