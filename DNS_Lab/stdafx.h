/**
 * @brief Ô¤±àÒëÍ·ÎÄ¼þ
 * @author why2001
 * @version 0.0.1
 */
#ifndef STDAFX_H
#define STDAFX_H

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
#include "windows.h"
#endif // _MSC_VER


#ifndef NOP
#define NOP
#else
#undef NOP
#endif // NOP

// #define NO_LOG_SERVICE

#endif // STDAFX_H
