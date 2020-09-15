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

    /// @brief ��ϣ������
    struct hlist_node
    {
        struct hlist_node* next; /// ָ����һ������ָ��
        struct hlist_node** pprev; /// ָ����һ������nextָ��ĵ�ַ
        struct Record* Data; /// ��¼����
        time_t TimeToThrow; /// Ӧ����ʱ��
    };

    /**
     * @brief ��ϣ��¼��
     * @warning ����ֱ���޸Ĵ˽ṹ�������ݣ���������ȫ�˽�������Ϊ��Ӱ��
     */
    struct HashTable
    {
        struct hlist_head
        {
            struct hlist_node* next;
        }List[HASH_SIZE];
    };

    /**
     * @brief ��Ӽ�¼
     * @param Object Ŀ�������¼��ָ��
     * @param Item   Ŀ���¼
     * @return �ɹ����뷵��0����¼������������ֵ������ԭ�򷵻ظ�ֵ
     */
    int HashTableAppend(struct HashTable* const Object, struct Record* const Item);

    /**
     * @brief ��ѯ��¼
     * @details ��ѯ����������ܲ�ͬ�ڲο���¼
     * @param Object Ŀ�������¼��ָ��
     * @param Key    �ο���¼
     * @return �����ڣ�����Key�����򷵻�NULL
     * @warning ��ѯʧ�ܺ�ο���¼�����ݲ�����Ч
     */
    struct Record* const HashTableCheck(struct HashTable* const Object, struct Record* const Key);

    /**
     * @brief �����¼
     * @param Object Ŀ�������¼��ָ��
     * @warning �����¼���ڼ�¼����
     */
    void HashTableClear(struct HashTable* const Object);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _HASH_TABLE_H