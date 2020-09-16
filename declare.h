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

//�����ͱ���
typedef struct packet
{
    int type; //0���У�1ռ��
    int size;//���ĳ���
    char buf[512];//��������
    struct sockaddr_in dest;//Ŀ�ĵ�ַ
} packet;

//idӳ��Դ��Ϣ
typedef struct srcInfo
{
    int flag;//0���У�1ռ��
    char buf0, buf1;//ID�ֶ�
    SOCKADDR_IN procInfo;//Դ��ַ
    time_t start;//idӳ��ʱ��
} srcInfo;

//�����̲߳�����
typedef struct parameterPack
{
    char *buf;//��������
    int packSize;//���ĳ���
    SOCKADDR_IN source;//Դ��ַ
    SOCKET sock;//�����׽���
} parameterPack;

//�����в�������
//������������ָ�룩
void argRes(int, char *);

//��ʼ���������׽���
//���׽���ָ�룩
void initSocket(SOCKET *);

//��ȡDNS����ͷ
//���أ�����ͷ
//������ָ�룩
dnsHeader getHeader(char *);

//ID����
//���أ������ID
//��Դ��ַ������ָ�룩
unsigned short encodeID(SOCKADDR_IN, char *);

//�������ط��ر���
//���أ����ĳ���
//������ָ�룬���ĳ��ȣ�ip����ָ�룩
int makePack(char *, int, char *);

//�����̺߳���
//��SOCKET*��
DWORD WINAPI threadSend(LPVOID);

//���Ľ���������
//��parameterPack*��
DWORD WINAPI dealPacket(LPVOID);
