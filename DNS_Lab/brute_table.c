/**
 * @brief     基本记录表
 * @details   升序记录，二分查找，支持同一关键字多个键值
 * @author    王海昱
 * @version   0.0.1a
 * @date      2020.08.09-2020.08.09
 * @warning   仅支持至多$8*10^6$条记录
 * @warning   宽字符未经测试
 */

#include "stdafx.h"

#include "log_output.h"
#include "brute_table.h"

void BruteTableClear(struct BruteTable* const Object)
{
	if (Object == NULL)
	{
		return;
	}
	lputs(LOG_DBUG, "BruteTable: Cleaning a BruteTable.");
	free(Object->Data);
	Object->Data = Object->End = NULL;
	Object->Size = 0;
	Object->Ready = false;
	return;
}

void BruteTableFree(struct BruteTable* const Object)
{
	if (Object == NULL)
	{
		return;
	}
	lputs(LOG_DBUG, "BruteTable: Releasing a BruteTable.");
	for (int i = 0; i < Object->Size; i++)
	{
		RecordNodeFree(Object->Data[i]);
		Object->Data[i] = NULL;
	}
	BruteTableClear(Object);
	return;
}

#define MAX_SIZE (1 << 23)
#define STEP_BT_MIN 16
#define STEP_BT_MAX 2048
int BruteTableAppend(struct BruteTable* const Object, struct Record* const Item)
{
	if (Object->Data + Object->Size == Object->End)
	{
		lputs(LOG_DBUG, "BruteTable: Expanding the BruteTable.");
		if (Object->Size >= MAX_SIZE)
		{
			lputs(LOG_WARN, "BruteTable: The number of rules more than the limit! Skipped.");
			return 1;
		}
		struct BruteTable Temp = { NULL, NULL, Object->Size + min(STEP_BT_MAX, max(STEP_BT_MIN, Object->Size)) };
		Temp.Data = (struct Record**)malloc(Temp.Size * sizeof(struct Record*));
		if (Temp.Data == NULL)
		{
			lputs(LOG_ERRN, "BruteTable: Error while allocating memory!");
			return -1;
		}
		memset(Temp.Data, 0, Temp.Size * sizeof(struct Record*));
		Temp.End = Temp.Data + Object->Size;
		if (Object->Data != NULL)
		{
			lputs(LOG_DBUG, "BruteTable: Copy the data on the older table.");
			memcpy(Temp.Data, Object->Data, Object->Size * sizeof(struct Record*));
			BruteTableClear(Object);
			lputs(LOG_DBUG, "BruteTable: The older table was released.");
		}
		lputs(LOG_DBUG, "BruteTable: Change to the newer one.");
		memcpy(Object, &Temp, sizeof(struct BruteTable));
	}
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
	return;
}

static void BruteTableUnique(struct BruteTable* const Object)
{
	lputs(LOG_DBUG, "BruteTable: Making the BruteTable unique.");
	if (Object->Data == Object->End)
	{
		return;
	}
	struct Record** Dst = Object->Data;
	for (struct Record** it = Object->Data + 1; it != Object->End; it++)
	{
		if (RecordSortComp(it, Dst))
		{
			Dst++;
			*Dst = *it;
		}
	}
	Dst++;
	for (struct Record** it = Dst; it != Object->End; it++)
	{
		*it = NULL;
	}
	Object->End = Dst;
	return;
}

static struct Record** BruteTableBSearch(struct Record* const Key, const struct Record** const Begin, const struct Record** const End)
{
	struct Record** Low = Begin, ** High = End;
	if (Low == High)
	{
		return NULL;
	}
	for (; High - Low > 1;)
	{
		struct Record** Half = (High - Low) / 2 + Low;
		if (RecordFindComp(&Key, Half) < 0)
		{
			High = Half;
		}
		else
		{
			Low = Half;
		}
	}
	return Low;
}

struct Record* const BruteTableCheck(struct BruteTable* const Object, struct Record* const Key)
{
	if (!Object->Ready)
	{
		BruteTableSort(Object);
		BruteTableUnique(Object);
		Object->Ready = true;
	}
	for (;;)
	{
		struct Record** Pos = BruteTableBSearch(Key, Object->Data, Object->End);
		if (RecordFindCompU(&Key, Pos) == 0)
		{
			switch ((enum QueryType)Key->Type)
			{
			case A:
				if (Key->Data.IPv4 == -1)
				{
					return *Pos;
				}
				Key->Data.IPv4 = -1;
				if (Pos-- == Object->Data)
				{
					break;
				}
				if (RecordFindCompU(&Key, Pos) == 0)
				{
					return *Pos;
				}
				break;
			default:
				return NULL;
			}
		}
		else
		{
			switch ((enum QueryType)Key->Type)
			{
			case A:
				if (Key->Data.IPv4 == -1)
				{
					return NULL;
				}
				Key->Data.IPv4 = -1;
				break;
			default:
				return NULL;
			}
		}
	}
}

#ifdef _DEBUG
void BruteTablePrint(struct BruteTable* const Object)
{
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