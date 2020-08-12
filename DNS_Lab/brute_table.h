/**
 * @brief     ������¼��
 * @details   �����¼�����ֲ��ң�֧��ͬһ�ؼ��ֶ����ֵ
 * @author    ������
 * @version   0.0.2
 * @date      2020.08.09-2020.08.12
 * @warning   ��֧������$8*10^6$����¼
 * @warning   ���ַ�δ������
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
     * @brief ������¼��
     * @warning ����ֱ���޸Ĵ˽ṹ�������ݣ���������ȫ�˽�������Ϊ��Ӱ��
     */
    struct BruteTable
    {
        /// ��ѯ������ʶ
        bool Ready;

        /// ָ��������¼ָ���ָ��
        struct Record** Data;

        /// ָ��ĩβ��¼ָ����ָ��
        struct Record** End;

        /// ��ǰ���ڼ�¼������
        size_t Size;
    };

    /**
     * @brief ��Ӽ�¼
     * @param Object Ŀ�������¼��ָ��
     * @param Item   Ŀ���¼
     * @return �ɹ����뷵��0����¼������������ֵ������ԭ�򷵻ظ�ֵ     
     */
    int BruteTableAppend(struct BruteTable* const Object, struct Record* const Item);

    /**
     * @brief �����¼��
     * @param Object Ŀ�������¼��ָ��
     * @warning �������¼���������¼����
     * @see BruteTableFree
     */
    void BruteTableClear(struct BruteTable* const Object);

    /**
     * @brief ��ѯ��¼
     * @details ��ѯ����������ܲ�ͬ�ڲο���¼
     * @param Object Ŀ�������¼��ָ��
     * @param Key    �ο���¼
     * @return �����ڣ�����Key�����򷵻�NULL
     * @warning ��ѯʧ�ܺ�ο���¼�����ݲ�����Ч
     */
    struct Record* const BruteTableCheck(struct BruteTable* const Object, struct Record* const Key);

    /**
     * @brief �ͷż�¼��
     * @param Object Ŀ�������¼��ָ��
     * @warning �����¼�����ڼ�¼����
     * @see BruteTableClear
     */
    void BruteTableFree(struct BruteTable* const Object);

#ifdef _DEBUG

    /**
     * @brief ��¼����ӻ������stdout
     * @param Object Ŀ�������¼��ָ��
     */
    void BruteTablePrint(struct BruteTable* const Object);

#endif

#ifdef __cplusplus
}
#endif

#endif // _URL_FILTER_H
