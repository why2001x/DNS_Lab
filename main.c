#include "declare.h"

char host[128] = DEFAULTHOST;	 //host文件地址
char dnsServer[64] = DEFAULTDNS; //上游服务器
srcInfo idMap[IDMAX];			 //ID映射表
packet pack[BUFMAX];			 //发送缓冲区
HANDLE packMutex = NULL;		 //发送缓冲互斥锁
HANDLE logMutex = NULL;			 //Log互斥锁
HANDLE idMutex = NULL;			 //ID互斥锁
unsigned int index = 0;			 //报文序号

int main(int argc, char *argv[])
{
	//获取命令行参数，设定调试输出等级
	argRes(argc, argv);

	//初始化host文件
	InitURLFilter(host);

	//初始化socket
	SOCKET sock;
	initSocket(&sock);

	//初始化pack数组为空闲状态
	for (int i = 0; i < BUFMAX; i++)
		pack[i].type = 0;

	//初始化idMap数组为空闲状态
	for (int i = 0; i < IDMAX; i++)
		idMap[i].flag = 0;

	//创建互斥锁
	packMutex = CreateMutex(NULL, FALSE, NULL);
	logMutex = CreateMutex(NULL, FALSE, NULL);
	idMutex = CreateMutex(NULL, FALSE, NULL);

	//将发送与接收线程分离
	CreateThread(NULL, 0, threadSend, &sock, 0, NULL);

	printf("server is running.\n");

	//循环等待接受UDP数据
	while (1)
	{
		//打包接收线程所需参数
		parameterPack *tmp = (parameterPack *)malloc(sizeof(parameterPack));
		tmp->sock = sock;

		//动态申请缓冲区空间
		tmp->buf = (char *)malloc(sizeof(char) * DNSBUFMAX);
		memset(tmp->buf, 0, sizeof(char) * DNSBUFMAX);

		int len = sizeof(SOCKADDR_IN);
		//接收包内容，存入缓冲区
		tmp->packSize = recvfrom(sock, tmp->buf, sizeof(char) * DNSBUFMAX, 0, (SOCKADDR *)&tmp->source, &len);

		//异常跳过
		if (tmp->packSize <= 0)
			continue;

		//处理接受到的包，并将存入发送缓冲区
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
			//普通log
		case 'd':
			SetLogLevel(LOG_INFO);
			printf("LOG LEVEL：INFO\n");
			break;
		case 'D':
			//详细log
			SetLogLevel(LOG_DEBUG);
			printf("LOGO LEVEL：DEBUG\n");
			break;
		case 's':
			//指定上游服务器
			memcpy(dnsServer, optarg, strlen(optarg));
			printf("UPSTREAM SERVER：%s\n", optarg);
			break;
		case 'f':
			//指定host
			printf("HOST FILE DIR：%s\n", optarg);
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
	//初始化WSA
	WSADATA WSA;
	if (WSAStartup(MAKEWORD(2, 2), &WSA) != 0)
	{
		printf("Failed to initialize Winsock!\n");
		exit(1);
	}

	//开启UDP Socket
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

	//bind监听地址、端口为0.0.0.0、53
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;

	//转为网络字节序
	serverAddr.sin_port = htons(PORT);

	//监听地址，绑定为0.0.0.0，并字节序转换
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
	//等待互斥锁，防止映射同时发生造成ID复用
	WaitForSingleObject(idMutex, INFINITE);

	for (int i = 1; i < IDMAX; ++i)
	{
		//遍历可用的ID
		if (idMap[i].flag == 1)
			continue;

		//找到可用的IDMapping
		idMap[i].flag = 1;

		//存下源地址
		idMap[i].procInfo = src;

		//存下原ID
		idMap[i].buf0 = buf[0];
		idMap[i].buf1 = buf[1];

		//返回可用的ID映射下标，该下标即是用于向上游发出的Transaction ID
		return i;
	}

	ReleaseMutex(idMutex);

	return 0;
}

int makePack(char *buf, int size, char *ip)
{
	//1000 0001:QR=1/RA=1，response
	buf[2] = 0x81;

	//查到的ip结果全为0：则block该地址
	if ((ip[3] | ip[2] | ip[1] | ip[0]) == 0)
		buf[3] = 0x83; //RCode=3，返回NX_Domian错误
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

	// RDLENGTH:说明资源数据的字节数，对类型1（TYPE A记录）资源数据是4字节的IP地址
	buf[size + 10] = 0x00;
	buf[size + 11] = 0x04;

	// RDATA：返回查询的ip结果
	buf[size + 12] = ip[0];
	buf[size + 13] = ip[1];
	buf[size + 14] = ip[2];
	buf[size + 15] = ip[3];

	return size + 16;
}

DWORD WINAPI threadSend(LPVOID lpParamter)
{
	SOCKET *sock = (SOCKET *)lpParamter;
	//发送游标
	int cur = 0;

	printf("Creaing thread success.\n");

	while (1)
	{
		//等待缓冲互斥锁，防止并发写入与读取
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

		//每次查询完略微延时，防止长时间占用
		Sleep(1);
	}
	return 0L;
}

DWORD WINAPI dealPacket(LPVOID lpParamter) /*(char* buf, int packSize, SOCKADDR_IN source, SOCKET sock)*/
{
	//获取参数包
	parameterPack *tmp = (parameterPack *)lpParamter;

	//获取DNS头信息
	dnsHeader headInfo = getHeader(tmp->buf);

	//id转换
	unsigned short outID = 0;

	//目标地址
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;

	if (headInfo.QR == 0) // 查询报文
	{
		//取出域名
		char name[DNSBUFMAX] = {0};
		int type = 1;
		for (int i = 12; i < tmp->packSize;)
		{
			if (tmp->buf[i] == 0)
			{
				//取出查询类型
				type = (((int)(unsigned char)tmp->buf[i + 1]) << 8) + (int)(unsigned char)tmp->buf[i + 2];
				break;
			}
			int cnt = (int)tmp->buf[i] + i + 1;
			//利用strlen访问字符串末尾，逐字符添加域名
			for (i++; i < cnt; i++)
				name[strlen(name)] = tmp->buf[i];
			//非0数字则添加.
			if (tmp->buf[i] != 0)
				name[strlen(name)] = '.';
		}

		//host文件查询所得域名
		char ipBuf[4] = "";
		//URLCheck：查询类型（enum）、url字符串、查询结果（二进制ip？）
		int result = URLCheck(A, name, ipBuf);
		//buf[4]==0&buf[5]==1:QDCOUNT=1,即只有一条查询记录时
		//需要本地回复的内容包括包内的屏蔽域名以及非屏蔽ipv4查询，对非屏蔽ipv6查询应进行转发

		if (result && (type == 1 || ipBuf[0] == 0) && tmp->buf[4] == 0 && tmp->buf[5] == 1) //需要本地回复
		{
			//直接发回自构建返回包
			tmp->packSize = makePack(tmp->buf, tmp->packSize, ipBuf);
			dest = tmp->source;

			//等待线程锁，获得锁后遍历缓冲区
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				//找到了缓冲区中空闲可用的包结构体
				if (pack[i].type == 0)
				{
					//置占用状态
					pack[i].type = 1;
					//记录报文长度
					pack[i].size = tmp->packSize;
					//记录目的地址
					pack[i].dest = dest;
					//拷贝报文
					for (int j = 0; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			//结束缓冲区操作，释放缓冲互斥锁
			ReleaseMutex(packMutex);
		}
		else //需要转发
		{
			//修改目标地址为上游服务器
			inet_pton(AF_INET, dnsServer, &dest.sin_addr.S_un.S_addr);
			dest.sin_port = htons(53);

			//outID默认为零，需要查找可用的可转换ID再发出,互斥锁在编码函数内部实现
			outID = encodeID(tmp->source, tmp->buf);
			while (!outID)
			{
				//查询失败，所有ID被占用，则休眠50ms，重新尝试转换
				Sleep(50);
				outID = encodeID(tmp->source, tmp->buf);
			}

			//大致同上步，准备发送缓冲
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				if (pack[i].type == 0)
				{
					pack[i].type = 1;
					pack[i].size = tmp->packSize;
					pack[i].dest = dest;

					//准备Transaction ID
					pack[i].buf[0] = (char)(outID >> 8);
					pack[i].buf[1] = (char)(outID & 0xff);
					//TID之后将包塞入
					for (int j = 2; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			ReleaseMutex(packMutex);
		}

		//一级Log输出查询信息
		//二级Log输出每个收到报文的DNS头，以及ID映射，源和目标地址
		//输出Log需要请求Log互斥锁，防止多线程同时输出，打乱顺序
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
	else //响应报文
	{
		//取回ID
		outID = headInfo.ID;
		//由返回ID反查之前的ID
		WaitForSingleObject(idMutex, INFINITE);
		int result = idMap[outID].flag;
		if (result == 0)
		{
			ReleaseMutex(idMutex);
		}
		else
		{
			//释放id映射占用
			idMap[outID].flag = 0;
			ReleaseMutex(idMutex);

			//大致同上步，准备发送缓冲
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

			//应答包仅输出二级Log内容
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

	//释放动态申请内存
	free(tmp->buf);
	free(tmp);

	return 0L;
}
