/**
 *  @brief     DNS报文头
 *  @details   此
 *  @author    方淇
 *  @version   0.1
 *  @date      2020.07.11-2020.07.12
 */

#ifndef _DNSHEADER_H 
#define _DNSHEADER_H

#ifdef __cplusplus
extern "C" {
#endif

	//ID
	//由客户程序设置并由服务器返回结果。客户程序通过它来确定响应与查询是否匹配
	//QR：0表示查询报，1表示响应报。
	//OPCODE
	//通常值为0（标准查询），其他值为1（反向查询）和2（服务器状态请求）。
	//AA : 权威答案(Authoritative answer)
	//TC : 截断的(Truncated)
	//应答的总长度超512字节时，只返回前512个字节
	//RD : 期望递归(Recursion desired)
	//查询报中设置，响应报中返回
	//告诉名字服务器处理递归查询。如果该位为0，且被请求的名字服务器没有一个权威回答，就返回一个能解答该查询的其他名字服务器列表，这称为迭代查询
	//RA：递归可用(Recursion Available)
	//如果名字服务器支持递归查询，则在响应中该比特置为1
	//Z：必须为0，保留字段
	//RCODE : 响应码(Response coded)，仅用于响应报
	//值为0(没有差错)
	//值为3表示名字差错。从权威名字服务器返回，表示在查询中指定域名不存在
	//QDCOUNT
	//Number of entries in the question section
	//ANCOUNT
	//Number of RRs in the answer section
	//NSCOUNT
	//Number of name server RRs in authority records section
	//ARCOUNT
	//Number of RRs in additional records section



	struct header {
		unsigned short ID;
		unsigned char QR : 1;
		unsigned char Opcode : 4;
		unsigned char AA : 1;
		unsigned char TC : 1;
		unsigned char RD : 1;
		unsigned char RA : 1;
		unsigned char Z : 3;
		unsigned char RCODE : 4;
		unsigned short QDCOUNT;
		unsigned short ANCOUNT;
		unsigned short NSCOUNT;
		unsigned short ARCOUNT;
	};

#ifdef __cplusplus
}
#endif

#endif // _DNSHEADER_H