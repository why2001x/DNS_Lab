#include "getopt.h"


struct header {
	unsigned short ID;
	unsigned char QR : 1;
	unsigned char Opcode : 4;
	unsigned char AA : 1;
	unsigned char TC : 1;
	unsigned char RD : 1;
	unsigned char RA : 1;
	unsigned char Z : 4;
	unsigned char RCODE : 4;
	unsigned short QDCOUNT;
	unsigned short ANCOUNT;
	unsigned short NSCOUNT;
	unsigned short ARCOUNT;
};

int main(int argc, char *argv[])
{
	return 0;
}