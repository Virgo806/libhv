#ifndef HV_PLATFORM_H_
#define HV_PLATFORM_H_

/*
 * @功能：此头文件主要定义了平台相关宏
 *
 */

#include "hconfig.h"

// OS
// 操作系统宏
#if defined(WIN64) || defined(_WIN64)
    #define OS_WIN64
    #define OS_WIN32
#elif defined(WIN32)|| defined(_WIN32)
    #define OS_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
    #define OS_ANDROID
    #define OS_LINUX
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define OS_LINUX
#elif defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_MAC) && TARGET_OS_MAC
        #define OS_MAC
    #elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        #define OS_IOS
    #endif
    #define OS_DARWIN
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define OS_FREEBSD
    #define OS_BSD
#elif defined(__NetBSD__)
    #define OS_NETBSD
    #define OS_BSD
#elif defined(__OpenBSD__)
    #define OS_OPENBSD
    #define OS_BSD
#elif defined(sun) || defined(__sun) || defined(__sun__)
    #define OS_SOLARIS
#else
    #error "Unsupported operating system platform!"
#endif

#if defined(OS_WIN32) || defined(OS_WIN64)
    #undef  OS_UNIX
    #define OS_WIN
#else
    #define OS_UNIX
#endif

// ARCH
// CPU体系结构宏
#if defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #define ARCH_X86
    #define ARCH_X86_32
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #define ARCH_X64
    #define ARCH_X86_64
#elif defined(__arm__)
    #define ARCH_ARM
#elif defined(__aarch64__) || defined(__ARM64__)
    #define ARCH_ARM64
#else
    #define ARCH_UNKNOWN
    #warning "Unknown hardware architecture!"
#endif

// COMPILER
// 编译器宏
#if defined (_MSC_VER)
#define COMPILER_MSVC

#if (_MSC_VER < 1200) // Visual C++ 6.0
#define MSVS_VERSION    1998
#define MSVC_VERSION    60
#elif (_MSC_VER >= 1200) && (_MSC_VER < 1300) // Visual Studio 2002, MSVC++ 7.0
#define MSVS_VERSION    2002
#define MSVC_VERSION    70
#elif (_MSC_VER >= 1300) && (_MSC_VER < 1400) // Visual Studio 2003, MSVC++ 7.1
#define MSVS_VERSION    2003
#define MSVC_VERSION    71
#elif (_MSC_VER >= 1400) && (_MSC_VER < 1500) // Visual Studio 2005, MSVC++ 8.0
#define MSVS_VERSION    2005
#define MSVC_VERSION    80
#elif (_MSC_VER >= 1500) && (_MSC_VER < 1600) // Visual Studio 2008, MSVC++ 9.0
#define MSVS_VERSION    2008
#define MSVC_VERSION    90
#elif (_MSC_VER >= 1600) && (_MSC_VER < 1700) // Visual Studio 2010, MSVC++ 10.0
#define MSVS_VERSION    2010
#define MSVC_VERSION    100
#elif (_MSC_VER >= 1700) && (_MSC_VER < 1800) // Visual Studio 2012, MSVC++ 11.0
#define MSVS_VERSION    2012
#define MSVC_VERSION    110
#elif (_MSC_VER >= 1800) && (_MSC_VER < 1900) // Visual Studio 2013, MSVC++ 12.0
#define MSVS_VERSION    2013
#define MSVC_VERSION    120
#elif (_MSC_VER >= 1900) && (_MSC_VER < 1910) // Visual Studio 2015, MSVC++ 14.0
#define MSVS_VERSION    2015
#define MSVC_VERSION    140
#elif (_MSC_VER >= 1910) && (_MSC_VER < 1920) // Visual Studio 2017, MSVC++ 15.0
#define MSVS_VERSION    2017
#define MSVC_VERSION    150
#elif (_MSC_VER >= 1920) && (_MSC_VER < 2000) // Visual Studio 2019, MSVC++ 16.0
#define MSVS_VERSION    2019
#define MSVC_VERSION    160
#endif

#undef  HAVE_STDATOMIC_H
#define HAVE_STDATOMIC_H        0
#undef  HAVE_SYS_TIME_H
#define HAVE_SYS_TIME_H         0
#undef  HAVE_PTHREAD_H
#define HAVE_PTHREAD_H          0

