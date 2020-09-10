#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "stdafx.h"
#include "crc16.h"
#include "record.h"

#define HASH_SIZE (1 << 16)

    /// @brief 哈希链表结点
    struct hlist_node
    {
        struct hlist_node* next; /// 指向下一个结点的指针
        struct hlist_node** pprev; /// 指向上一个结点的next指针的地址
        struct Record* Data; /// 记录内容
        time_t TimeToThrow; /// 应抛弃时间
    };

    /**
     * @brief 哈希记录表
     * @warning 请勿直接修改此结构体内数据，除非您完全了解以上行为的影响
     */
    struct HashTable
    {
        struct hlist_head
        {
            struct hlist_node* next;
        }List[HASH_SIZE];
    };

    /**
     * @brief 添加记录
     * @param Object 目标基本记录表指针
     * @param Item   目标记录
     * @return 成功插入返回0；记录表已满返回正值；其他原因返回负值
     */
    int HashTableAppend(struct HashTable* const Object, struct Record* const Item);

    /**
     * @brief 查询记录
     * @details 查询结果将尽可能不同于参考记录
     * @param Object 目标基本记录表指针
     * @param Key    参考记录
     * @return 若存在，返回Key；否则返回NULL
     * @warning 查询失败后参考记录中数据不再有效
     */
    struct Record* const HashTableCheck(struct HashTable* const Object, struct Record* const Key);

    /**
     * @brief 清除记录
     * @param Object 目标基本记录表指针
     * @warning 清除记录表内记录内容
     */
    void HashTableClear(struct HashTable* const Object);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _HASH_TABLE_H