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

//待发送报文
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

//处理线程参数包
typedef struct parameterPack
{
    char *buf;
    int packSize;
    SOCKADDR_IN source;
    SOCKET sock;
} parameterPack;

//命令行参数处理
//（参数数量，指针）
void argRes(int, char *);

//初始化并连接套接字
//（套接字指针）
void initSocket(SOCKET *);

//读取DNS报文头
//返回：报文头
//（报文指针）
dnsHeader getHeader(char *);

//ID编码
//返回：编码后ID
//（源地址，报文指针）
unsigned short encodeID(SOCKADDR_IN, char *);

//制作本地返回报文
//返回：报文长度
//（报文指针，报文长度，ip数组指针）
int makePack(char *, int, char *);

//发送线程函数
//（SOCKET*）
DWORD WINAPI threadSend(LPVOID);

//报文解析处理函数
//（parameterPack*）
DWORD WINAPI dealPacket(LPVOID);
