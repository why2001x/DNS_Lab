#include "declare.h"
#include "log_output.h"
#include "url_filter.h"
#pragma comment(lib, "ws2_32.lib") //���� ws2_32.dll

//�������DNS��IP������û��ֶ����룩
char dnsServer[64];
srcInfo idMap[IDMAX];
packet pack[BUFMAX];
HANDLE packMutex = NULL;
unsigned int index;

int main(int argc, char* argv[])
{
	//TODO::�����в���
	SetLogLevel(LOG_INFO);
	InitLog(NULL);
	InitURLFilter("dnsrelay.txt");

	getServer(); //�����ⲿDNS
	SOCKET sock;
	initSocket(&sock);//��ʼ��socket

	//��ʼ��pack����Ϊ����״̬
	for (int i = 0; i < BUFMAX; i++)
		pack[i].type = 0;
	//��ʼ��idMap����Ϊ��ʹ�ø�id
	for (int i = 0; i < IDMAX; i++)
		idMap[i].flag = 0;
	//pack�߳���
	packMutex = CreateMutex(NULL, FALSE, NULL);
	//���������Listenning�̣߳�����������ܷ�Ϊ�����̴߳���
	CreateThread(NULL, 0, threadSend, &sock, 0, NULL);

	puts("Server is running.");

	//ѭ���ȴ��������ݰ�
	while (1)
	{
		parameterPack* tmp = (parameterPack*)malloc(sizeof(parameterPack));
		tmp->sock = sock;

		tmp->buf = (char*)malloc(sizeof(char) * DNSBUFMAX);
		memset(tmp->buf, 0, sizeof(char) * DNSBUFMAX);

		int len = sizeof(SOCKADDR_IN);
		//���հ����ݣ����뻺����
		tmp->packSize = recvfrom(sock, tmp->buf, sizeof(char) * DNSBUFMAX, 0, (SOCKADDR*)&tmp->source, &len);

		if (tmp->packSize <= 0)
			continue;

		//������ܵ��İ�������������ָ�롢sockaddr����
		CreateThread(NULL, 0, dealPacket, tmp, 0, NULL);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}

void getServer()
{
	//���û�ѡ��ָ����DNS���Σ�����ʹ��Ĭ��DNS
	printf("Choose your DNS server(input -1 to skip):\n");
	//�����û�DNS
	scanf_s("%s", dnsServer, sizeof(dnsServer));
	if (strcmp(dnsServer, "-1") == 0)
	{
		//Ĭ�����
		strcpy_s(dnsServer, sizeof(dnsServer), DEFAULTDNS);
	}
	else
	{
		unsigned long tmp;
		//������ı���IP��ַת��Ϊ�����������ֽ��򡱵�IP��ַ���ж�IP�Ƿ�Ϸ�
		//������IPv4���͡�����ı���IP��ַ
		int flag = inet_pton(AF_INET, dnsServer, &tmp);
		//�Ƿ�ʱ
		if (flag < 1)
		{
			printf("Error! Enable default server.\n");
			//Ӧ��Ĭ��DNS
			strcpy_s(dnsServer, sizeof(dnsServer), DEFAULTDNS);
		}
	}
	return;
}
//��ʼ��socket
void initSocket(SOCKET* sock)
{
	//��ʼ��WSA
	WSADATA WSA;
	if (WSAStartup(MAKEWORD(2, 2), &WSA) != 0)
	{
		printf("Failed to initialize Winsock!\n");
		exit(1);
	}
	
	//����UDP Socket
	SOCKET tmpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (tmpSock == INVALID_SOCKET)
	{
		printf("Creating socket failed!\n");
		WSACleanup();
		exit(1);
	}
	else
	{
		*sock = tmpSock;
	}

	//bind������ַ���˿�Ϊ0.0.0.0��53
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//תΪ�����ֽ���
	serverAddr.sin_port = htons(PORT);
	//������ַ����Ϊ0.0.0.0�����ֽ���ת��
	inet_pton(AF_INET, "0.0.0.0", &serverAddr.sin_addr.S_un.S_addr);
	if (bind(*sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Binding socket failed!\n");
		exit(1);
	}

	puts("Binding socket Success!");

	return;
}
//����IDת��
unsigned short encodeID(SOCKADDR_IN src, char* buf)
{
	for (int i = 1; i < IDMAX; ++i)
	{
		//�������õ�IDMapping
		if (idMap[i].flag == 1)
			continue;

		//�ҵ����õ�IDMapping
		idMap[i].flag = 1;
		//����Դ��ַ
		idMap[i].procInfo = src;
		//����ԭID
		idMap[i].buf0 = buf[0];
		idMap[i].buf1 = buf[1];
		//���ؿ��õ�IDӳ���±꣬���±꼴�����������η�����Transaction ID
		return i;
	}
	return 0;
}
//�ɰ������ذ���
int makePack(char* buf, int size, char* ip)
{
	//1000 0001:QR=1/RA=1
	buf[2] = 0x81;//response
	//�鵽��ip���ȫΪ0����block�õ�ַ
	if ((ip[3] | ip[2] | ip[1] | ip[0]) == 0)//block
		//RCode=3������NX_Domian����
		buf[3] = 0x83;
	else
		buf[3] = 0x80;

	//answer count
	buf[6] = 0x00;
	buf[7] = 0x01;

	// Name (offset=12)
	buf[size] = 0xC0;
	buf[size + 1] = 0x0C;

	// Type
	buf[size + 2] = 0x00;
	buf[size + 3] = 0x01;

	// Class
	buf[size + 4] = 0x00;
	buf[size + 5] = 0x01;

	// TTL = 64s
	buf[size + 6] = 0x00;
	buf[size + 7] = 0x00;
	buf[size + 8] = 0x01;
	buf[size + 9] = 0x00;

	// RDLENGTH:˵����Դ���ݵ��ֽ�����������1��TYPE A��¼����Դ������4�ֽڵ�IP��ַ
	buf[size + 10] = 0x00;
	buf[size + 11] = 0x04;

	// RDATA�����ز�ѯ��ip���
	buf[size + 12] = ip[0];
	buf[size + 13] = ip[1];
	buf[size + 14] = ip[2];
	buf[size + 15] = ip[3];

	return size + 16;
}
//�����̺߳�����������
DWORD WINAPI threadSend(LPVOID lpParamter)
{
	SOCKET* sock = (SOCKET*)lpParamter;
	int cur = 0;

	puts("Creaing thread success.");

	while (1)
	{
		// �Ƿ���ʵ����Ҫ�ȵ�100�ٷ�����û��Ҫ�İ�
		WaitForSingleObject(packMutex, INFINITE);
		for (int i = 0; i < BUFMAX; ++i)
		{
			if (!pack[(cur + i) % BUFMAX].type)
				continue;
			cur = (cur + i) % BUFMAX;
			pack[cur].type = 0;
			if (sendto(*sock, pack[cur].buf, pack[cur].size, 0, (SOCKADDR*)&pack[cur].dest, sizeof(SOCKADDR)) == -1)
			{
				printf("Sending error!\n");
				break;
			}
		}
		ReleaseMutex(packMutex);
	}
	return 0L;
}
//�����ģ�׼������
DWORD WINAPI dealPacket(LPVOID lpParamter)/*(char* buf, int packSize, SOCKADDR_IN source, SOCKET sock)*/
{
	//��ȡ������
	parameterPack* tmp = (parameterPack*)lpParamter;

	//idת��
	unsigned short outID = 0;

	//Ŀ���ַ
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;

	//buf[2]Ϊ��ͷ�������ֽ�
	//0x80;1000 0000����ͷ����and,��ȡ�����λ�����λΪQR������0��1��Ϊ��ѯ����Ӧ
	if ((tmp->buf[2] & 0x80) == 0) // ����ǲ�ѯ���ĵĻ�
	{		
		//ȡ������
		char name[DNSBUFMAX] = { 0 };
		int type = 1;
		//i=12:���ĵ�QNAME�ֶ�
		for (int i = 12; i < tmp->packSize;)
		{
			if (tmp->buf[i] == 0)
			{
				//ȡ����ѯ����
				type = (int)(unsigned char)tmp->buf[i + 1] + (int)(unsigned char)tmp->buf[i + 2];
				break;
			}
			int cnt = (int)tmp->buf[i] + i + 1;
			for (i++; i < cnt; i++)
				//strlen:λ��Ϊ�׸�ֵΪ0���������ַ������ݸ��¶�����
				name[strlen(name)] = tmp->buf[i];
			if (tmp->buf[i] != 0)
				name[strlen(name)] = '.';
		}

		lprintf(LOG_INFO,"%8d: Type %02d, Name: %s\n",++index,type,name);

		char ipBuf[4] = "";
		//URLCheck����ѯ���ͣ�enum����url�ַ�������ѯ�����������ip����
		//buf[4]==0&buf[5]==1:QDCOUNT=1,��ֻ��һ����ѯ��¼ʱ
		int result = URLCheck(A, name, ipBuf);
		if (result && tmp->buf[4] == 0 && tmp->buf[5] == 1) // ���ڱ����ļ���
		{
			//ֱ�ӷ����Թ������ذ�
			tmp->packSize = makePack(tmp->buf, tmp->packSize, ipBuf);

			//�ȴ��߳���
			//WaitForSingleObject(packMutex, INFINITE);
			//����������������
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				//�ҵ��˻������п��п��õİ��ṹ��
				if (pack[i].type == 0)
				{
					//��Ϊ�ð����ڲ�ѯ���ȴ������̱߳���������
					pack[i].type = 1;

					//�ɰ�
					pack[i].size = tmp->packSize;
					pack[i].dest = tmp->source;
					for (int j = 0; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			//�ͷ���
			ReleaseMutex(packMutex);
		}
		else // �����ڱ����ļ��У���ת������
		{
			//��Ŀ���ַΪ���η�����
			inet_pton(AF_INET, dnsServer, &dest.sin_addr.S_un.S_addr);
			dest.sin_port = htons(53);

			//outIDĬ��Ϊ�㣬��Ҫ���ҿ��õĿ�ת��ID�ٷ���
			outID = encodeID(tmp->source, tmp->buf);
			while (!outID)
			{
				//�õ�һ�����õ�ת��id
				Sleep(50);
				outID = encodeID(tmp->source, tmp->buf);
			}


			//����ͬ�ϲ���׼�����ͻ���
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				if (pack[i].type == 0)
				{
					pack[i].type = 1;
					pack[i].size = tmp->packSize;
					pack[i].dest = dest;

					//׼��Transaction ID
					pack[i].buf[0] = (char)(outID >> 8);
					pack[i].buf[1] = (char)(outID & 0xff);
					//TID֮�󽫰�����
					for (int j = 2; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			ReleaseMutex(packMutex);
		}
	}
	else //��Ӧ����
	{
		//ȡ��ID,unsigned char���޸�
		outID = (((unsigned short)(unsigned char)tmp->buf[0]) << 8) + (unsigned short)(unsigned char)tmp->buf[1];
		//�ɷ���ID����֮ǰ��ID
		if (idMap[outID].flag == 0)
		{
			printf("ID Mapping failed!\n");
		}
		else
		{
			//�ͷ�idӳ��ռ��
			idMap[outID].flag = 0;

			//����ͬ�ϲ���׼�����ͻ���
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				if (pack[i].type == 0)
				{
					pack[i].type = 1;
					pack[i].size = tmp->packSize;
					pack[i].dest = idMap[outID].procInfo;

					pack[i].buf[0] = idMap[outID].buf0;
					pack[i].buf[1] = idMap[outID].buf1;
					for (int j = 2; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			ReleaseMutex(packMutex);
		}
	}

	//�ͷŶ�̬�����ڴ�
	free(tmp->buf);
	free(tmp);

	return 0L;
}
