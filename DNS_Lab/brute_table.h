/**
 * @file      brute_table.h
 * @brief     基本记录表
 * @details   升序记录，二分查找，支持同一关键字多个键值
 * @author    王海昱
 * @version   0.0.2
 * @date      2020.08.09-2020.08.12
 * @warning   仅支持至多$8*10^6$条记录
 * @warning   宽字符未经测试
 */

#ifndef _BRUTE_TABLE_H
#define _BRUTE_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdafx.h"
#include "record.h"

    /**
     * @brief 基本记录表
     * @warning 请勿直接修改此结构体内数据，除非您完全了解以上行为的影响
     */
    struct BruteTable
    {
        /// 查询就绪标识
        bool Ready;

        /// 指向首条记录指针的指针
        struct Record** Data;

        /// 指向末尾记录指针后的指针
        struct Record** End;

        /// 当前表内记录数上限
        size_t Size;
    };

    /**
     * @brief 添加记录
     * @param Object 目标基本记录表指针
     * @param Item   目标记录
     * @return 成功插入返回0；记录表已满返回正值；其他原因返回负值     
     */
    int BruteTableAppend(struct BruteTable* const Object, struct Record* const Item);

    /**
     * @brief 清除记录表
     * @param Object 目标基本记录表指针
     * @warning 仅清除记录表，不清除记录内容
     * @see BruteTableFree
     */
    void BruteTableClear(struct BruteTable* const Object);

    /**
     * @brief 查询记录
     * @details 查询结果将尽可能不同于参考记录
     * @param Object 目标基本记录表指针
     * @param Key    参考记录
     * @return 若存在，返回Key；否则返回NULL
     * @warning 查询失败后参考记录中数据不再有效
     */
    struct Record* const BruteTableCheck(struct BruteTable* const Object, struct Record* const Key);

    /**
     * @brief 释放记录表
     * @param Object 目标基本记录表指针
     * @warning 清除记录表及表内记录内容
     * @see BruteTableClear
     */
    void BruteTableFree(struct BruteTable* const Object);

#ifdef _DEBUG

    /**
     * @brief 记录表可视化输出至stdout
     * @param Object 目标基本记录表指针
     */
    void BruteTablePrint(struct BruteTable* const Object);

#endif

#ifdef __cplusplus
}
#endif

#endif // _URL_FILTER_H
