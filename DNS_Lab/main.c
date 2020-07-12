#include "stdbool.h"
#include "stdio.h"

#include "getopt.h"

#include "dnsheader.h"

int main(int argc, char* argv[])
{
	printf("%lld\n", sizeof(struct header));
	return 0;
}