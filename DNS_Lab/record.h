/**
 * @brief     记录格式
 * @author    王海昱
 * @version   0.0.1a
 * @date      2020.08.10-2020.08.10
 * @warning   宽字符未经测试
 */

#ifndef _RECORD_H
#define _RECORD_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "dnsdef.h"

    /**
     * @ingroup RecordTable
     * @defgroup Record
     * @{
     */

    /// 记录内数据类型
    union RecordData
    {
        ipv4_t IPv4;
        ipv6_t IPv6;
    };
    extern const union RecordData RECORD_DATA_MAX;

    /// 单条记录
    struct Record
    {
        uchar Type;
        union RecordData Data;
        char* Domain;
    };

    /**
     * @brief 排序用记录比较函数
     * @details 左右两侧内容满足全序关系
     * @param LPtr 关系左侧记录指针
     * @param RPtr 关系右侧记录指针
     * @return 左右相等返回0，左侧在前返回正值，否则为负值
     */
    int RecordSortComp(void* LPtr, void* RPtr);

    /**
     * @brief 查询用记录比较函数
     * @details 数据域是否参与比较取决于目标记录内容
     * @param KeyPtr 目标记录指针
     * @param ParPtr 参考记录指针
     * @return 匹配返回0，应向后查询返回正值，否则为负值
     */
    int RecordFindComp(void* KeyPtr, void* ParPtr);

    /**
     * @brief 查询用记录弱比较函数
     * @details 数据域不参与比较
     * @param KeyPtr 目标记录指针
     * @param ParPtr 参考记录指针
     * @return 匹配返回0，应向后查询返回正值，否则为负值
     */
    int RecordFindCompU(void* KeyPtr, void* ParPtr);

    /**
     * @brief 记录初始化
     * @param Object 记录指针
     * @param Type   记录类型
     * @param Data   数据内容
     * @param Domain 域名
     * @return 成功初始化返回0；不支持记录类型返回正值；其他原因返回负值
     */
    int RecordNodeInit(struct Record* const Object, const uchar Type, const union RecordData Data, const char* const Domain);

    /**
     * @brief 构造一条记录
     * @param Type   记录类型
     * @param Data   数据内容
     * @param Domain 域名
     * @return 成功返回指向记录的指针；否则返回NULL
     */
    struct Record* RecordNode(const uchar Type, const union RecordData Data, const char* const Domain);

    /**
     * @brief 释放一条记录
     * @param Object 目标记录指针
     */
    void RecordNodeFree(struct Record* const Object);

    /**
     * @brief 输出记录到字符串
     * @param Object 输出记录指针
     * @param Dst 目标字符串
     */
    void sprintRecord(const struct Record* const Object, char* const Dst);

    /**@}*/

#ifdef __cplusplus
}
#endif

#endif // _RECORD_H