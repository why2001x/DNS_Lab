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
        memcpy(Object, &Temp, sizeof(struct BruteTable));
    }
    /// 添加待插入记录
    *(Object->End) = Item;
    Object->End++;
    Object->Ready = false;
    lputs(LOG_DBUG, "BruteTable: A Record add to the end of BruteTable.");
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
    if (Key == NULL)
    {
        return NULL;
    }
    if (!Object->Ready)
    {
        BruteTableSort(Object);
        BruteTableUnique(Object);
        Object->Ready = true;
    }
    struct Record** Pos = BruteTableBSearch(Key, Object->Data, Object->End, RecordFindComp);
    switch ((enum QueryType)Key->Type)
    {
    case A:
        if (RecordFindCompU(&Key, Pos))
        {
            Key->Data.IPv4 = -1;
            break;
        }
        if (Key->Data.IPv4 != (*Pos)->Data.IPv4)
        {
            Key->Data.IPv4 = (*Pos)->Data.IPv4;
            return Key;
        }
        if (Pos-- == Object->Data)
        {
            break;
        }
        if (RecordFindCompU(&Key, Pos) == 0)
        {
            Key->Data.IPv4 = (*Pos)->Data.IPv4;
            return Key;
        }
        Key->Data.IPv4 = -1;
        break;
    default:
        return NULL;
    }
    Pos = BruteTableBSearch(Key, Object->Data, Object->End, RecordFindComp);
    switch ((enum QueryType)Key->Type)
    {
    case A:
        if (RecordFindCompU(&Key, Pos))
        {
            return NULL;
        }
        Key->Data.IPv4 = (*Pos)->Data.IPv4;
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