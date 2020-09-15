#include "dnsdef.h"
#ifdef _MSC_VER
int ipv4Comp(ipv4_t Lhs, ipv4_t Rhs)
{
    if (Lhs.S_un.S_un_b.s_b1 != Rhs.S_un.S_un_b.s_b1)
    {
        return (Lhs.S_un.S_un_b.s_b1 > Rhs.S_un.S_un_b.s_b1) -
            (Lhs.S_un.S_un_b.s_b1 < Rhs.S_un.S_un_b.s_b1);
    }
    else if (Lhs.S_un.S_un_b.s_b2 != Rhs.S_un.S_un_b.s_b2)
    {
        return (Lhs.S_un.S_un_b.s_b2 > Rhs.S_un.S_un_b.s_b2) -
            (Lhs.S_un.S_un_b.s_b2 < Rhs.S_un.S_un_b.s_b2);
    }
    else if (Lhs.S_un.S_un_b.s_b3 != Rhs.S_un.S_un_b.s_b3)
    {
        return (Lhs.S_un.S_un_b.s_b3 > Rhs.S_un.S_un_b.s_b3) -
            (Lhs.S_un.S_un_b.s_b3 < Rhs.S_un.S_un_b.s_b3);
    }
    else if (Lhs.S_un.S_un_b.s_b4 != Rhs.S_un.S_un_b.s_b4)
    {
        return (Lhs.S_un.S_un_b.s_b4 > Rhs.S_un.S_un_b.s_b4) -
            (Lhs.S_un.S_un_b.s_b4 < Rhs.S_un.S_un_b.s_b4);
    }
    return 0;
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
#endif // _MSC_VER