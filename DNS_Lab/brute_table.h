/**
 * @brief     ������¼��
 * @details   �����¼�����ֲ��ң�֧��ͬһ�ؼ��ֶ����ֵ
 * @author    ������
 * @version   0.0.1a
 * @date      2020.08.09-2020.08.09
 * @warning   ��֧������$8*10^6$����¼
 * @warning   ���ַ�δ������
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
