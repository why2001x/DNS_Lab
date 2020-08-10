/**
 *  @brief     日志服务
 *  @details   日志服务相关实现
 *  @author    王海昱
 *  @version   0.0.1d
 *  @date      2020.07.24-2020.07.10
 */

#include "stdafx.h"

#include "log_output.h"

static FILE* LogFile;

static int LogLevel = LOG_WARN;

static bool Ready;

int InitLog(const char FileName[])
{
    static const char Default[] = "./log.txt";
    if (!FileName || !(*FileName))
    {
        fputs("Using the default log file.\n", stderr);
        FileName = Default;
    }
    if (LogFile)
    {
        fputs("Try to restart the log service.\n", stderr);
        switch (fclose(LogFile))
        {
        case 0:
            Ready = false;
            break;
        case EOF:
            fputs("Unable to close the LogFile!\n", stderr);
            if (LogFile)
            {
                fputs("Using the last one instead.\n", stderr);
            }
            return 1;
        default:
            fputs("Unknown Error!\n", stderr);
            return 1;
        }
    }
    if ((LogFile = fopen(FileName, "a")) == NULL)
    {
        fputs("Unable to open the specified LogFile!\n", stderr);
        return 1;
    }
    Ready = true;
    return 0;
}

void SetLogLevel(const int DstLevel)
{
    LogLevel = DstLevel;
    return;
}

static int LogNotReady(void)
{
    if (Ready)
    {
        return 0;
    }
    if (InitLog(NULL))
    {
        fputs("Start the log service failed.\n", stderr);
        return 1;
    }
    return 0;
}

static int lprefix(const int WLevel)
{
    static const char* Type[] = { [LOG_CRNT] = "Critical",
                                 [LOG_DBUG] = "Debug",
                                 [LOG_ERRN] = "Error",
                                 [LOG_INFO] = "Info",
                                 [LOG_WARN] = "Warning" };
    if ((*Type[WLevel]) == '\0')
    {
        fputs("Unknown Warning Level.\n", stderr);
        return EOF;
    }
    char Buffer[30];
    time_t timer = time(NULL);
    strftime(Buffer, 30, "%Y-%m-%d %H:%M:%S", localtime(&timer));
    fprintf(LogFile, "%s %s: ", Buffer, Type[WLevel]);
    return 0;
}

int lprintf(const int WLevel, char const* const Format, ...)
{
    if (WLevel > LogLevel)
    {
        return 0;
    }
    va_list ArgList;
    va_start(ArgList, Format);
    if (LogNotReady())
    {
        return EOF;
    }
    if (lprefix(WLevel))
    {
        return EOF;
    }
    int ret = vfprintf(LogFile, Format, ArgList);
    fflush(LogFile);
    return ret;
}

int lputs(const int WLevel, char const* const Buffer)
{
    if (WLevel > LogLevel)
    {
        return 0;
    }
    if (LogNotReady())
    {
        return EOF;
    }
    if (lprefix(WLevel))
    {
        return EOF;
    }
    int ret = fputs(Buffer, LogFile);
    if (ret < 0)
    {
        return ret;
    }
    if (fputc('\n', LogFile))
    {
        fflush(LogFile);
        return 0;
    }
    return EOF;
}