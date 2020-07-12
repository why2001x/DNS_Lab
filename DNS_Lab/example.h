/**
 *  @brief     简要说明
 *  @details   详细说明（此文件定义了？？？
 *  @author    主要作者
 *  @author    联合作者1 联合作者2 ...
 *  @version   x.y.z
 *  @date      2020.07.xx-2020.xx.xx
 *  @pre       前提条件（可省略）
 *  @bug       看那，我的老伙计！调用func时候非法内存访问，怕是指针飞了。（没有就省略）
 *  @warning   非正常使用可能损坏您的系统（可省略）
 *  @copyright GNU Public License.（可省略）
 */

#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif
    /**
	 * 一般通过结构体
	 * 详细一点的内容，再细的去里面写
	 */
	typedef struct example
	{
		/// 一个一行罢
		int a; ///< 说明一下
		int b; ///< 命名尽可能一致
		       ///< 一行太长就再来几行
	} example; ///< 也许typedef一下会更好？我不确定

	/// 单行的批注写这里
	extern int global; ///< 或者这里也可以（注意'<'）

	/**
	 * @brief 简要说明
	 *
	 * 当然写点详细的是最好
	 * 
	 * @param  a a是个啥
	 * @param  b b又是个啥
	 * @return   返回了啥啊
	 */
	extern int func(int a, int b);

#ifdef __cplusplus
}
#endif

#endif // _EXAMPLE_H