// 禁掉一些MSVC编译时警告
#pragma warning (disable: 4018) // signed/unsigned comparison
#pragma warning (disable: 4100) // unused param
#pragma warning (disable: 4244) // conversion loss of data
#pragma warning (disable: 4251) // STL dll
#pragma warning (disable: 4267) // size_t => int
#pragma warning (disable: 4819) // Unicode
#pragma warning (disable: 4996) // _CRT_SECURE_NO_WARNINGS

#elif defined(__MINGW32__) || defined(__MINGW64__)
#define COMPILER_MINGW

#elif defined(__GNUC__)
#define COMPILER_GCC

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#elif defined(__clang__)
#define COMPILER_CLANG

#else
#warning "Untested compiler!"
#endif

// headers
// #include一些常用头文件
#ifdef OS_WIN
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <winsock2.h>
    #include <ws2tcpip.h>   // for inet_pton,inet_ntop
    #include <windows.h>
    #include <process.h>    // for getpid,exec
    #include <direct.h>     // for mkdir,rmdir,chdir,getcwd
    #include <io.h>         // for open,close,read,write,lseek,tell

    #define hv_sleep(s)     Sleep((s) * 1000)
    #define hv_msleep(ms)   Sleep(ms)
    #define hv_usleep(us)   Sleep((us) / 1000)
    #define hv_delay(ms)    hv_msleep(ms)
    #define hv_mkdir(dir)   mkdir(dir)

    #ifndef S_ISREG
    #define S_ISREG(st_mode) (((st_mode) & S_IFMT) == S_IFREG)
    #endif
    #ifndef S_ISDIR
    #define S_ISDIR(st_mode) (((st_mode) & S_IFMT) == S_IFDIR)
    #endif
#else
    #include <unistd.h>
    #include <dirent.h>     // for mkdir,rmdir,chdir,getcwd

    // socket
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netinet/udp.h>
    #include <netdb.h>  // for gethostbyname

    #define hv_sleep(s)     sleep(s)
    #define hv_msleep(ms)   usleep((ms) * 1000)
    #define hv_usleep(us)   usleep(us)
    #define hv_delay(ms)    hv_msleep(ms)
    #define hv_mkdir(dir)   mkdir(dir, 0777)
#endif

#ifdef _MSC_VER
    typedef int pid_t;
    typedef int gid_t;
    typedef int uid_t;
    #define strcasecmp  stricmp
    #define strncasecmp strnicmp
#else
    typedef int                 BOOL;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef void*               HANDLE;
    #include <strings.h>
    #define stricmp     strcasecmp
    #define strnicmp    strncasecmp
#endif

// ENDIAN
// 大小端宏
#ifndef BIG_ENDIAN
#define BIG_ENDIAN      4321
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN   1234
#endif
#ifndef NET_ENDIAN
#define NET_ENDIAN      BIG_ENDIAN
#endif

// BYTE_ORDER
// 字节序宏
#ifndef BYTE_ORDER
#if defined(ARCH_X86) || defined(ARCH_X86_64) || defined(__ARMEL__)
#define BYTE_ORDER      LITTLE_ENDIAN
#elif defined(__ARMEB__)
#define BYTE_ORDER      BIG_ENDIAN
#endif
#endif

// ANSI C
// #include ANSI C 标准头文件
#include <assert.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>

// bool类型兼容处理
// c99前C语言是没有bool类型的
#ifndef __cplusplus
#if HAVE_STDBOOL_H
#include <stdbool.h>
#else
    #ifndef bool
    #define bool char
    #endif

    #ifndef true
    #define ture 1
    #endif

    #ifndef false
    #define false 0
    #endif
#endif
#endif

// int类型兼容处理
#if HAVE_STDINT_H
#include <stdint.h>
#elif defined(_MSC_VER) && _MSC_VER < 1700
typedef __int8              int8_t;
typedef __int16             int16_t;
typedef __int32             int32_t;
typedef __int64             int64_t;
typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;
#endif

typedef float               float32_t;
typedef double              float64_t;

typedef int (*method_t)(void* userdata);
typedef void (*procedure_t)(void* userdata);

// 根据生成的hconfig.h，#include一些非ANSI C 头文件
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>   // for gettimeofday
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_PTHREAD_H
#include <pthread.h>
#endif

#endif // HV_PLATFORM_H_
