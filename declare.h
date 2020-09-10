#pragma once

#include "stdafx.h"

//DNS默认53UDP
#define PORT 53
//缓冲区大小
#define BUFMAX 1024
//Transaction ID最大大小（16^4)
#define IDMAX 65536

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

void getServer();
int initWSA();
int createSocket(SOCKET*);
int bindSocketAddr(SOCKET);
unsigned short encodeID(SOCKADDR_IN, char*);
DWORD WINAPI threadSend(LPVOID);
void dealPacket(char*, int, SOCKADDR_IN, SOCKET);
int makePack(char*, int, char*);
