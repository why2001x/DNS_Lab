/**
 * @brief     记录表
 * @details   支持增加/查询/清空记录、同一关键字多个键值
 * @author    王海昱
 * @version   0.0.1a
 * @date      2020.08.10-2020.08.10
 * @warning   宽字符未经测试
 */

#ifndef _RECORD_TABLE_H
#define _RECORD_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef USE_HASH_TRIE
#include "hash_table.h"
    //struct TrieNode
    //{
    //    bool HasRec, HasLower;
    //    struct Record* Rec;
    //    struct TrieNode* Next[26];
    //};
    //struct HashTable
    //{
    //    char** String;
    //    struct TrieNode** Data;
    //    size_t Size;
    //};
    typedef struct HashTable RecordTable;
    const int (*RecordTableAppend)(RecordTable* const, struct Record* const) = HashTableAppend;
    const void (*RecordTableClear)(RecordTable* const) = HashTableClear;
    const struct Record* const(*RecordCheck)(RecordTable* const, struct Record* const) = HashTableCheck;
#ifdef _DEBUG
    const void (*RecordTablePrint)(struct BruteTable* const Object) = BruteTablePrint;
#endif

#else

#define USE_BRUTE

#include "brute_table.h"
    typedef struct BruteTable RecordTable;
    const int (*RecordTableAppend)(RecordTable* const, struct Record* const) = BruteTableAppend;
    const void (*RecordTableClear)(RecordTable* const) = BruteTableClear;
    const struct Record* const(*RecordCheck)(RecordTable* const, struct Record* const) = BruteTableCheck;
#ifdef _DEBUG
    const void (*RecordTablePrint)(struct BruteTable* const Object)= BruteTablePrint;
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif // _URL_FILTER_H
