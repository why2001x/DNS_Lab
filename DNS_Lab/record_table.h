#ifndef _RECORD_TABLE_H
#define _RECORD_TABLE_H
#ifdef __cplusplus
extern "C" {
#endif
    /**
     * @defgroup RecordTable
     * @{
     */
#define USE_BRUTE
#include "brute_table.h"
    typedef struct BruteTable RecordTable; /// 记录表
    const int (*RecordTableAppend)(RecordTable* const Object, struct Record* const Item) = BruteTableAppend; /// 向记录表插入一条记录
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

    /**@}*/

#ifndef _DEBUG
#define RecordTablePrint(Object)
#endif // !_DEBUG

#ifdef __cplusplus
}
#endif

#endif // _URL_FILTER_H
