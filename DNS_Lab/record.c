#include "stdafx.h"

#include "log_output.h"

#include "record.h"

const union RecordData RECORD_DATA_MAX = { .IPv6.u.Word = {-1,-1,-1,-1,-1,-1,-1,-1} };

int RecordSortComp(void* LPtr, void* RPtr)
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
	case AAAA:
		return ipv6Comp(Lhs.Data.IPv6, Rhs.Data.IPv6);
	default:
		return 0;
	}
}
int RecordFindComp(void* KeyPtr, void* ParPtr)
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
		if (ipv4Comp(Key.Data.IPv4, RECORD_DATA_MAX.IPv4) == 0)
		{
			return 0;
		}
		return ipv4Comp(Key.Data.IPv4, Par.Data.IPv4);
	case AAAA:
		if (ipv6Comp(Key.Data.IPv6, RECORD_DATA_MAX.IPv6) == 0)
		{
			return 0;
		}
		return ipv6Comp(Key.Data.IPv6, Par.Data.IPv6);
	default:
		return 0;
	}
}

int RecordFindCompU(void* KeyPtr, void* ParPtr)
{
	struct Record Key = **(struct Record**)KeyPtr, Par = **(struct Record**)ParPtr;
	int Comp = strcmp(Key.Domain, Par.Domain);
	if (Comp)
	{
		return Comp;
	}
	return (Key.Type > Par.Type) - (Key.Type < Par.Type);
}

int RecordNodeInit(struct Record* const Object, const uchar Type, const union RecordData Data, const char* const Domain)
{
	lputs(LOG_DBUG, "RecordNode: Initializing a RecordNode.");
	int Length = strlen(Domain);
	char* Temp = (char*)malloc((size_t)Length + 1);
	if (Temp == NULL)
	{
		lputs(LOG_INFO, "RecordNode: Error while allocating memory for a new Domain!");
		return -1;
	}
	strcpy(Temp, Domain);
	switch ((enum QueryType)Type)
	{
	case A:
		lputs(LOG_DBUG, "RecordNode: Record A.");
		Object->Data = Data;
		break;
	case AAAA:
		lputs(LOG_DBUG, "RecordNode: Record AAAA.");
		Object->Data = Data;
		break;
	default:
		lprintf(LOG_INFO, "RecordNode: Unknown query type %d skipped.\n", Type);
		free(Temp);
		return 1;
	}
	Object->Type = Type;
	free(Object->Domain);
	Object->Domain = Temp;
	return 0;
}

struct Record* RecordNode(const uchar Type, const union RecordData Data, const char* const Domain)
{
	lputs(LOG_DBUG, "RecordNode: Constructing new RecordNode.");
	struct Record* Node = (struct Record*)malloc(sizeof(struct Record));
	if (Node == NULL)
	{
		lputs(LOG_INFO, "RecordNode: Error while allocating memory for a new Record!");
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

void RecordNodeFree(struct Record* const Object)
{
	if (Object == NULL)
	{
		return;
	}
	lputs(LOG_DBUG, "RecordNode: Releasing a RecordNode.");
	if (Object->Domain != NULL)
	{
		free(Object->Domain);
		Object->Domain = NULL;
	}
	free(Object);
	return;
}
