/**
 *  @brief     URL过滤器
 *  @details   URL过滤器相关实现
 *  @author    王海昱
 *  @version   0.0.1a
 *  @date      2020.07.24-2020.07.28
 *  @warning   仅支持至多$8*10^6$条记录
 */

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "log_output.h"
#include "url_filter.h"

#define BUFFER_SIZE 300 ///< 输入缓冲区长度限制

union RecordData
{
	ipv4_t IPv4;
};
struct Record
{
	uchar Type;
	union RecordData Data;
	char* Domain;
};

static int RecordSortComp(void* LPtr, void* RPtr)
{
	struct Record Lhs = **(struct Record**)LPtr, Rhs = **(struct Record**)RPtr;
	int Comp = strcmp(Lhs.Domain, Rhs.Domain);
	if (Comp)
	{
		return Comp;
	}
	if (Lhs.Type != Rhs.Type)
	{
		return (Lhs.Type > Rhs.Type) - (Lhs.Type < Rhs.Type);
	}
	switch ((enum QueryType)Lhs.Type)
	{
	case A:
		return ipv4Comp(Lhs.Data.IPv4, Rhs.Data.IPv4);
	default:
		return 0;
	}
}
static int RecordFindComp(void* KeyPtr, void* ParPtr)
{
	struct Record Key = **(struct Record**)KeyPtr, Par = **(struct Record**)ParPtr;
	int Comp = strcmp(Key.Domain, Par.Domain);
	if (Comp)
	{
		return Comp;
	}
	if (Key.Type != Par.Type)
	{
		return (Key.Type > Par.Type) - (Key.Type < Par.Type);
	}
	switch ((enum QueryType)Key.Type)
	{
	case A:
		if (Key.Data.IPv4 == -1)
		{
			return 0;
		}
		return ipv4Comp(Key.Data.IPv4, Par.Data.IPv4);
	default:
		return 0;
	}
}

static int RecordFindCompU(void* KeyPtr, void* ParPtr)
{
	struct Record Key = **(struct Record**)KeyPtr, Par = **(struct Record**)ParPtr;
	int Comp = strcmp(Key.Domain, Par.Domain);
	if (Comp)
	{
		return Comp;
	}
	return (Key.Type > Par.Type) - (Key.Type < Par.Type);
}

static int RecordNodeInit(struct Record* const Object, const uchar Type, const union RecordData Data, const char* const Domain)
{
	lputs(LOG_DBUG, "URLFilter: Initializing a RecordNode.");
	int Length = strlen(Domain);
	char* Temp = (char*)malloc((size_t)Length + 1);
	if (Temp == NULL)
	{
		lputs(LOG_ERRN, "URLFilter: Error while allocating memory for a new Domain!");
		return -1;
	}
	strcpy(Temp, Domain);
	switch ((enum QueryType)Type)
	{
	case A:
		lputs(LOG_DBUG, "URLFilter: Record A.");
		Object->Data.IPv4 = Data.IPv4;
		break;
	default:
		lprintf(LOG_WARN, "URLFilter: Unknown query type %d skipped.\n", Type);
		free(Temp);
		return 1;
	}
	Object->Type = Type;
	free(Object->Domain);
	Object->Domain = Temp;
	return 0;
}

static struct Record* RecordNode(const uchar Type, const union RecordData Data, const char* const Domain)
{
	lputs(LOG_DBUG, "URLFilter: Constructing new RecordNode.");
	struct Record* Node = malloc(sizeof(struct Record));
	if (Node == NULL)
	{
		lputs(LOG_ERRN, "URLFilter: Error while allocating memory for a new Record!");
		return NULL;
	}
	memset(Node, 0, sizeof(struct Record));
	if (RecordNodeInit(Node, Type, Data, Domain))
	{
		free(Node);
		return NULL;
	}
	return Node;
}

static void RecordNodeFree(struct Record* const Object)
{
	if (Object == NULL)
	{
		return;
	}
	lputs(LOG_DBUG, "URLFilter: Releasing a RecordNode.");
	if (Object->Domain != NULL)
	{
		free(Object->Domain);
		Object->Domain = NULL;
	}
	free(Object);
	return;
}

struct TrieNode
{
	bool HasRec, HasLower;
	struct Record* Rec;
	struct TrieNode* Next[26];
};

struct HashTable
{
	char** String;
	struct TrieNode** Data;
	size_t Size;
};

struct BruteTable
{
	struct Record** Data, ** End;
	size_t Size;
};

