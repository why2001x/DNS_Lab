#include "declare.h"
#include "log_output.h"
#include "url_filter.h"
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll

//存放上游DNS的IP（如果用户手动输入）
char dnsServer[64];
srcInfo idMap[IDMAX];
packet pack[BUFMAX];
HANDLE packMutex = NULL;
unsigned int index;

int main(int argc, char* argv[])
{
	//TODO::命令行参数
	SetLogLevel(LOG_INFO);
	InitLog(NULL);
	InitURLFilter("dnsrelay.txt");

	getServer(); //输入外部DNS
	SOCKET sock;
	initSocket(&sock);//初始化socket

	//初始化pack数组为空闲状态
	for (int i = 0; i < BUFMAX; i++)
		pack[i].type = 0;
	//初始化idMap数组为可使用该id
	for (int i = 0; i < IDMAX; i++)
		idMap[i].flag = 0;
	//pack线程锁
	packMutex = CreateMutex(NULL, FALSE, NULL);
	//开启服务端Listenning线程，将发送与接受分为两个线程处理
	CreateThread(NULL, 0, threadSend, &sock, 0, NULL);

	puts("Server is running.");

	//循环等待接受数据包
	while (1)
	{
		parameterPack* tmp = (parameterPack*)malloc(sizeof(parameterPack));
		tmp->sock = sock;

		tmp->buf = (char*)malloc(sizeof(char) * DNSBUFMAX);
		memset(tmp->buf, 0, sizeof(char) * DNSBUFMAX);

		int len = sizeof(SOCKADDR_IN);
		//接收包内容，存入缓冲区
		tmp->packSize = recvfrom(sock, tmp->buf, sizeof(char) * DNSBUFMAX, 0, (SOCKADDR*)&tmp->source, &len);

		if (tmp->packSize <= 0)
			continue;

		//处理接受到的包，并将缓冲区指针、sockaddr传出
		CreateThread(NULL, 0, dealPacket, tmp, 0, NULL);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}

void getServer()
{
	//按用户选择指定的DNS上游，否则使用默认DNS
	printf("Choose your DNS server(input -1 to skip):\n");
	//读入用户DNS
	scanf_s("%s", dnsServer, sizeof(dnsServer));
	if (strcmp(dnsServer, "-1") == 0)
	{
		//默认情况
		strcpy_s(dnsServer, sizeof(dnsServer), DEFAULTDNS);
	}
	else
	{
		unsigned long tmp;
		//将点分文本的IP地址转换为二进制网络字节序”的IP地址，判断IP是否合法
		//参数：IPv4类型、点分文本的IP地址
		int flag = inet_pton(AF_INET, dnsServer, &tmp);
		//非法时
		if (flag < 1)
		{
			printf("Error! Enable default server.\n");
			//应用默认DNS
			strcpy_s(dnsServer, sizeof(dnsServer), DEFAULTDNS);
		}
	}
	return;
}
//初始化socket
void initSocket(SOCKET* sock)
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
	if (bind(*sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Binding socket failed!\n");
		exit(1);
	}

	puts("Binding socket Success!");

	return;
}
//内外ID转换
unsigned short encodeID(SOCKADDR_IN src, char* buf)
{
	for (int i = 1; i < IDMAX; ++i)
	{
		//遍历可用的IDMapping
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
	return 0;
}
//成包，返回包长
int makePack(char* buf, int size, char* ip)
{
	//1000 0001:QR=1/RA=1
	buf[2] = 0x81;//response
	//查到的ip结果全为0：则block该地址
	if ((ip[3] | ip[2] | ip[1] | ip[0]) == 0)//block
		//RCode=3，返回NX_Domian错误
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
//发送线程函数（单独）
DWORD WINAPI threadSend(LPVOID lpParamter)
{
	SOCKET* sock = (SOCKET*)lpParamter;
	int cur = 0;

	puts("Creaing thread success.");

	while (1)
	{
		// 是否其实不需要等到100再放弃？没必要的啊
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
//处理报文，准备发送
DWORD WINAPI dealPacket(LPVOID lpParamter)/*(char* buf, int packSize, SOCKADDR_IN source, SOCKET sock)*/
{
	//获取参数包
	parameterPack* tmp = (parameterPack*)lpParamter;

	//id转换
	unsigned short outID = 0;

	//目标地址
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;

	//buf[2]为包头第三个字节
	//0x80;1000 0000，包头与其and,则取出最高位。最高位为QR，按其0、1分为查询与响应
	if ((tmp->buf[2] & 0x80) == 0) // 如果是查询报文的话
	{		
		//取出域名
		char name[DNSBUFMAX] = { 0 };
		int type = 1;
		//i=12:正文的QNAME字段
		for (int i = 12; i < tmp->packSize;)
		{
			if (tmp->buf[i] == 0)
			{
				//取出查询类型
				type = (int)(unsigned char)tmp->buf[i + 1] + (int)(unsigned char)tmp->buf[i + 2];
				break;
			}
			int cnt = (int)tmp->buf[i] + i + 1;
			for (i++; i < cnt; i++)
				//strlen:位置为首个值为0处，会随字符串内容更新而更新
				name[strlen(name)] = tmp->buf[i];
			if (tmp->buf[i] != 0)
				name[strlen(name)] = '.';
		}

		lprintf(LOG_INFO,"%8d: Type %02d, Name: %s\n",++index,type,name);

		char ipBuf[4] = "";
		//URLCheck：查询类型（enum）、url字符串、查询结果（二进制ip？）
		//buf[4]==0&buf[5]==1:QDCOUNT=1,即只有一条查询记录时
		int result = URLCheck(A, name, ipBuf);
		if (result && tmp->buf[4] == 0 && tmp->buf[5] == 1) // 存在本地文件中
		{
			//直接发回自构建返回包
			tmp->packSize = makePack(tmp->buf, tmp->packSize, ipBuf);

			//等待线程锁
			//WaitForSingleObject(packMutex, INFINITE);
			//获得锁后遍历缓冲区
			WaitForSingleObject(packMutex, INFINITE);
			for (int i = 0; i < BUFMAX; i++)
			{
				//找到了缓冲区中空闲可用的包结构体
				if (pack[i].type == 0)
				{
					//改为该包用于查询，等待发送线程遍历并发送
					pack[i].type = 1;

					//成包
					pack[i].size = tmp->packSize;
					pack[i].dest = tmp->source;
					for (int j = 0; j < tmp->packSize; j++)
						pack[i].buf[j] = tmp->buf[j];
					break;
				}
			}
			//释放锁
			ReleaseMutex(packMutex);
		}
		else // 不存在本地文件中，则转发上游
		{
			//改目标地址为上游服务器
			inet_pton(AF_INET, dnsServer, &dest.sin_addr.S_un.S_addr);
			dest.sin_port = htons(53);

			//outID默认为零，需要查找可用的可转换ID再发出
			outID = encodeID(tmp->source, tmp->buf);
			while (!outID)
			{
				//得到一个可用的转换id
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
	}
	else //响应报文
	{
		//取回ID,unsigned char待修改
		outID = (((unsigned short)(unsigned char)tmp->buf[0]) << 8) + (unsigned short)(unsigned char)tmp->buf[1];
		//由返回ID反查之前的ID
		if (idMap[outID].flag == 0)
		{
			printf("ID Mapping failed!\n");
		}
		else
		{
			//释放id映射占用
			idMap[outID].flag = 0;

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
		}
	}

	//释放动态申请内存
	free(tmp->buf);
	free(tmp);

	return 0L;
}
