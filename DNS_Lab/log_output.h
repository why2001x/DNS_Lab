/**
 *  @brief     日志服务
 *  @details   可将日志分级输出至屏幕或文件
 *  @author    王海昱
 *  @version   0.0.1c
 *  @date      2020.07.24-2020.07.28
 *  @warning   日志服务自身错误将仅输出至stderr
 *  @warning   宽字符未经测试
 */

#ifndef _LOG_OUTPUT_H
#define _LOG_OUTPUT_H

#ifdef __cplusplus
extern "C"
{
#endif

	/// 最严重级别
#define LOG_CRNT (1) ///< 此类错误很少使用，输出此类错误后应立即结束相关进程
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
#define LOG_OFF (0) ///< 虚拟日志等级，仅便于调整日志级别


#ifdef NO_LOG_SERVICE
#define InitLog(FileName) (0)
#define SetLogLevel(DstLevel)
#define lprintf(WLevel, Buffer, ...) (0)
#define lputs(WLevel, Buffer) (0)
#else

/**
	 * @brief 日志服务初始化
	 *
	 * 若要输出至非默认文件
	 * 应至少进行一次初始化
	 * 以获得预期的输出效果
	 *
	 * @warning 宽字符文件名未经测试，新建文件可能会导致文件路径存在乱码
	 *
	 * @param FileName 文件名，应包含路径
	 * @return 若初始化正常完成，返回值为0，否则为其他数值
	 */
	extern int InitLog(const char FileName[]);

	/**
		 * @brief 更改日志等级
		 * @param DstLevel 目标日志等级
		 */
	extern void SetLogLevel(const int DstLevel);

	/**
		 * @brief 日志服务-格式化输出
		 *
		 * 格式化输出至日志
		 * 行首将被添加时间与错误级别提示
		 * 使用方法与printf相近
		 *
		 * @warning 注意添加行末换行符
		 *
		 * @param WLevel 错误级别
		 * @param Format 格式化字符串
		 * @param ... 参量表
		 * @return 若正常输出，返回值为用户输出字符数，否则返回值为负值
		 */
	extern int lprintf(const int WLevel, char const* const Format, ...);

	/**
		 * @brief 日志服务-字符串输出
		 *
		 * 将指定字符串输出至日志
		 * 行首将被添加时间与错误级别提示
		 * 行末将被添加换行符
		 * 使用方法与puts相近
		 *
		 *
		 * @param WLevel 错误级别
		 * @param Buffer 输出字符串
		 * @return 若正常输出，返回值为正值，否则返回值为EOF
		 */
	extern int lputs(const int WLevel, char const* const Buffer);

#endif // NO_LOG_SERVICE

#ifdef __cplusplus
}
#endif

#endif // _LOG_OUTPUT_H