static void BruteTableClear(struct BruteTable* const Object)
{
	if (Object == NULL)
	{
		return;
	}
	lputs(LOG_DBUG, "URLFilter: Cleaning a BruteTable.");
	free(Object->Data);
	Object->Data = Object->End = NULL;
	Object->Size = 0;
	return;
}

static void BruteTableFree(struct BruteTable* const Object)
{
	if (Object == NULL)
	{
		return;
	}
	lputs(LOG_DBUG, "URLFilter: Releasing a BruteTable.");
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
static int BruteTableAppend(struct BruteTable* const Object, struct Record* const Item)
{
	if (Object->Data + Object->Size == Object->End)
	{
		lputs(LOG_DBUG, "URLFilter: Expanding the BruteTable.");
		if (Object->Size >= MAX_SIZE)
		{
			lputs(LOG_WARN, "URLFilter: The number of rules more than the limit! Skipped.");
			return 1;
		}
		struct BruteTable Temp = { NULL, NULL, Object->Size + min(STEP_BT_MAX, max(STEP_BT_MIN, Object->Size)) };
		Temp.Data = (struct Record**)malloc(Temp.Size * sizeof(struct Record*));
		if (Temp.Data == NULL)
		{
			lputs(LOG_ERRN, "URLFilter: Error while allocating memory!");
			return -1;
		}
		memset(Temp.Data, 0, Temp.Size * sizeof(struct Record*));
		Temp.End = Temp.Data + Object->Size;
		if (Object->Data != NULL)
		{
			lputs(LOG_DBUG, "URLFilter: Copy the data on the older table.");
			memcpy(Temp.Data, Object->Data, Object->Size * sizeof(struct Record*));
			BruteTableClear(Object);
			lputs(LOG_DBUG, "URLFilter: The older table was released.");
		}
		lputs(LOG_DBUG, "URLFilter: Change to the newer one.");
		memcpy(Object, &Temp, sizeof(struct BruteTable));
	}
	*(Object->End) = Item;
	Object->End++;
	lputs(LOG_DBUG, "URLFilter: A Record add to the end of BruteTable.");
	return 0;
}
#undef MAX_SIZE
#undef STEP_BT_MIN
#undef STEP_BT_MAX

static void BruteTableSort(struct BruteTable* const Object)
{
	lputs(LOG_DBUG, "URLFilter: Sorting the BruteTable.");
	qsort(Object->Data, Object->End - Object->Data, sizeof(struct Record*), RecordSortComp);
	return;
}

static void BruteTableUnique(struct BruteTable* const Object)
{
	lputs(LOG_DBUG, "URLFilter: Making the BruteTable unique.");
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

static const struct Record* const BruteTableCheck(struct BruteTable* const Object, struct Record* const Key)
{
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

static void BruteTablePrint(struct BruteTable* const Object)
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

#if USE_HASH_TRIE
typedef struct HashTable RecordTable;
RecordTable List;
#else
#define USE_BRUTE
typedef struct BruteTable RecordTable;
const int (*RecordTableAppend)(RecordTable* const, struct Record* const) = BruteTableAppend;
const void (*RecordTableClear)(RecordTable* const) = BruteTableClear;
const void (*RecordTableSort)(RecordTable* const) = BruteTableSort;
const void (*RecordTableUnique)(RecordTable* const) = BruteTableUnique;
const struct Record* const(*RecordCheck)(RecordTable* const, struct Record* const) = BruteTableCheck;
RecordTable List;
#endif


int InitURLFilter(const char* const FileName)
{
	lputs(LOG_WARN, "URLFilter: Initializing...");
	FILE* RulesFile;
	lputs(LOG_INFO, "URLFilter: Reading the rule file...");
	if ((RulesFile = fopen(FileName, "r")) == NULL)
	{
		lputs(LOG_ERRN, "URLFilter: Error while opening the specified file!");
		lputs(LOG_ERRN, "URLFilter: Existing rules unchanged.");
		lputs(LOG_ERRN, "URLFilter: Initializing Failed!");
		return 1;
	}
	RecordTable Temp = { NULL, NULL, 0 };
	lputs(LOG_INFO, "URLFilter: Constructing the new filter...");
	int Count = 0;
	for (char Buffer[BUFFER_SIZE]; fgets(Buffer, BUFFER_SIZE, RulesFile); Count++)
	{
		char Domain[BUFFER_SIZE];
		if (sscanf(Buffer, "%[#]", Domain))
		{
			lprintf(LOG_DBUG, "URLFilter: Explanatory note. [line: %d]\n", Count + 1);
			continue;
		}
		int Part[4];
		if (sscanf(Buffer, "%d.%d.%d.%d%s", &Part[3], &Part[2], &Part[1], &Part[0], Domain) != 5)
		{
			lprintf(LOG_WARN, "URLFilter: Invalid rule. [line: %d]\n", Count + 1);
			continue;
		}
		if ((Part[0] | Part[1] | Part[2] | Part[3]) > 255)
		{
			lprintf(LOG_WARN, "URLFilter: Invalid IP %d.%d.%d.%d. [line: %d]\n", Part[3], Part[2], Part[1], Part[0], Count + 1);
			continue;
		}
		_strlwr(Domain);
#define L1(x) ((x) << 8)
		union RecordData Data = { .IPv4 = L1(L1(L1(Part[3]) | Part[2]) | Part[1]) | Part[0] };
#undef L1
		struct Record* NewRec = RecordNode((enum QueryType)A, Data, Domain);
		if (NewRec == NULL)
		{
			lputs(LOG_ERRN, "URLFilter: Cleaning the temporary filter...");
			RecordTableClear(&Temp);
			lputs(LOG_ERRN, "URLFilter: Existing rules unchanged.");
			lputs(LOG_ERRN, "URLFilter: Initializing Failed!");
			return 1;
		}
		int Return = RecordTableAppend(&Temp, NewRec);
		if (Return)
		{
			if (Return < 0)
			{
				lputs(LOG_ERRN, "URLFilter: Cleaning the temporary filter...");
				RecordTableClear(&Temp);
				lputs(LOG_ERRN, "URLFilter: Existing rules unchanged.");
				lputs(LOG_ERRN, "URLFilter: Initializing Failed!");
				return 1;
			}
			lputs(LOG_WARN, "URLFilter: There're too many rules in the file.");
			lputs(LOG_WARN, "URLFilter: The system may not work as expected.");
			break;
		}
	}
	lprintf(LOG_INFO, "URLFilter: %d rules read from file.", Count);
	fclose(RulesFile);
	lputs(LOG_INFO, "URLFilter: Rule file is closed.");
#ifdef USE_BRUTE
	lputs(LOG_DBUG, "URLFilter: Using BruteTable for recording.");
	RecordTableSort(&Temp);
	RecordTableUnique(&Temp);
#endif
	RecordTableClear(&List);
	List = Temp;
	lputs(LOG_WARN, "URLFilter: Initializing Succeded!");
	return 0;
}

inline void URL2Domain(const char* const URLString, char* const Domain)
{
	lputs(LOG_DBUG, "URLFilter: Extracting the domain from URL.");
	char* Start = strstr(URLString, "://");
	if (Start == NULL)
	{
		Start = URLString;
	}
	else
	{
		Start += 3;
	}
	char* Finish = strstr(Start, "/");
	int Length = 0;
	for (int i = Start - URLString; &URLString[i] != Finish && URLString[i] != '\0'; i++)
	{
		Domain[Length++] = URLString[i];
	}
	Domain[Length] = '\0';
	return;
}

int URLCheck(const int Type, const char* const URLString, void* const IP)
{
	lputs(LOG_INFO, "URLFilter: Accepted a URL query.");
	struct Record Key;
	Key.Type = Type;
	switch ((enum QueryType)Key.Type)
	{
	case A:
		if (IP == NULL)
		{
			Key.Data.IPv4 = -1;
			break;
		}
		Key.Data.IPv4 = *(ipv4_t*)IP;
		break;
	default:
		lprintf(LOG_WARN, "URLFilter: Unknown query type %d.\n", Type);
		return 0;
	}
	Key.Domain = (char*)malloc(strlen(URLString) + 1);
	if (Key.Domain == NULL)
	{
		lputs(LOG_ERRN, "URLFilter: Error while allocating memory!");
		return 0;
	}
	URL2Domain(URLString, Key.Domain);
	const struct Record* const Result = RecordCheck(&List, &Key);
	free(Key.Domain);
	Key.Domain = NULL;
	if (Result == NULL)
	{
		lputs(LOG_INFO, "URLFilter: Query not found.");
		return 0;
	}
	lputs(LOG_INFO, "URLFilter: Query found.");
	if (IP == NULL)
	{
		lputs(LOG_INFO, "URLFilter: But asked for no result.");
		return 1;
	}
	switch ((enum QueryType)Key.Type)
	{
	case A:
		*(ipv4_t*)IP = Result->Data.IPv4;
		break;
	default:
		return 1;
	}
	return 1;
}

#undef USE_BRUTE