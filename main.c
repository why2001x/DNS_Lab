#include "declare.h"

char host[128] = DEFAULTHOST;	 //host�ļ���ַ
char dnsServer[64] = DEFAULTDNS; //���η�����
srcInfo idMap[IDMAX];			 //IDӳ���
packet pack[BUFMAX];			 //���ͻ�����
HANDLE packMutex = NULL;		 //���ͻ��廥����
HANDLE logMutex = NULL;			 //Log������
HANDLE idMutex = NULL;			 //ID������
unsigned int index = 0;			 //�������

int main(int argc, char *argv[])
{
	//��ȡ�����в������趨��������ȼ�
	argRes(argc, argv);

	//��ʼ��host�ļ�
	InitURLFilter(host);

	//��ʼ��socket
	SOCKET sock;
	initSocket(&sock);

	//��ʼ��pack����Ϊ����״̬
	for (int i = 0; i < BUFMAX; i++)
		pack[i].type = 0;

	//��ʼ��idMap����Ϊ����״̬
	for (int i = 0; i < IDMAX; i++)
		idMap[i].flag = 0;

	//����������
	packMutex = CreateMutex(NULL, FALSE, NULL);
	logMutex = CreateMutex(NULL, FALSE, NULL);
	idMutex = CreateMutex(NULL, FALSE, NULL);

	//������������̷߳���
	CreateThread(NULL, 0, threadSend, &sock, 0, NULL);

	printf("server is running.\n");

	//ѭ���ȴ�����UDP����
	while (1)
	{
		//��������߳��������
		parameterPack *tmp = (parameterPack *)malloc(sizeof(parameterPack));
		tmp->sock = sock;

		//��̬���뻺�����ռ�
		tmp->buf = (char *)malloc(sizeof(char) * DNSBUFMAX);
		memset(tmp->buf, 0, sizeof(char) * DNSBUFMAX);

		int len = sizeof(SOCKADDR_IN);
		//���հ����ݣ����뻺����
		tmp->packSize = recvfrom(sock, tmp->buf, sizeof(char) * DNSBUFMAX, 0, (SOCKADDR *)&tmp->source, &len);

		//�쳣����
		if (tmp->packSize <= 0)
			continue;

		//������ܵ��İ����������뷢�ͻ�����
		CreateThread(NULL, 0, dealPacket, tmp, 0, NULL);
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}

void argRes(int argc, char *argv[])
{
	int ch;
	while ((ch = getopt(argc, argv, "dDs:f:")) != -1)
	{
		switch (ch)
		{
			//��ͨlog
		case 'd':
			SetLogLevel(LOG_INFO);
			printf("LOG LEVEL��INFO\n");
			break;
		case 'D':
			//��ϸlog
			SetLogLevel(LOG_DEBUG);
			printf("LOGO LEVEL��DEBUG\n");
			break;
		case 's':
			//ָ�����η�����
			memcpy(dnsServer, optarg, strlen(optarg));
			printf("UPSTREAM SERVER��%s\n", optarg);
			break;
		case 'f':
			//ָ��host
			printf("HOST FILE DIR��%s\n", optarg);
			InitURLFilter(optarg);
			break;
		default:
			printf("Illegal Parameters.\n");
		case 'h':
			printf("-h :Help\n");
			printf("-d :Brief Log\n");
			printf("-D :Verbose Log\n");
			printf("-s :DNS Upstream Server IP\n");
			printf("-f :Host File Dir\n");
			break;
		}
	}
}

void initSocket(SOCKET *sock)
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
	if (bind(*sock, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Binding socket failed!\n");
		exit(1);
	}

	puts("Binding socket Success!");

	return;
}

dnsHeader getHeader(char *buf)
{
	dnsHeader ret;
	ret.ID = (((unsigned short)(unsigned char)buf[0]) << 8) + (unsigned short)(unsigned char)buf[1];
	ret.QR = buf[2] & 0x80;
	ret.Opcode = buf[2] & 0x78;
	ret.AA = buf[2] & 0x04;
	ret.TC = buf[2] & 0x02;
	ret.RD = buf[2] & 0x01;
	ret.RA = buf[3] & 0x80;
	ret.RCODE = buf[3] & 0x08;
	ret.QDCOUNT = (((unsigned short)(unsigned char)buf[4]) >> 8) + (unsigned short)(unsigned char)buf[5];
	ret.ANCOUNT = (((unsigned short)(unsigned char)buf[6]) >> 8) + (unsigned short)(unsigned char)buf[7];
	ret.NSCOUNT = (((unsigned short)(unsigned char)buf[8]) >> 8) + (unsigned short)(unsigned char)buf[9];
	ret.ARCOUNT = (((unsigned short)(unsigned char)buf[10]) >> 8) + (unsigned short)(unsigned char)buf[11];
	return ret;
}

unsigned short encodeID(SOCKADDR_IN src, char *buf)
{
	//�ȴ�����������ֹӳ��ͬʱ�������ID����
	WaitForSingleObject(idMutex, INFINITE);

	for (int i = 1; i < IDMAX; ++i)
	{
		//�������õ�ID
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

	ReleaseMutex(idMutex);

	return 0;
}

int makePack(char *buf, int size, char *ip)
{
	//1000 0001:QR=1/RA=1��response
	buf[2] = 0x81;

	//�鵽��ip���ȫΪ0����block�õ�ַ
	if ((ip[3] | ip[2] | ip[1] | ip[0]) == 0)
		buf[3] = 0x83; //RCode=3������NX_Domian����
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

DWORD WINAPI threadSend(LPVOID lpParamter)
{
	SOCKET *sock = (SOCKET *)lpParamter;
	//�����α�
	int cur = 0;

	printf("Creaing thread success.\n");

	while (1)
	{
		//�ȴ����廥��������ֹ����д�����ȡ
		WaitForSingleObject(packMutex, INFINITE);
		for (int i = 0; i < BUFMAX; ++i)
		{
			if (!pack[(cur + i) % BUFMAX].type)
				continue;
			cur = (cur + i) % BUFMAX;
			pack[cur].type = 0;
			if (sendto(*sock, pack[cur].buf, pack[cur].size, 0, (SOCKADDR *)&pack[cur].dest, sizeof(SOCKADDR)) == -1)
			{
				printf("Sending error!\n");
				break;
			}
		}
		ReleaseMutex(packMutex);

		//ÿ�β�ѯ����΢��ʱ����ֹ��ʱ��ռ��
		Sleep(1);
	}
	return 0L;
}

DWORD WINAPI dealPacket(LPVOID lpParamter) /*(char* buf, int packSize, SOCKADDR_IN source, SOCKET sock)*/
{
	//��ȡ������
	parameterPack *tmp = (parameterPack *)lpParamter;

	//��ȡDNSͷ��Ϣ
	dnsHeader headInfo = getHeader(tmp->buf);

	//idת��
	unsigned short outID = 0;

	//Ŀ���ַ
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;

	if (headInfo.QR == 0) // ��ѯ����
	{
		//ȡ������
		char name[DNSBUFMAX] = {0};
		int type = 1;
		for (int i = 12; i < tmp->packSize;)
		{
			if (tmp->buf[i] == 0)
			{
				//ȡ����ѯ����
				type = (((int)(unsigned char)tmp->buf[i + 1]) << 8) + (int)(unsigned char)tmp->buf[i + 2];
				break;
			}
			int cnt = (int)tmp->buf[i] + i + 1;
			//����strlen�����ַ���ĩβ�����ַ��������
			for (i++; i < cnt; i++)
				name[strlen(name)] = tmp->buf[i];
			//��0���������.
			if (tmp->buf[i] != 0)
				name[strlen(name)] = '.';
		}

		//host�ļ���ѯ��������
		char ipBuf[4] = "";
		//URLCheck����ѯ���ͣ�enum����url�ַ�������ѯ�����������ip����
		int result = URLCheck(A, name, ipBuf);
		//buf[4]==0&buf[5]==1:QDCOUNT=1,��ֻ��һ����ѯ��¼ʱ
		//��Ҫ���ػظ������ݰ������ڵ����������Լ�������ipv4��ѯ���Է�����ipv6��ѯӦ����ת��

		if (result && (type == 1 || ipBuf[0] == 0) && tmp->buf[4] == 0 && tmp->buf[5] == 1) //��Ҫ���ػظ�
		{
			//ֱ�ӷ����Թ������ذ�
			tmp->packSize = makePack(tmp->buf, tmp->packSize, ipBuf);
			dest = tmp->source;

			//�ȴ��߳���������������������
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				//�ҵ��˻������п��п��õİ��ṹ��
				if (pack[i].type == 0)
				{
					//��ռ��״̬
					pack[i].type = 1;
					//��¼���ĳ���
					pack[i].size = tmp->packSize;
					//��¼Ŀ�ĵ�ַ
					pack[i].dest = dest;
					//��������
					for (int j = 0; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			//�����������������ͷŻ��廥����
			ReleaseMutex(packMutex);
		}
		else //��Ҫת��
		{
			//�޸�Ŀ���ַΪ���η�����
			inet_pton(AF_INET, dnsServer, &dest.sin_addr.S_un.S_addr);
			dest.sin_port = htons(53);

			//outIDĬ��Ϊ�㣬��Ҫ���ҿ��õĿ�ת��ID�ٷ���,�������ڱ��뺯���ڲ�ʵ��
			outID = encodeID(tmp->source, tmp->buf);
			while (!outID)
			{
				//��ѯʧ�ܣ�����ID��ռ�ã�������50ms�����³���ת��
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

		//һ��Log�����ѯ��Ϣ
		//����Log���ÿ���յ����ĵ�DNSͷ���Լ�IDӳ�䣬Դ��Ŀ���ַ
		//���Log��Ҫ����Log����������ֹ���߳�ͬʱ���������˳��
		char srcIP[64] = {0};
		char destIP[64] = {0};
		inet_ntop(AF_INET, &tmp->source.sin_addr.S_un.S_addr, srcIP, sizeof(char) * 63);
		inet_ntop(AF_INET, &dest.sin_addr.S_un.S_addr, destIP, sizeof(char) * 63);
		WaitForSingleObject(logMutex, INFINITE);
		lprintf(LOG_INFO, "%8d: Type %02d, Name: %s\n", ++index, type, name);
		lprintf(LOG_DEBUG, "         ID %04x QR %d Opcode %x AA %d TC %d RD %d RA %d RCODE %x\n"
						   "                                    QDCOUNT %d ANCOUNT %d NSCOUNT %d ARCOUNT %d\n"
						   "                                    ID[%5d->%5d] Source %s:%d Destination %s:%d\n",
				headInfo.ID, headInfo.QR, headInfo.Opcode, headInfo.AA,
				headInfo.TC, headInfo.RD, headInfo.RA, headInfo.RCODE,
				headInfo.QDCOUNT, headInfo.ANCOUNT, headInfo.NSCOUNT, headInfo.ARCOUNT,
				headInfo.ID, outID ? outID : headInfo.ID,
				srcIP, ntohs(tmp->source.sin_port),
				destIP, ntohs(dest.sin_port));
		ReleaseMutex(logMutex);
	}
	else //��Ӧ����
	{
		//ȡ��ID
		outID = headInfo.ID;
		//�ɷ���ID����֮ǰ��ID
		WaitForSingleObject(idMutex, INFINITE);
		int result = idMap[outID].flag;
		if (result == 0)
		{
			ReleaseMutex(idMutex);
		}
		else
		{
			//�ͷ�idӳ��ռ��
			idMap[outID].flag = 0;
			ReleaseMutex(idMutex);

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

			//Ӧ������������Log����
			char srcIP[64] = {0};
			char destIP[64] = {0};
			inet_ntop(AF_INET, &tmp->source.sin_addr.S_un.S_addr, srcIP, sizeof(char) * 63);
			inet_ntop(AF_INET, &idMap[outID].procInfo.sin_addr.S_un.S_addr, destIP, sizeof(char) * 63);
			WaitForSingleObject(logMutex, INFINITE);
			lprintf(LOG_DEBUG, "%7d: ID %04x QR %d Opcode %x AA %d TC %d RD %d RA %d RCODE %x\n"
							   "                                    QDCOUNT %d ANCOUNT %d NSCOUNT %d ARCOUNT %d\n"
							   "                                    ID[%5d->%5d] Source %s:%d Destination %s:%d\n",
					++index, headInfo.ID, headInfo.QR, headInfo.Opcode, headInfo.AA,
					headInfo.TC, headInfo.RD, headInfo.RA, headInfo.RCODE,
					headInfo.QDCOUNT, headInfo.ANCOUNT, headInfo.NSCOUNT, headInfo.ARCOUNT,
					headInfo.ID, (((unsigned short)(unsigned char)idMap[outID].buf0) << 8) + (unsigned short)(unsigned char)idMap[outID].buf1,
					srcIP, ntohs(tmp->source.sin_port),
					destIP, ntohs(idMap[outID].procInfo.sin_port));
			ReleaseMutex(logMutex);
		}
	}

	//�ͷŶ�̬�����ڴ�
	free(tmp->buf);
	free(tmp);

	return 0L;
}
