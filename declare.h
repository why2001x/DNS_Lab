#pragma once
#pragma comment(lib, "ws2_32.lib") //���� ws2_32.dll

#include "stdafx.h"
#include "dnsdef.h"
#include "log_output.h"
#include "url_filter.h"
#include "getopt.h"
#include "dnsHeader.h"

#define DNSBUFMAX 512              //DNS�������ĳ���
#define PORT 53                    //DNSĬ�϶˿�
#define BUFMAX 512                 //����������
#define IDMAX 65536                //Transaction ID����
#define DEFAULTDNS "10.3.9.5"      //Ĭ�����η�����
#define DEFAULTHOST "dnsrelay.txt" //Ĭ��host

//��������Ϣ
typedef struct packet
{
    int type; //0���У�1ռ��
    int size;
    char buf[512];
    struct sockaddr_in dest;
} packet;
//idӳ��Դ��Ϣ
typedef struct srcInfo
{
    int flag;
    char buf0, buf1;
    struct sockaddr_in procInfo;
} srcInfo;
//�����̲߳�����
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
