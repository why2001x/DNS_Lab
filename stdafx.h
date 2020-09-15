/**
 * @file    stdafx.h
 * @brief   预编译头文件
 * @author  why2001
 * @version 0.0.1
 */
#ifndef STDAFX_H
#define STDAFX_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdarg.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#ifdef _MSC_VER
//#include "xthreads.h"
#else
#include "threads.h"
#endif

#include "getopt.h"

#if (defined(_MSC_VER) || defined(_WIN32))

#include <sys/stat.h>

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

    // #define NO_LOG_SERVICE

#ifdef __cplusplus
}
#endif
#endif // STDAFX_H
