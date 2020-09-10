#ifndef _BRUTE_TABLE_H
#define _BRUTE_TABLE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "stdafx.h"
#include "record.h"
    struct BruteTable { /// 基本记录表
        bool Ready; /// 查询就绪标识
        struct Record** Data; /// 指向首条记录指针的指针
        struct Record** End; /// 指向末尾记录指针后的指针
        size_t Size; /// 当前表内记录数上限
    };
    int BruteTableAppend(struct BruteTable* const Object, struct Record* const Item); /// 添加记录 成功返回0；记录表已满返回正值；其他原因返回负值
    struct Record* const BruteTableCheck(struct BruteTable* const Object, struct Record* const Key); /// 查询记录 若存在，返回Key；否则返回NULL
    void BruteTableClear(struct BruteTable* const Object); /// 清除记录
#ifdef _DEBUG
    void BruteTablePrint(struct BruteTable* const Object); /// 记录表输出至stdout
#endif
#ifdef __cplusplus
}
#endif
#endif // _URL_FILTER_H