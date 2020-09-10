#ifndef STDAFX_H
#define STDAFX_H
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "stdarg.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "getopt.h"
#if defined(_MSC_VER) or defined(_WIN32)
#include "WinSock2.h"
#include "Ws2tcpip.h"
#include "Windows.h"
#pragma comment(lib,"Ws2_32.lib")
#endif // _MSC_VER
#ifndef NOP
#define NOP
#else
#undef NOP
#endif // NOP
#ifdef __cplusplus
}
#endif
#endif // STDAFX_H