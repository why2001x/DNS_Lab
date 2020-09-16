#include "stdafx.h"

#include "log_output.h"

#ifndef NO_LOG_SERVICE

static HANDLE LogLock;

static FILE* LogFile;

static int LogLevel = LOG_OFF;

static bool Ready;

int InitLog(FILE* Dst)
{
    if (Dst == NULL)
    {
        fputs("Using stderr for log.\n", stderr);
        Dst = stderr;
    }
    if (LogFile && LogFile != stderr)
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
    LogFile = stderr;
    Ready = true;
    return 0;
}

void SetLogLevel(const int DstLevel)
{
    LogLevel = DstLevel;
    return;
}
int GetLogLevel()
{
    return LogLevel;
}

static int LogNotReady(void)
{
    if (LogLock == NULL)
    {
        LogLock = CreateMutex(NULL, FALSE, NULL);
    }
    WaitForSingleObject(LogLock, INFINITE);
    if (Ready)
    {
        return 0;
    }
    if (InitLog(NULL))
    {
        fputs("Start the log service failed.\n", stderr);
        ReleaseMutex(LogLock);
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
        ReleaseMutex(LogLock);
        return EOF;
    }
    int ret = vfprintf(LogFile, Format, ArgList);
    fflush(LogFile);
    ReleaseMutex(LogLock);
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
        ReleaseMutex(LogLock);
        return EOF;
    }
    int ret = fputs(Buffer, LogFile);
    if (ret < 0)
    {
        ReleaseMutex(LogLock);
        return ret;
    }
    if (fputc('\n', LogFile))
    {
        fflush(LogFile);
        ReleaseMutex(LogLock);
        return 0;
    }
    ReleaseMutex(LogLock);
    return EOF;
}

#endif // !NO_LOG_SERVICE