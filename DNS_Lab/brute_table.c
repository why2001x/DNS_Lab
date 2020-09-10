#include "stdafx.h"

#include "log_output.h"
#include "brute_table.h"

static void BruteTableReset(struct BruteTable* const Object)
{
    if (Object == NULL)
    {
        return;
    }
    lputs(LOG_DBUG, "BruteTable: Resetting a BruteTable.");
    /// 释放记录指针数组内存空间
    free(Object->Data);
    /// 初始化基本记录表
    Object->Data = Object->End = NULL;
    Object->Size = 0;
    Object->Ready = false;
    return;
}

void BruteTableClear(struct BruteTable* const Object)
{
    if (Object == NULL)
    {
        return;
    }
    lputs(LOG_DBUG, "BruteTable: Cleaning a BruteTable.");
    for (int i = 0; i < Object->Size; i++)
    {
        RecordNodeFree(Object->Data[i]);
        Object->Data[i] = NULL;
    }
    BruteTableReset(Object);
    return;
}

#define MAX_SIZE (1 << 23)
#define STEP_BT_MIN 16
#define STEP_BT_MAX 2048
int BruteTableAppend(struct BruteTable* const Object, struct Record* const Item)
{
#ifdef _THR_XTHREADS_H

#else
#endif
    /// 判断表内空间是否已满
    if (Object->Data + Object->Size == Object->End)
    {
        lputs(LOG_DBUG, "BruteTable: Expanding the BruteTable.");
        /// 判断表记录数是否到达约定上限
        if (Object->Size >= MAX_SIZE)
        {
            lputs(LOG_WARN, "BruteTable: The number of rules more than the limit! Skipped.");
            return 1;
        }
        /// 初始化新表
        struct BruteTable Temp = { NULL };
        Temp.Size = Object->Size + min(STEP_BT_MAX, max(STEP_BT_MIN, Object->Size));
        Temp.Data = (struct Record**)malloc(Temp.Size * sizeof(struct Record*));
        if (Temp.Data == NULL)
        {
            lputs(LOG_ERRN, "BruteTable: Error while allocating memory!");
            return -1;
        }
        memset(Temp.Data, 0, Temp.Size * sizeof(struct Record*));
        Temp.End = Temp.Data + Object->Size;
        /// 拷贝旧表记录至新表
        if (Object->Data != NULL)
        {
            lputs(LOG_DBUG, "BruteTable: Copy the data on the older table.");
            memcpy(Temp.Data, Object->Data, Object->Size * sizeof(struct Record*));
            BruteTableReset(Object);
            lputs(LOG_DBUG, "BruteTable: The older table was released.");
        }
        lputs(LOG_DBUG, "BruteTable: Change to the newer one.");
        Object->Ready = false;
        memcpy(Object, &Temp, sizeof(struct BruteTable));
    }
    /// 添加待插入记录
    Object->Ready = false;
    *(Object->End) = Item;
    Object->End++;
    lputs(LOG_DBUG, "BruteTable: A Record add to the end of BruteTable.");
#ifdef _THR_XTHREADS_H
#else
#endif
    return 0;
}
#undef MAX_SIZE
#undef STEP_BT_MIN
#undef STEP_BT_MAX

static void BruteTableSort(struct BruteTable* const Object)
{
    lputs(LOG_DBUG, "BruteTable: Sorting the BruteTable.");
    qsort(Object->Data, Object->End - Object->Data, sizeof(struct Record*), RecordSortComp);
    Object->Ready = false;
    return;
}

static void BruteTableUnique(struct BruteTable* const Object)
{
    lputs(LOG_DBUG, "BruteTable: Making the BruteTable unique.");
    if (Object->Data == Object->End)
    {
        return;
    }
    /// 记录去重
    struct Record** Dst = Object->Data;
    for (struct Record** it = Object->Data + 1; it != Object->End; it++)
    {
        if (RecordSortComp(it, Dst) == 0)
        {
            /// 释放无用记录，避免内存泄露
            RecordNodeFree(*it);
            continue;
        }
        Dst++;
        *Dst = *it;
    }
    Dst++;
    /// 维护表内信息
    for (struct Record** it = Dst; it != Object->End; it++)
    {
        *it = NULL;
    }
    Object->End = Dst;
    Object->Ready = false;
    return;
}

