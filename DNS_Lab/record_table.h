/**
 * @brief     ��¼��
 * @details   ֧������/��ѯ/��ռ�¼��ͬһ�ؼ��ֶ����ֵ
 * @author    ������
 * @version   0.0.2
 * @date      2020.08.10-2020.08.10
 * @warning   ���ַ�δ������
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
    const int (*RecordTableAppend)(RecordTable* const Object, struct Record* const Item) = HashTableAppend;
    const void (*RecordTableClear)(RecordTable* const Object) = HashTableClear;
    const struct Record* const(*RecordCheck)(RecordTable* const Object, struct Record* const) = HashTableCheck;
#ifdef _DEBUG
    const void (*RecordTablePrint)(struct RecordTable* const Object) = HashTablePrint;
#endif // _DEBUG

#else // USE_HASH_TRIE

#define USE_BRUTE

#include "brute_table.h"
    /**
     * @brief   ��¼��
     * @warning ��ʹ��RecordTable*�������з���
     */
    typedef struct BruteTable RecordTable;

    /**
     * @brief ���¼�����һ����¼
     * @param Object Ŀ���¼��ָ��
     * @param Item   �������¼ָ��
     * @return �ɹ����뷵��0����¼������������ֵ������ԭ�򷵻ظ�ֵ
     */
    const int (*RecordTableAppend)(RecordTable* const Object, struct Record* const Item) = BruteTableAppend;

    /**
     * @brief �����¼��
     * @param Object Ŀ���¼��ָ��
     * @warning �������¼���������¼����
     * @see RecordTableFree
     */
    const void (*RecordTableClear)(RecordTable* const Object) = BruteTableClear;

    /**
     * @brief ��ѯ��¼
     * @details ��ѯ����������ܲ�ͬ�ڲο���¼
     * @param Object Ŀ�������¼��ָ��
     * @param Key    �ο���¼
     * @return �����ڣ�����Key�����򷵻�NULL
     * @warning ��ѯʧ�ܺ�ο���¼�����ݲ�����Ч
     */
    const struct Record* const(*RecordCheck)(RecordTable* const Object, struct Record* const Key) = BruteTableCheck;

    /**
     * @brief �ͷż�¼��
     * @param Object Ŀ�������¼��ָ��
     * @warning �����¼�����ڼ�¼����
     * @see RecordTableClear
     */
    const void (*RecordTableFree)(RecordTable* const Object) = BruteTableFree;

#ifdef _DEBUG

    /**
     * @brief ��¼����ӻ������stdout
     * @param Object Ŀ���¼��ָ��
     */
    const void (*RecordTablePrint)(struct RecordTable* const Object) = BruteTablePrint;

#endif // _DEBUG

#endif // USE_HASH_TRIE

#ifndef _DEBUG
#define RecordTablePrint(Object)
#endif // !_DEBUG



#ifdef __cplusplus
}
#endif

#endif // _URL_FILTER_H
