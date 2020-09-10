#ifndef _RECORD_H
#define _RECORD_H
#ifdef __cplusplus
extern "C" {
#endif
#include "dnsdef.h"
    /**
     * @ingroup RecordTable
     * @defgroup Record
     * @{
     */
    union RecordData { ipv4_t IPv4; ipv6_t IPv6; }; /// 记录内数据类型
    extern const union RecordData RECORD_DATA_MAX; /// 记录数据最大值
    struct Record {  uchar Type; union RecordData Data; char* Domain; }; /// 单条记录
    int RecordSortComp(void* LPtr, void* RPtr); /// 全序记录比较函数
    int RecordFindComp(void* KeyPtr, void* ParPtr); /// 记录内容决定比较函数
    int RecordFindCompU(void* KeyPtr, void* ParPtr);/// 内容无关记录比较函数
    int RecordNodeInit(struct Record* const Object, const uchar Type, const union RecordData Data, const char* const Domain); /// 记录初始化 成功返回0；不支持记录类型返回正值；其他原因返回负值
    struct Record* RecordNode(const uchar Type, const union RecordData Data, const char* const Domain); /// 构造一条记录 成功返回指向记录的指针；否则返回NULL
    void RecordNodeFree(struct Record* const Object); ///  释放一条记录
    /**@}*/
#ifdef __cplusplus
}
#endif
#endif // _RECORD_H