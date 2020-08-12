#include "stdafx.h"

#include "dnsdef.h"
#include "dnsheader.h"
#include "log_output.h"
#include "url_filter.h"

int main(int argc, char* argv[])
{
	InitURLFilter("dnsrelay.txt");
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