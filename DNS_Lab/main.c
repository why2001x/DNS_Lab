#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "getopt.h"

//#define NO_LOG_SERVICE
#include "dnsheader.h"
#include "log_output.h"

int main(int argc, char* argv[])
{
	lputs(LOG_ERRN, "Main: Test No Log!");
	return 0;
	printf("%lld\n", sizeof(struct header));
	printf("%d\n", lprintf(LOG_WARN, "Test\n", 1234, ' ', 98.76));
	printf("%d\n", lprintf(LOG_INFO, "%d%c%lf\n", 1234, ' ', 98.76));
	Sleep(2000);
	SetLogLevel(LOG_ERRN);
	printf("%d\n", lputs(LOG_WARN, "Hello World!"));
	return 0;
}