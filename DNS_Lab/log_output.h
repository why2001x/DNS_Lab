#ifndef _LOG_OUTPUT_H
#define _LOG_OUTPUT_H
#ifdef __cplusplus
extern "C" {
#endif
    /**
     * @defgroup LogService
     * @{
     */
     /// 最严重级别
#define LOG_CRNT (1)
#define LOG_CRITICAL LOG_CRNT
/// 程序错误
#define LOG_ERRN (LOG_CRNT << 1)
#define LOG_ERROR LOG_ERRN
/// 程序未按预期运行
#define LOG_WARN (LOG_ERRN << 1)
#define LOG_WARNING LOG_WARN
/// 程序正常运行信息
#define LOG_INFO (LOG_WARN << 1)
/// 程序调试信息
#define LOG_DBUG (LOG_INFO << 1)
#define LOG_DEBUG LOG_DBUG
/// 关闭日志
#define LOG_OFF (0)
    extern int InitLog(const char FileName[]); /// 日志服务初始化
    extern void SetLogLevel(const int DstLevel); /// 更改日志等级
    extern int lprintf(const int WLevel, char const* const Format, ...); /// 日志服务-格式化输出
    extern int lputs(const int WLevel, char const* const Buffer); /// 日志服务-字符串输出
    /**@}*/
#ifdef __cplusplus
}
#endif
#endif // _LOG_OUTPUT_H