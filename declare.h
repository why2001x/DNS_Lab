#pragma once

#include "stdafx.h"

//DNS默认53UDP
#define PORT 53
//缓冲区大小
#define BUFMAX 1024
//Transaction ID最大大小（16^4)
#define IDMAX 65536

//dns头结构
typedef struct dnsHeader
{
    unsigned short TranID;
    unsigned short Flags;
    unsigned short QueryCount;
    unsigned short AnswerCount;
    unsigned short AuthoriryCount;
    unsigned short AdditionalCount;
} dnsHeader;

//包结构体，用于之后开启数组，方便进行包的管理
typedef struct packet
{
    int type; //0空闲，----1查询，2响应
    int size;
    char buf[512];
    struct sockaddr_in dest;
} packet;

typedef struct srcInfo
{
    int flag;
    char buf0, buf1;
    struct sockaddr_in procInfo;
} srcInfo;

void getServer();
int initWSA();
int createSocket(SOCKET*);
int bindSocketAddr(SOCKET);
dnsHeader dealDNSHeader(char*);
unsigned short encodeID(SOCKADDR_IN, char*);
DWORD WINAPI threadSend(LPVOID);
void dealPacket(char*, int, SOCKADDR_IN, SOCKET);
int makePack(char*, int, char*);
//
//void send_out(char* buff, int buff_size, struct sockaddr_in recv_from, SOCKET my_socket, int thread_id);
//void encodelocaldns(char* buff, int packet_size, struct sockaddr_in recv_from, string ip_addr, DnsHeader Dnshdr, SOCKET my_socket, int thread_id);
//DnsHeader HandleDnsHeader(char* buff);
//void handle_packet(char* buff, int packet_size, struct sockaddr_in recv_from, map<string, string> dnsmap, SOCKET my_socket, int thread_id);
//map<string, string> load_file();
//void DNSHandleThread(map<string, string>dnsmap, SOCKET my_socket, int thread_id);
//packet Getpack();