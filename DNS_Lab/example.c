/**
 *  @brief     简要说明
 *  @details   详细说明（此处实现了一些与？？？相关的函数
 *  @author    主要作者
 *  @author    联合作者1 联合作者2 ...
 *  @version   x.y.z
 *  @date      2020.07.xx-2020.xx.xx
 *  @pre       前提条件（可省略）
 *  @bug       看那，我的老伙计！调用func时候非法内存访问，怕是指针飞了。（没有就省略）
 *  @warning   非正常使用可能损坏您的系统（可省略）
 *  @copyright GNU Public License.（可省略）
 */

#include "stdlib.h"
#include "stdio.h"

#include "example.h"

/// 给变量最好写点啥
/// 多行可以连着这样
/** 或者再长一点的
 */
int global; ///< 前面加个g_也许更好？
            ///< 再写点问题不大

///inline只在.c 里写，大段注释看 .h，此行可省略
inline int func(int a, int b) ///< 在这种位置写的话，大括号扔底下无脑格式化时这条不会跑
{
    /// 比较重要的写一下，变量名自己带点含义这行就省了
    int temp; ///< 一个函数里的变量命名至少要统一，文件/项目先不做要求？

    return a + b; ///< 一眼能看懂的就不写了，写的看的都难受
}