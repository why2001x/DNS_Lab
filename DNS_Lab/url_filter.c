/**
 *  @brief     URL过滤器
 *  @details   URL过滤器相关实现
 *  @author    王海昱
 *  @version   0.0.2
 *  @date      2020.07.24-2020.08.10
 */

#include "stdafx.h"

#include "log_output.h"
#include "record_table.h"
#include "url_filter.h"

/// 输入缓冲区长度限制
#define BUFFER_SIZE 300


RecordTable List;

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
    RecordTable Temp = { false, NULL, NULL, 0 };
    lputs(LOG_INFO, "URLFilter: Constructing the new filter...");
    int Count = 0;
    for (char Buffer[BUFFER_SIZE]; fgets(Buffer, BUFFER_SIZE, RulesFile); Count++)
    {
        char Domain[BUFFER_SIZE];
        if (strstr(Buffer, "#") != NULL)
        {
            lprintf(LOG_DBUG, "URLFilter: Explanatory note. [line: %d]\n", Count + 1);
            *strstr(Buffer, "#") = '\0';
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
    RecordTableFree(&List);
    List = Temp;
    lputs(LOG_WARN, "URLFilter: Initializing Succeded!");
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

int URLCheck(const int Type, const char* const URLString, void* const Dst)
{
    lputs(LOG_INFO, "URLFilter: Accepted a URL query.");
    struct Record Key;
    Key.Type = Type;
    switch ((enum QueryType)Key.Type)
    {
    case A:
        if (Dst == NULL)
        {
            Key.Data.IPv4 = -1;
            break;
        }
        Key.Data.IPv4 = *(ipv4_t*)Dst;
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
    if (Dst == NULL)
    {
        lputs(LOG_INFO, "URLFilter: But asked for no result.");
        return 1;
    }
    switch ((enum QueryType)Key.Type)
    {
    case A:
        *(ipv4_t*)Dst = Result->Data.IPv4;
        break;
    default:
        return 1;
    }
    return 1;
}
