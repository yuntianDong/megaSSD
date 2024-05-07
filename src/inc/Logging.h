#include <stdio.h>
#include <time.h>
#include <string>

#define	SYS_LOG_ERROR			10			// log level：ERROR
#define SYS_LOG_FATAL			20			// log level：FATAL
#define	SYS_LOG_WARNING			30			// log level：WARNING
#define	SYS_LOG_INFO			40			// log level：INFO
#define SYS_LOG_STEP            50          // log level：STEP
#define SYS_LOG_SKIP            60          // log level：SKIP
#define SYS_LOG_NOTICE          70          // log level：NOTICE
#define	SYS_LOG_DEBUG			80			// log level：DEBUG

#ifndef SYSLOGLEVEL
#define SYSLOGLEVEL				SYS_LOG_DEBUG
#endif

#define LOG(level, fmt, ...) \
    do { \
        time_t t = time(NULL); \
        struct tm* now = localtime(&t); \
        char timeStr[64]; \
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", now); \
        const char* lvl; \
        switch(level) { \
            case SYS_LOG_ERROR: lvl = "ERROR"; break; \
            case SYS_LOG_WARNING: lvl = "WARNING"; break; \
            case SYS_LOG_INFO: lvl = "INFO"; break; \
            case SYS_LOG_DEBUG: lvl = "DEBUG"; break; \
            default: lvl = "UNKNOWN"; break; \
        } \
        fprintf(stderr, "[%s] [%s:%4d] %s : " fmt "\n", timeStr, __FILE__, __LINE__, lvl, ##__VA_ARGS__); \
    } while(0)

#if(SYSLOGLEVEL >= SYS_LOG_DEBUG)
#define LOGDEBUG(fmt, ...)	LOG(SYS_LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOGDEBUG(fmt, ...)
#endif

#if(SYSLOGLEVEL >= SYS_LOG_INFO)
#define LOGINFO(fmt, ...)	LOG(SYS_LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define LOGINFO(fmt, ...)
#endif

#if(SYSLOGLEVEL >= SYS_LOG_WARNING)
#define LOGWARN(fmt, ...)	LOG(SYS_LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define LOGWARN(fmt, ...)
#endif

#if(SYSLOGLEVEL >= SYS_LOG_ERROR)
#define LOGERROR(fmt, ...)	LOG(SYS_LOG_ERROR, fmt, ##__VA_ARGS__)
#else
#define LOGERROR(fmt, ...)
#endif