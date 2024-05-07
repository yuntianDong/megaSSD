/*
 * file : Wrapper.cpp
 */
#include <boost/python.hpp>
#include "Logging.h"

#define PY_LOG(filename,line,level, fmt, ...) \
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
            case SYS_LOG_STEP: lvl = "STEP"; break; \
            case SYS_LOG_FATAL: lvl = "FATAL"; break; \
            case SYS_LOG_NOTICE: lvl = "NOTICE"; break; \
            case SYS_LOG_SKIP: lvl = "SKIP"; break; \
            default: lvl = "UNKNOWN"; break; \
        } \
        fprintf(stderr, "[%s] [%s:%4d] %s : " fmt "\n", timeStr, filename, line, lvl, ##__VA_ARGS__); \
    } while(0)

using namespace boost::python;


void LogDebug(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_DEBUG, "%s",msg.c_str());
}

void LogInfo(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_INFO, "%s",msg.c_str());
}

void LogWarn(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_WARNING, "%s",msg.c_str());
}

void LogError(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_ERROR, "%s",msg.c_str());
}

void LogStep(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_STEP, "%s",msg.c_str());
}

void LogFatal(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_FATAL, "%s",msg.c_str());
}

void LogNotice(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_NOTICE, "%s",msg.c_str());
}

void LogSkip(std::string filename,uint32_t line,std::string msg)
{
    PY_LOG(filename.c_str(),line,SYS_LOG_SKIP, "%s",msg.c_str());
}

BOOST_PYTHON_MODULE(xLogger)
{
    scope().attr("SYS_LOG_ERROR")   = 10;
    scope().attr("SYS_LOG_FATAL")   = 20;
    scope().attr("SYS_LOG_WARNING") = 30;
    scope().attr("SYS_LOG_INFO")    = 40;
    scope().attr("SYS_LOG_STEP")    = 50;
    scope().attr("SYS_LOG_SKIP")    = 60;
    scope().attr("SYS_LOG_NOTICE")  = 70;
    scope().attr("SYS_LOG_DEBUG")   = 80;

    def("LOGDEBUG", LogDebug);
    def("LOGINFO",  LogInfo);
    def("LOGWARN",  LogWarn);
    def("LOGERROR", LogError);
    def("LOGSTEP",  LogStep);
    def("LOGFATAL", LogFatal);
    def("LOGSKIP",  LogSkip);
    def("LOGNOTICE", LogNotice);
}