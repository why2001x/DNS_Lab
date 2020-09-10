#include "dnsdef.h"

int ipv4Comp(ipv4_t Lhs, ipv4_t Rhs)
{
	return (Lhs > Rhs) - (Lhs < Rhs);
}

int ipv6Comp(ipv6_t Lhs, ipv6_t Rhs)
{
	for (int i = 15; i >= 0; i--)
	{
		if (Lhs.u.Byte[i] != Rhs.u.Byte[i])
		{
			return (Lhs.u.Byte[i] > Rhs.u.Byte[i]) - (Lhs.u.Byte[i] < Rhs.u.Byte[i]);
		}
	}
	return 0;
}