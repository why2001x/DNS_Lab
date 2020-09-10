#pragma once

#include "stdafx.h"

//DNS报文长度
#define DNSBUFMAX 512
//DNS默认53UDP
#define PORT 53
//缓冲区大小
#define BUFMAX 1024
//Transaction ID最大大小（16^4)
#define IDMAX 65536
//默认DNS上游
#define DEFAULTDNS  "10.3.9.5"

//包结构体，用于之后开启数组，方便进行包的管理
typedef struct packet
{
    int type; //0空闲，1占用
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

typedef struct parameterPack
{
    char* buf;
    int packSize;
    SOCKADDR_IN source;
    SOCKET sock;
}parameterPack;

void getServer();
int initWSA();
int createSocket(SOCKET*);
int bindSocketAddr(SOCKET);
unsigned short encodeID(SOCKADDR_IN, char*);
DWORD WINAPI threadSend(LPVOID);
DWORD WINAPI dealPacket(LPVOID);
//void dealPacket(char*, int, SOCKADDR_IN, SOCKET);
int makePack(char*, int, char*);
