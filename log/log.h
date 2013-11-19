/*
* @filename log.h
* @brief log writer library
* @author macwe@qq.com
* example:
* {
*   // step1:
*   thefox::initLog("logs", "basename");
*   // step2:
*   LOG_TRACE << "log text"<< 123 << ' ' << -3.14;
*   LOG_DEBUG << "log text"<< 123 << ' ' << -3.14;
*   LOG_INFO << "log text"<< 123 << ' ' << -3.14;
*   LOG_WARN << "log text"<< 123 << ' ' << -3.14;
*   LOG_ERROR << "log text"<< 123 << ' ' << -3.14;
*   LOG_FATAL << "log text"<< 123 << ' ' << -3.14;
* }
*/
#ifndef _THEFOX_LOG_H_
#define _THEFOX_LOG_H_

#include <memory>
#include <base/scoped_ptr.h>
#include <log/logger.h>
#include <log/logfile.h>

namespace thefox
{

static scoped_ptr<LogFile> g_logFile;

inline void logOutputFunc(const char* msg, int len)
{
	g_logFile->append(msg, len);
}

inline void logFlushFunc()
{
	g_logFile->flush();
}

/// @brief initialize log library
inline void initLog(const String &dir, const String& basename)
{
	g_logFile.reset(new LogFile(dir, basename, 1024*1024));
	Logger::setOutput(logOutputFunc);
	Logger::setFlush(logFlushFunc);
}

inline void setLogLevel(Logger::LogLevel level)
{
	Logger::setLogLevel(level);
}

}

#endif // _THEFOX_LOG_H_