static struct Record** BruteTableBSearch(struct Record* const Key, const struct Record** const Begin, const struct Record** const End, const int (*Compare)(const void*, const void*))
{
    struct Record** Low = Begin, ** High = End;
    if (Low == High)
    {
        return NULL;
    }
    for (; High - Low > 1;)
    {
        struct Record** Half = (High - Low) / 2 + Low;
        if (Compare(&Key, Half) >= 0)
        {
            Low = Half;
        }
        else
        {
            High = Half;
        }
    }
    return Low;
}

struct Record* const BruteTableCheck(struct BruteTable* const Object, struct Record* const Key)
{
    /// 空表
    if (Object->Data == Object->End)
    {
        return NULL;
    }
    /// 若记录表未就绪，排序并去重
    if (!Object->Ready)
    {
        BruteTableSort(Object);
        BruteTableUnique(Object);
        Object->Ready = true;
    }
    /// 非查询请求
    if (Key == NULL)
    {
        return NULL;
    }
    /// 首次查询
    /// 保证与Key的域名、记录类型匹配，但记录内容不同
    struct Record** Pos = BruteTableBSearch(Key, Object->Data, Object->End, RecordFindComp);
    switch ((enum QueryType)Key->Type)
    {
    case A:
        /// 不存在符合条件的记录
        if (RecordFindCompU(&Key, Pos))
        {
            /// 键值改为理论最大值
            Key->Data = RECORD_DATA_MAX;
            break;
        }
        /// 若记录正确且内容与键值不同
        if (ipv4Comp(Key->Data.IPv4, (*Pos)->Data.IPv4))
        {
            /// 返回查询结果
            Key->Data = (*Pos)->Data;
            return Key;
        }
        /// 若本条记录为表内首条记录
        if (Pos-- == Object->Data)
        {
            /// 跳出
            break;
        }
        /// 查询上一条记录
        /// 若满足条件
        if (RecordFindCompU(&Key, Pos) == 0)
        {
            /// 返回查询结果
            Key->Data = (*Pos)->Data;
            return Key;
        }
        /// 键值改为理论最大值
        Key->Data = RECORD_DATA_MAX;
        /// 转交二次查询
        break;
    case AAAA:
        /// 不存在符合条件的记录
        if (RecordFindCompU(&Key, Pos))
        {
            /// 键值改为理论最大值
            Key->Data = RECORD_DATA_MAX;
            break;
        }
        /// 若记录正确且内容与键值不同
        if (ipv6Comp(Key->Data.IPv6, (*Pos)->Data.IPv6))
        {
            /// 返回查询结果
            Key->Data = (*Pos)->Data;
            return Key;
        }
        /// 若本条记录为表内首条记录
        if (Pos-- == Object->Data)
        {
            /// 跳出
            break;
        }
        /// 查询上一条记录
        /// 若满足条件
        if (RecordFindCompU(&Key, Pos) == 0)
        {
            /// 返回查询结果
            Key->Data = (*Pos)->Data;
            return Key;
        }
        /// 键值改为理论最大值
        Key->Data = RECORD_DATA_MAX;
        /// 转交二次查询
        break;
    default:
        return NULL;
    }
    /// 二次查询
    /// 保证与Key的域名、记录类型匹配
    /// 此次查询返回同域名、记录类型中满足条件的最后一条记录
    /// 保证当且仅当此次仍无匹配项，全表无匹配项
    Pos = BruteTableBSearch(Key, Object->Data, Object->End, RecordFindComp);
    switch ((enum QueryType)Key->Type)
    {
    case A:
    case AAAA:
        /// 若查询结果不符合条件
        /// 即全表无符合条件的值
        if (RecordFindCompU(&Key, Pos))
        {
            return NULL;
        }
        /// 返回查询结果
        Key->Data = (*Pos)->Data;
        return Key;
    default:
        return NULL;
    }
}

#ifdef _DEBUG
void BruteTablePrint(struct BruteTable* const Object)
{
    if (!Object->Ready)
    {
        BruteTableSort(Object);
        BruteTableUnique(Object);
        Object->Ready = true;
    }
    printf("Size: %d Capability: %d\n", Object->End - Object->Data, Object->Size);
    for (struct Record** it = Object->Data; it != Object->End; it++)
    {
        uchar Part[4];
        memcpy(Part, &(*it)->Data.IPv4, 4);
        printf("%d.%d.%d.%d %s\n", Part[3], Part[2], Part[1], Part[0], (*it)->Domain);
    }
    return;
}
#endif