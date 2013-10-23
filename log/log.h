#ifndef _THEFOX_LOG_H_
#define _THEFOX_LOG_H_

#include <base/scoped_ptr.h>
#include <log/logger.h>
#include <log/logfile.h>

namespace thefox
{

scoped_ptr<LogFile> g_logFile;

void logOutputFunc(const char* msg, int len)
{
	g_logFile->append(msg, len);
}

void logFlushFunc()
{
	g_logFile->flush();
}

void initLog(const String& basename)
{
	g_logFile.reset(new LogFile(basename, 200*1000));
	Logger::setOutput(logOutputFunc);
	Logger::setFlush(logFlushFunc);
}

}

#endif // _THEFOX_LOG_H_