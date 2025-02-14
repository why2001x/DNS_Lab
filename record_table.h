﻿/**
 * @brief     记录表
 * @details   支持增加/查询/清空记录、同一关键字多个键值
 * @author    王海昱
 * @version   0.0.2
 * @date      2020.08.10-2020.08.10
 * @warning   宽字符未经测试
 */

#ifndef _RECORD_TABLE_H
#define _RECORD_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @defgroup RecordTable
     * @{
     */

#ifdef USE_HASH
#include "hash_table.h"
    //struct HashTable
    //{
    //    char** String;
    //    struct TrieNode** Data;
    //    size_t Size;
    //};

    /**
     * @brief   记录表
     * @warning 请使用RecordTable*函数进行访问
     */
    typedef struct HashTable RecordTable;

    /**
     * @brief 向记录表插入一条记录
     * @param Object 目标记录表指针
     * @param Item   待插入记录指针
     * @return 成功插入返回0；记录表已满返回正值；其他原因返回负值
     */
    const int (*RecordTableAppend)(RecordTable* const Object, struct Record* const Item) = HashTableAppend;

    /**
     * @brief 查询记录
     * @details 查询结果将尽可能不同于参考记录
     * @param Object 目标基本记录表指针
     * @param Key    参考记录
     * @return 若存在，返回Key；否则返回NULL
     * @warning 查询失败后参考记录中数据不再有效
     */
    const struct Record* const(*RecordCheck)(RecordTable* const Object, struct Record* const) = HashTableCheck;

    /**
     * @brief 清除记录
     * @param Object 目标记录表指针
     * @warning 清除记录表内记录内容
     */
    const void (*RecordTableClear)(RecordTable* const Object) = HashTableClear;

#ifdef _DEBUG

    /**
     * @brief 记录表可视化输出至stdout
     * @param Object 目标记录表指针
     */
    const void (*RecordTablePrint)(RecordTable* const Object) = HashTablePrint;

#endif // _DEBUG

#else // USE_HASH_TRIE

#define USE_BRUTE

#include "brute_table.h"

    /**
     * @brief   记录表
     * @warning 请使用RecordTable*函数进行访问
     */
    typedef struct BruteTable RecordTable;

    /**
     * @brief 向记录表插入一条记录
     * @param Object 目标记录表指针
     * @param Item   待插入记录指针
     * @return 成功插入返回0；记录表已满返回正值；其他原因返回负值
     */
    const int (*RecordTableAppend)(RecordTable* const Object, struct Record* const Item) = BruteTableAppend;

    /**
     * @brief 查询记录
     * @details 查询结果将尽可能不同于参考记录
     * @param Object 目标基本记录表指针
     * @param Key    参考记录
     * @return 若存在，返回Key；否则返回NULL
     * @warning 查询失败后参考记录中数据不再有效
     */
    const struct Record* const(*RecordCheck)(RecordTable* const Object, struct Record* const Key) = BruteTableCheck;

    /**
     * @brief 清除记录
     * @param Object 目标记录表指针
     * @warning 清除记录表内记录内容
     */
    const void (*RecordTableClear)(RecordTable* const Object) = BruteTableClear;

#ifdef _DEBUG

    /**
     * @brief 记录表可视化输出至stdout
     * @param Object 目标记录表指针
     */
    const void (*RecordTablePrint)(RecordTable* const Object) = BruteTablePrint;

#endif // _DEBUG

#endif // USE_HASH_TRIE

    /**@}*/

#ifndef _DEBUG
#define RecordTablePrint(Object)
#endif // !_DEBUG

#ifdef __cplusplus
}
#endif

#endif // _RECORD_TABLE_H
