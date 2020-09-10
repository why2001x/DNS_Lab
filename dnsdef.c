#include "dnsdef.h"

int ipv4Comp(ipv4_t Lhs, ipv4_t Rhs)
{
	return (Lhs > Rhs) - (Lhs < Rhs);
}