/**
 *  @brief     数据类型相关实现
 *  @author    王海昱
 *  @version   0.0.1
 *  @date      2020.07.28-2020.07.28
 */

#include "dnsdef.h"

int ipv4Comp(ipv4_t Lhs, ipv4_t Rhs)
{
	return (Lhs > Rhs) - (Lhs < Rhs);
}