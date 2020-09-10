#pragma once

#include "stdafx.h"

//DNS���ĳ���
#define DNSBUFMAX 512
//DNSĬ��53UDP
#define PORT 53
//��������С
#define BUFMAX 1024
//Transaction ID����С��16^4)
#define IDMAX 65536
//Ĭ��DNS����
#define DEFAULTDNS  "10.3.9.5"

//���ṹ�壬����֮�������飬������а��Ĺ���
typedef struct packet
{
    int type; //0���У�1ռ��
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
