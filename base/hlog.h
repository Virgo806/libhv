#ifndef HV_LOG_H_
#define HV_LOG_H_

/*
 * @功能：写日志
 *
 */

/*
 * hlog is thread-safe
 */

#include <string.h>

#ifdef _WIN32
#define DIR_SEPARATOR       '\\'
#define DIR_SEPARATOR_STR   "\\"
#else
#define DIR_SEPARATOR       '/'
#define DIR_SEPARATOR_STR   "/"
#endif

// __FILE__是完整路径宏，对于日志来说太长，这里我们提取出文件名__FILENAME__
#ifndef __FILENAME__
// #define __FILENAME__  (strrchr(__FILE__, DIR_SEPARATOR) ? strrchr(__FILE__, DIR_SEPARATOR) + 1 : __FILE__)
#define __FILENAME__  (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#include "hexport.h"

#ifdef __cplusplus
extern "C" {
#endif

// 定义一些终端颜色宏，方便区分日志级别
#define CLR_CLR         "\033[0m"       /* 恢复颜色 */
#define CLR_BLACK       "\033[30m"      /* 黑色字 */
#define CLR_RED         "\033[31m"      /* 红色字 */
#define CLR_GREEN       "\033[32m"      /* 绿色字 */
#define CLR_YELLOW      "\033[33m"      /* 黄色字 */
#define CLR_BLUE        "\033[34m"      /* 蓝色字 */
#define CLR_PURPLE      "\033[35m"      /* 紫色字 */
#define CLR_SKYBLUE     "\033[36m"      /* 天蓝字 */
#define CLR_WHITE       "\033[37m"      /* 白色字 */

#define CLR_BLK_WHT     "\033[40;37m"   /* 黑底白字 */
#define CLR_RED_WHT     "\033[41;37m"   /* 红底白字 */
#define CLR_GREEN_WHT   "\033[42;37m"   /* 绿底白字 */
#define CLR_YELLOW_WHT  "\033[43;37m"   /* 黄底白字 */
#define CLR_BLUE_WHT    "\033[44;37m"   /* 蓝底白字 */
#define CLR_PURPLE_WHT  "\033[45;37m"   /* 紫底白字 */
#define CLR_SKYBLUE_WHT "\033[46;37m"   /* 天蓝底白字 */
#define CLR_WHT_BLK     "\033[47;30m"   /* 白底黑字 */

// XXX(id, str, clr)
#define LOG_LEVEL_MAP(XXX) \
    XXX(LOG_LEVEL_DEBUG, "DEBUG", CLR_WHITE)     \
    XXX(LOG_LEVEL_INFO,  "INFO ", CLR_GREEN)     \
    XXX(LOG_LEVEL_WARN,  "WARN ", CLR_YELLOW)    \
    XXX(LOG_LEVEL_ERROR, "ERROR", CLR_RED)       \
    XXX(LOG_LEVEL_FATAL, "FATAL", CLR_RED_WHT)

// 定义日志级别
typedef enum {
    LOG_LEVEL_VERBOSE = 0,
#define XXX(id, str, clr) id,
    LOG_LEVEL_MAP(XXX)
#undef  XXX
    LOG_LEVEL_SILENT
} log_level_e;

#define DEFAULT_LOG_FILE            "libhv"
#define DEFAULT_LOG_LEVEL           LOG_LEVEL_INFO
#define DEFAULT_LOG_REMAIN_DAYS     1
#define DEFAULT_LOG_MAX_BUFSIZE     (1<<14)  // 16k
#define DEFAULT_LOG_MAX_FILESIZE    (1<<24)  // 16M

// logger: default file_logger
// network_logger() see event/nlog.h
typedef void (*logger_handler)(int loglevel, const char* buf, int len);

// 实现了几种常见的日志处理器，标准输出、标准错误、写文件
HV_EXPORT void stdout_logger(int loglevel, const char* buf, int len);
HV_EXPORT void stderr_logger(int loglevel, const char* buf, int len);
HV_EXPORT void file_logger(int loglevel, const char* buf, int len);
// 在event模块中我们还实现了一个网络日志服务，
// 连上的客户端就会收到日志，是不是有点像android的logcat呢
// network_logger implement see event/nlog.h
// HV_EXPORT void network_logger(int loglevel, const char* buf, int len);

typedef struct logger_s logger_t;
// 创建日志器
HV_EXPORT logger_t* logger_create();
// 销毁日志器
HV_EXPORT void logger_destroy(logger_t* logger);

// 设置日志处理器
HV_EXPORT void logger_set_handler(logger_t* logger, logger_handler fn);
// 设置日志级别
HV_EXPORT void logger_set_level(logger_t* logger, int level);
// level = [VERBOSE,DEBUG,INFO,WARN,ERROR,FATAL,SILENT]
HV_EXPORT void logger_set_level_by_str(logger_t* logger, const char* level);
// 设置一条日志最大的长度
HV_EXPORT void logger_set_max_bufsize(logger_t* logger, unsigned int bufsize);
// 启用日志颜色打印
HV_EXPORT void logger_enable_color(logger_t* logger, int on);
// 打印日志
HV_EXPORT int  logger_print(logger_t* logger, int level, const char* fmt, ...);

// 下面是文件日志器的一些设置
// below for file logger
// 设置日志文件
HV_EXPORT void logger_set_file(logger_t* logger, const char* filepath);
// 设置日志文件最大尺寸
HV_EXPORT void logger_set_max_filesize(logger_t* logger, unsigned long long filesize);
// 16, 16M, 16MB
HV_EXPORT void logger_set_max_filesize_by_str(logger_t* logger, const char* filesize);
// 设置日志保留天数
HV_EXPORT void logger_set_remain_days(logger_t* logger, int days);
// 启用fsync
// @NOTE: fsync用来强制刷新缓存到磁盘，日志的实时可见性更好，但会影响性能，
// 在日志很多的场合，建议关闭此选项，可使用定时器定时调用logger_fsync去刷新到磁盘。
HV_EXPORT void logger_enable_fsync(logger_t* logger, int on);
// 强制调用fsync刷新缓存到磁盘
HV_EXPORT void logger_fsync(logger_t* logger);
// 获取当前的日志文件路径
HV_EXPORT const char* logger_get_cur_file(logger_t* logger);

// 提供一个默认的日志器
// hlog: default logger instance
HV_EXPORT logger_t* hv_default_logger();

// hlog_xxx即使用默认的日志器
// macro hlog*
#define hlog                            hv_default_logger()
#define hlog_set_file(filepath)         logger_set_file(hlog, filepath)
#define hlog_set_level(level)           logger_set_level(hlog, level)
#define hlog_set_level_by_str(level)    logger_set_level_by_str(hlog, level)
#define hlog_set_max_filesize(filesize) logger_set_max_filesize(hlog, filesize)
#define hlog_set_max_filesize_by_str(filesize) logger_set_max_filesize_by_str(hlog, filesize)
#define hlog_set_remain_days(days)      logger_set_remain_days(hlog, days)
#define hlog_enable_fsync()             logger_enable_fsync(hlog, 1)
#define hlog_disable_fsync()            logger_enable_fsync(hlog, 0)
#define hlog_fsync()                    logger_fsync(hlog)
#define hlog_get_cur_file()             logger_get_cur_file(hlog)

#define hlogd(fmt, ...) logger_print(hlog, LOG_LEVEL_DEBUG, fmt " [%s:%d:%s]\n", ## __VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define hlogi(fmt, ...) logger_print(hlog, LOG_LEVEL_INFO,  fmt " [%s:%d:%s]\n", ## __VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define hlogw(fmt, ...) logger_print(hlog, LOG_LEVEL_WARN,  fmt " [%s:%d:%s]\n", ## __VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define hloge(fmt, ...) logger_print(hlog, LOG_LEVEL_ERROR, fmt " [%s:%d:%s]\n", ## __VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define hlogf(fmt, ...) logger_print(hlog, LOG_LEVEL_FATAL, fmt " [%s:%d:%s]\n", ## __VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)

// 在android平台，使用__android_log_print
// below for android
#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#define LOG_TAG "JNI"
#undef  hlogd
#undef  hlogi
#undef  hlogw
#undef  hloge
#undef  hlogf
#define hlogd(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define hlogi(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define hlogw(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define hloge(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define hlogf(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)
#endif

// 更通用的日志宏
// macro alias
#if !defined(LOGD) && !defined(LOGI) && !defined(LOGW) && !defined(LOGE) && !defined(LOGF)
#define LOGD    hlogd
#define LOGI    hlogi
#define LOGW    hlogw
#define LOGE    hloge
#define LOGF    hlogf
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // HV_LOG_H_
