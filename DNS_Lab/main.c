#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "getopt.h"

#include "dnsheader.h"
#include "log_output.h"

int main(int argc, char* argv[])
{
	printf("%lld\n", sizeof(struct header));
	printf("%d\n", lprintf(LOG_INFO, "Test\n%d%c%lf\n", 1234, ' ', 98.76));
	Sleep(2000);
	printf("%d\n", lputs(LOG_INFO, "Hello World!"));
	return 0;
}