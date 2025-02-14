﻿#include "stdafx.h"

#include "log_output.h"
#include "record_table.h"
#include "url_filter.h"

static RecordTable List;
static bool Ready = true;
static time_t FileTime;
static char LastFileName[65536];
static HANDLE FilterLock;

int InitURLFilter(const char* const FileName)
{
    if (FilterLock == NULL)
    {
        FilterLock = CreateMutex(NULL, FALSE, NULL);
    }
    lputs(LOG_INFO, "URLFilter: Initializing...");
    FILE* RulesFile;
    lputs(LOG_DBUG, "URLFilter: Reading the rule file...");
    if ((RulesFile = fopen(FileName, "r")) == NULL)
    {
        lputs(LOG_DBUG, "URLFilter: Error while opening the specified file!");
        lputs(LOG_DBUG, "URLFilter: Existing rules unchanged.");
        lputs(LOG_INFO, "URLFilter: Initializing Failed!");
        return 1;
    }
    RecordTable Temp = { NULL };
    lputs(LOG_DBUG, "URLFilter: Constructing the new filter...");
    int Count = 0;
    for (char Buffer[BUFFER_SIZE]; fgets(Buffer, BUFFER_SIZE, RulesFile); Count++)
    {
        /// 清除行末多余字符
        if (strlen(Buffer) >= (BUFFER_SIZE - 1))
        {
            scanf("%*[\n\r\0]");
        }
        /// 注释
        if (strstr(Buffer, "#") != NULL)
        {
            lprintf(LOG_DBUG, "URLFilter: Explanatory note. [line: %d]\n", Count + 1);
            *strstr(Buffer, "#") = '\0';
        }
        /// 超长行且未被注释
        if (strlen(Buffer) >= (BUFFER_SIZE - 1))
        {
            lprintf(LOG_DBUG, "URLFilter: Invalid rule. (Too long) [line: %d]\n", Count + 1);
            continue;
        }
        char IP[BUFFER_SIZE], Domain[BUFFER_SIZE];
        {
            int Result = sscanf(Buffer, "%s%s", IP, Domain);
            if (Result == 0)
            {
                continue;
            }
            else if (Result != 2 || (strlen(IP) >= IP_BUF_SIZE))
            {
                lprintf(LOG_DBUG, "URLFilter: Invalid rule. (IP part too long) [line: %d]\n", Count + 1);
                continue;
            }
        }
        enum QueryType Type;
        union RecordData Data;
        if (inet_pton(AF_INET, IP, &Data) == 1)
        {
            Type = A;
        }
        else if (inet_pton(AF_INET6, IP, &Data) == 1)
        {
            Type = AAAA;
        }
        else
        {
            lprintf(LOG_DBUG, "URLFilter: Invalid IP %s not in presentation format) [line: %d]\n", IP, Count + 1);
            continue;
        }

#ifdef _MSC_VER
        _strlwr(Domain);
#else
        strlwr(Domain);
#endif
        struct Record* NewRec = RecordNode(Type, Data, Domain);
        if (NewRec == NULL)
        {
            lputs(LOG_DBUG, "URLFilter: Releasing the temporary filter...");
            RecordTableClear(&Temp);
            fclose(RulesFile);
            lputs(LOG_DBUG, "URLFilter: Rule file is closed.");
            lputs(LOG_DBUG, "URLFilter: Existing rules unchanged.");
            lputs(LOG_INFO, "URLFilter: Initializing Failed!");
            return 1;
        }
        {
            int Return = RecordTableAppend(&Temp, NewRec);
            if (Return)
            {
                if (Return < 0)
                {
                    lputs(LOG_DBUG, "URLFilter: Releasing the temporary filter...");
                    RecordTableClear(&Temp);
                    fclose(RulesFile);
                    lputs(LOG_DBUG, "URLFilter: Rule file is closed.");
                    lputs(LOG_DBUG, "URLFilter: Existing rules unchanged.");
                    lputs(LOG_INFO, "URLFilter: Initializing Failed!");
                    return 1;
                }
                lputs(LOG_INFO, "URLFilter: There're too many rules in the file.");
                lputs(LOG_INFO, "URLFilter: The system may not work as expected.");
                break;
            }
        }
    }
    lprintf(LOG_INFO, "URLFilter: %d rules read from file.\n", Count);
#ifdef _MSC_VER
    int fd = _fileno(RulesFile);
    struct _stat Buffer;
    _fstat(fd, &Buffer);
#else
#endif
    fclose(RulesFile);
    lputs(LOG_DBUG, "URLFilter: Rule file is closed.");
    RecordTableClear(&List);
    List = Temp;
    FileTime = Buffer.st_mtime;
    if (FileName != LastFileName)
    {
        strcpy(LastFileName, FileName);
    }
#ifdef USE_BRUTE
    RecordCheck(&List, NULL);
#endif
    lputs(LOG_INFO, "URLFilter: Initializing Succeded!");
    return 0;
}

static inline void URL2Domain(const char* const URLString, char* const Domain)
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
    for (char* i = Start; i != Finish && *i != '\0'; i++)
    {
        Domain[Length++] = *i;
    }
    Domain[Length] = '\0';
    return;
}
static inline void FilterHotFix()
{
    if (*LastFileName == NULL)
    {
        return;
    }
    if (FilterLock == NULL)
    {
        FilterLock = CreateMutex(NULL, FALSE, NULL);
    }
    WaitForSingleObject(FilterLock, INFINITE);
    FILE* File = fopen(LastFileName, "r");
#ifdef _MSC_VER
    int fd = _fileno(File);
    struct _stat Buffer;
    _fstat(fd, &Buffer);
#else
#endif
    fclose(File);
    if (FileTime != Buffer.st_mtime)
    {
        InitURLFilter(LastFileName);
    }
    ReleaseMutex(FilterLock);
    return;
}
int URLCheck(const int Type, const char* const URLString, void* const Dst)
{
    lputs(LOG_DBUG, "URLFilter: Accepted a URL query.");
    struct Record Key;
    Key.Type = Type;
    switch ((enum QueryType)Key.Type)
    {
    case A:
    case AAAA:
        if (Dst == NULL)
        {
            Key.Data = RECORD_DATA_MAX;
            break;
        }
        Key.Data = *(union RecordData*)Dst;
        break;
    default:
        lprintf(LOG_DBUG, "URLFilter: Unknown query type %d.\n", Type);
        return 0;
    }
    Key.Domain = (char*)malloc(strlen(URLString) + 1);
    if (Key.Domain == NULL)
    {
        lputs(LOG_INFO, "URLFilter: Error while allocating memory!");
        return 0;
    }
    URL2Domain(URLString, Key.Domain);
    FilterHotFix();
    const struct Record* const Result = RecordCheck(&List, &Key);
    free(Key.Domain);
    Key.Domain = NULL;
    if (Result == NULL)
    {
        lputs(LOG_DBUG, "URLFilter: Query not found.");
        return 0;
    }
    lputs(LOG_DBUG, "URLFilter: Query found.");
    if (Dst == NULL)
    {
        lputs(LOG_DBUG, "URLFilter: But asked for no result.");
        return 1;
    }
    switch ((enum QueryType)Key.Type)
    {
    case A:
    case AAAA:
        *(union RecordData*)Dst = Result->Data;
        break;
    default:
        return 1;
    }
    return 1;
}
