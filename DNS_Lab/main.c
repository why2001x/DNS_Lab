/**
 * @file  main.c
 * @brief 主程序
 */
#include "stdafx.h"

#include "dnsdef.h"
#include "dnsheader.h"
#include "log_output.h"
#include "url_filter.h"

int main(int argc, char* argv[])
{
	srand(time(NULL));
	int ec = 0;
	for (int i = 0; i < (int)1e2; i++)
	{
		if (InitURLFilter("dnsrelay.txt"))
		{
			if (++ec % 10 == 0)
			{
				printf(" EC: %d\r",ec);
			}
		}URLCheck((enum QueryType)A, "www.zhihu.com", NULL);
	}
	puts("");
	uchar buf[10] = { 255,255,255,255,255 };
	for (int cnt = 1; cnt <= 5; cnt++)
	{
		printf("%d: %d ", cnt, URLCheck((enum QueryType)A, "www.zhihu.com", buf));
		printf("%d.%d.%d.%d\n", buf[3], buf[2], buf[1], buf[0]);
	}
	return 0;
	printf("%lld\n", sizeof(struct header));
	printf("%d\n", lprintf(LOG_WARN, "Test\n", 1234, ' ', 98.76));
	printf("%d\n", lprintf(LOG_INFO, "%d%c%lf\n", 1234, ' ', 98.76));
	Sleep(2000);
	SetLogLevel(LOG_ERRN);
	printf("%d\n", lputs(LOG_WARN, "Hello World!"));
	return 0;
}

/**
 * @mainpage 《计算机网络》课程设计 文档
 *
 * @section 成员信息
 *
 * 北京邮电大学 计算机学院 2018级
 * - 方淇   学号：2018211356
 * - 王海昱 学号：2018211358
 * - 何志臻 学号：2018211364
 *
 * @section 实验环境
 *
 * Windows10(x86_64) + VS2019
 *
 * @section 实验概况
 * 
 */