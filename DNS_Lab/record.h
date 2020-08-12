/**
 * @brief     ��¼��ʽ
 * @author    ������
 * @version   0.0.1a
 * @date      2020.08.10-2020.08.10
 * @warning   ���ַ�δ������
 */

#ifndef _RECORD_H
#define _RECORD_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "dnsdef.h"

    /// ��¼����������
    union RecordData
    {
        ipv4_t IPv4;
    };

    /// ������¼
    struct Record
    {
        uchar Type;
        union RecordData Data;
        char* Domain;
    };

    /**
     * @brief �����ü�¼�ȽϺ���
     * @details ����������������ȫ���ϵ
     * @param LPtr ��ϵ����¼ָ��
     * @param RPtr ��ϵ�Ҳ��¼ָ��
     * @return ������ȷ���0�������ǰ������ֵ������Ϊ��ֵ
     */
    int RecordSortComp(void* LPtr, void* RPtr);

    /**
     * @brief ��ѯ�ü�¼�ȽϺ���
     * @details �������Ƿ����Ƚ�ȡ����Ŀ���¼����
     * @param KeyPtr Ŀ���¼ָ��
     * @param ParPtr �ο���¼ָ��
     * @return ƥ�䷵��0��Ӧ����ѯ������ֵ������Ϊ��ֵ
     */
    int RecordFindComp(void* KeyPtr, void* ParPtr);

    /**
     * @brief ��ѯ�ü�¼���ȽϺ���
     * @details �����򲻲���Ƚ�
     * @param KeyPtr Ŀ���¼ָ��
     * @param ParPtr �ο���¼ָ��
     * @return ƥ�䷵��0��Ӧ����ѯ������ֵ������Ϊ��ֵ
     */
    int RecordFindCompU(void* KeyPtr, void* ParPtr);

    /**
     * @brief ��¼��ʼ��
     * @param Object ��¼ָ��
     * @param Type   ��¼����
     * @param Data   ��������
     * @param Domain ����
     * @return �ɹ���ʼ������0����֧�ּ�¼���ͷ�����ֵ������ԭ�򷵻ظ�ֵ
     */
    int RecordNodeInit(struct Record* const Object, const uchar Type, const union RecordData Data, const char* const Domain);

    /**
     * @brief ����һ����¼
     * @param Type   ��¼����
     * @param Data   ��������
     * @param Domain ����
     * @return �ɹ�����ָ���¼��ָ�룻���򷵻�NULL
     */
    struct Record* RecordNode(const uchar Type, const union RecordData Data, const char* const Domain);

    /**
     * @brief �ͷ�һ����¼
     * @param Object Ŀ���¼ָ��
     */
    void RecordNodeFree(struct Record* const Object);

#ifdef __cplusplus
}
#endif

#endif // _RECORD_H