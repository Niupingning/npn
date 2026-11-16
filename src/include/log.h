/*
 * @file  logger.h
 * @date 2021/08/6
 *
 * 安卓log服务使用了以下几种log优先级， 也即记录类型， 注意， 有的记录类型只记录到环形的log缓存中，有的则不仅记录到环形的log缓存中，
 * 而且还会打印到屏幕上，另外还有几种类型不会记录。
 * 
 * typedef enum android_LogPriority {
 *	 ANDROID_LOG_UNKNOWN = 0,
 *	 ANDROID_LOG_DEFAULT,	
 *	 ANDROID_LOG_VERBOSE,
 *	 ANDROID_LOG_DEBUG,
 *	 ANDROID_LOG_INFO,
 *	 ANDROID_LOG_WARN,
 *	 ANDROID_LOG_ERROR,
 *	 ANDROID_LOG_FATAL,
 *	 ANDROID_LOG_SILENT,	 
 * } android_LogPriority;
 *
 *
 * 每种记录类型在记录的条目中有不同的标志如下所示:
 *	 ANDROID_LOG_UNKNOWN	 	记录标志： （这种类型不会记录在记录条目中，故没有标志）
 *	 ANDROID_LOG_DEFAULT 		记录标志： （这种类型不会记录在记录条目中，故没有标志）
 *	 ANDROID_LOG_VERBOSE 		记录标志： V
 *	 ANDROID_LOG_DEBUG		 	记录标志： D
 *	 ANDROID_LOG_INFO		 	记录标志： I
 *	 ANDROID_LOG_WARN		 	记录标志： W
 *	 ANDROID_LOG_ERROR		 	记录标志： E
 *	 ANDROID_LOG_FATAL		 	记录标志： F
 *	 ANDROID_LOG_SILENT 	 	记录标志： S
 *
 * 同时如下4种记录类型不仅会记录在缓存中，即记录条目中，同时也会打印在屏幕上：
 *	  ANDROID_LOG_WARN
 *	  ANDROID_LOG_ERROR
 *	  ANDROID_LOG_FATAL
 *	  ANDROID_LOG_SILENT
 *
 * 记录条目在缓存中作如下格式的保存：
 *	记录标志/用户自定义的标签（调用的进程ID): 用户自定义的记录信息
 * 
 * 使用时注意以下几点： 
 * 1， 使用前请先定义自己的模块标签标志，以后可用模块标签标志进行过滤查询， 即： LOG_TAGT 宏
 * 2， 编译自己的模块时一定要包含共享库： liblog.so
 * 3， 当前的log环形缓存大小为256K 字节左右，如果记录条目增加，以及最后超过这个大小时，环形缓存中
 *     保存的总是最新的条目数，即旧的条目数会逐次移出， 新的条目数逐条进入。
 * 4， 保存于环形缓存中的记录条目可通过工具程序logcat查看及导出到自己指定的文件中。logcat的用法可通过命令行获得：
 *	   logcat --help
 *
 * 用法示例 ：
 * LOGINF(" --- %d: VOLTAGE OVER and Value : %d --- PID : %d", 50, 60, getpid());
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <strings.h>
#include <android/log.h>

#ifndef LOG_TAGT
#define LOG_TAGT "IVIHOMEAPP"
#endif


#define LOGINF(fmt, ...)    __android_log_print(ANDROID_LOG_INFO,    LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)
#define LOGERR(fmt, ...)    __android_log_print(ANDROID_LOG_ERROR,   LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)
#define LOGWAR(fmt, ...)    __android_log_print(ANDROID_LOG_WARN,    LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)
#define LOGDBG(fmt, ...)    __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)
#define LOGVERBOSE(fmt, ...)    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)

#define LOGFATAL(fmt, ...)    __android_log_print(ANDROID_LOG_FATAL, LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)
#define LOGSILENT(fmt, ...)    __android_log_print(ANDROID_LOG_SILENT, LOG_TAGT, "[%s:%d]-" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__)



#endif 





