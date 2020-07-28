/**
 *  @brief     URL过滤器
 *  @details   过滤器将根据资源文件提供的内容,对特定网址返回指定IP
 *  @author    王海昱
 *  @version   0.0.1a
 *  @date      2020.07.23-2020.07.26
 *  @warning   宽字符未经测试
 *  @bug       仅支持A记录
 */

#ifndef _URL_FLITER_H
#define _URL_FLITER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "dnsdef.h"

	/**
	 * @brief URL过滤器初始化
	 *
	 * 启动或更新资源文件后
	 * 应至少进行一次初始化
	 * 以获得预期的过滤效果
	 *
	 * @warning 仅支持纯ASCII文件
	 *
	 * @param FileName 文件名，应包含路径
	 * @return 若初始化正常完成，返回值为0，否则为其他数值
	 */
	extern int InitURLFliter(const char* const FileName);

	/**
	 * @brief URL检测
	 *
	 * 基于资源文件信息
	 * 对输入URL进行检测
	 * 严格匹配
	 *
	 * @warning 仅A记录可查询成功
	 *
	 * @param URLString URL字符串
	 * @param IP 查询结果指针，若为NULL则不被存储；存在多个查询结果时，返回下一个
	 * @return 若查询成功，返回值为1，否则为0
	 */
	extern int URLCheck(const int Type, const char* const URLString, void* const IP);

#ifdef __cplusplus
}
#endif

#endif // _URL_FLITER_H
