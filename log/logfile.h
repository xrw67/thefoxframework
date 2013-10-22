/*
* @file tflog.h
* @brief 输出日志
*
* @ahthor macwe@qq.com
*/

#ifndef _THEFOX_LOGFILE_H_
#define _THEFOX_LOGFILE_H_

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <base/MutexLock.h>
#include <base/Directory.h>
#include <base/scoped_ptr.h>
namespace thefox
{


class LogFile : noncopyable
{
public:
	LogFile(const String& basename,
	size_t rollSize,
	bool threadSafe = true,
	int flushInterval = 3);
	~LogFile();

	void append(const char* logline, int len);
	void flush();

private:
	void append_unlocked(const char* logline, int len);

	static String getLogFileName(const String& basename, time_t* now);
	void rollFile();

	const String _basename;
	const size_t _rollSize;///< 文件缓冲中达到这么多字节的数据后写到文件里
	const int _flushInterval;///< 每隔这个时间将文件缓冲区中的数据写到文件中

	int _count;

	scoped_ptr<MutexLock> _mutex;
	time_t _startOfPeriod;
	time_t _lastRoll;
	time_t _lastFlush;
	class File;
	scoped_ptr<File> _file;

	const static int kCheckTimeRoll_ = 1024; ///< 统计达到这么多行后写到文件
	const static int kRollPerSeconds_ = 60*60*24;
};



// 日志操作类
class TFLog
{
public:
	// 打开日志文件
	bool OpenLog(const char *logDir, const char *prefix)
	{
		if (NULL != m_file)
		{
			CloseLog();
		}

		char filePath[512] = {0};
		if (MakeFullPath(logDir, filePath, sizeof(filePath)))
		{
			time(&m_timeLogBegin);
			tm *pTm = localtime(&m_timeLogBegin);

			tm tmTemp;
			tmTemp = *pTm;

			pTm->tm_hour = 0;
			pTm->tm_min = 0;
			pTm->tm_sec = 0;
			m_timeLogBegin = mktime(pTm);

			m_timeLogEnd = m_timeLogBegin + 86400;

			if (NULL != prefix && '\0' != *prefix)
			{
				sprintf(&filePath[strlen(filePath)], "%s", prefix);
			}
			sprintf(&filePath[strlen(filePath)], "%04d-%02d-%02d.log",
				pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
			m_file = fopen(filePath, "a+");
			if (m_file)
			{
				fprintf(m_file, "Log begin at:%02d:%02d:%02d\r\n", tmTemp.tm_hour, tmTemp.tm_min, tmTemp.tm_sec);
			}
		}
		return true;
	}
	// 关闭日志文件
	bool CloseLog()
	{
		if (m_file)
		{
			time_t timeNow;
			time(&timeNow);
			tm *pTm = localtime(&timeNow);
			fprintf(m_file, "Log end at:%02d:%02d:%02d\r\n", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
			
			fclose(m_file);
			m_file = NULL;
		}
		return true;;
	}
	

	FILE *m_file; // 日志文件指针
	std::string m_logDir; // 日志文件路径，不含文件名
	std::string m_prefix; // 日志文件前缀
	// 这两个用来保证每天生成一个日志文件
	time_t m_timeLogBegin;// 写日志文件开始时间戳
	time_t m_timeLogEnd;// 写日志文件结束时间戳
	unsigned int m_logLevel; // 日志级别
};

}

#endif // _THEFOX_LOGFILE_H_