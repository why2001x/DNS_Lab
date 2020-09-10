#ifndef _DNSHEADER_H 
#define _DNSHEADER_H
#ifdef __cplusplus
extern "C" {
#endif
	struct header {
		unsigned short ID;
		unsigned char QR : 1;
		unsigned char Opcode : 4;
		unsigned char AA : 1;
		unsigned char TC : 1;
		unsigned char RD : 1;
		unsigned char RA : 1;
		unsigned char NOP : 3;
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