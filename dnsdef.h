/**
 *  @file      dnsdef.h
 *  @brief     数据类型定义
 *  @author    王海昱
 *  @version   0.0.1
 *  @date      2020.07.26-2020.07.28
 */

#ifndef _DNSDEF_H
#define _DNSDEF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdafx.h"
#include "dnsheader.h"

#ifndef UCHAR
#define UCHAR uchar
    typedef unsigned char uchar;
#endif

    typedef struct in_addr ipv4_t;
    int ipv4Comp(ipv4_t Lhs, ipv4_t Rhs);

    typedef struct in6_addr ipv6_t;
    int ipv6Comp(ipv6_t Lhs, ipv6_t Rhs);

    /// DNS报文查询类型
    enum QueryType
    {
        A = 1,      ///< 由域名获得IPv4地址
        NS = 2,     ///< 查询域名服务器
        CNAME = 5,  ///< 查询规范名称
        SOA = 6,    ///< 开始授权
        WKS = 11,   ///< 熟知服务
        PTR = 12,   ///< 把IP地址转换成域名
        HINFO = 13, ///< 主机信息
        MX = 15,    ///< 邮件交换
        AAAA = 28,  ///< 由域名获得IPv6地址
        AXFR = 252, ///< 传送整个区的请求
        ANY = 255,  ///< 对所有记录的请求
    };

#ifdef __cplusplus
}
#endif

#endif // _DNSDEF_H
