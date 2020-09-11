#pragma once
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll

#include "stdafx.h"
#include "dnsdef.h"
#include "log_output.h"
#include "url_filter.h"
#include "getopt.h"
#include "dnsHeader.h"

#define DNSBUFMAX 512              //DNS单个报文长度
#define PORT 53                    //DNS默认端口
#define BUFMAX 512                 //缓冲区长度
#define IDMAX 65536                //Transaction ID容量
#define DEFAULTDNS "10.3.9.5"      //默认上游服务器
#define DEFAULTHOST "dnsrelay.txt" //默认host

//待发送信息
typedef struct packet
{
    int type; //0空闲，1占用
    int size;
    char buf[512];
    struct sockaddr_in dest;
} packet;
//id映射源信息
typedef struct srcInfo
{
    int flag;
    char buf0, buf1;
    struct sockaddr_in procInfo;
} srcInfo;
//接收线程参数包
typedef struct parameterPack
{
    char *buf;
    int packSize;
    SOCKADDR_IN source;
    SOCKET sock;
} parameterPack;

void argRes(int, char *);
void initSocket(SOCKET *);
dnsHeader getHeader(char *);
unsigned short encodeID(SOCKADDR_IN, char *);
int makePack(char *, int, char *);
DWORD WINAPI threadSend(LPVOID);
DWORD WINAPI dealPacket(LPVOID);
