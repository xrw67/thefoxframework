/*
* @file tflog.h
* @brief 输出日志
*
* @ahthor macwe@qq.com
*/

#ifndef _TF_LOG_H_
#define _TF_LOG_H_

#ifdef USE_NAMESPACE
namespace TheFox {
#endif

#ifdef __cpluscpus
extern "C" {
#endif

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>


class TFLog
{
public:
	/// @brief 日志等级
	enum LogLevel
	{
		LOG_TRACE = 0,
		LOG_DEBUG = 1, //< 调试
		LOG_INFO = 2, //< 提示
		LOG_WARN = 3,//< 警告
		LOG_ERROR = 4 //< 错误
		LOG_FATAL = 5
	};

	TFLog(const char *logDir, const char *prefix, unsigned int logLevel)
	: m_file(NULL)
	, m_logDir(logDir)
	, m_prefix(prefix)
	, m_timeLogBegin(0)
	, m_timeLogEnd(0)
	, m_logLevel(logLevel)
	{
		ASSERT(logDir);
		ASSERT(prefix);
		
		OpenLog(logDir, prefix);
	}
	~TFLog()
	{
		CloseLog();
	}
	
	/// @brief 写入日志，带时间标记
	/// @param [in] logLevel 日志级别
	/// @param [in] fmt 日志的内容，格式与参见printf函数
	void Log(unsigned int logLevel, const char *fmt, ...)
	{
		if (NULL == m_file)
		{
			return;
		}
		if (logLevel < m_logLevel)
		{
			return;
		}
		
		time_t timeNow;
		time(&timeNow);
		
		if (timeNow < m_timeLogBegin || timeNow > m_timeLogEnd)
		{
			CloseLog();
			OpenLog(m_logDir.c_str(), m_prefix.c_str());
			if (NULL == m_file)
			{
				return;
			}
		}
		
		tm *pTm = localtime(&timeNow);
		fprintf(m_file, "%s %02d:%02d:%02d > ", GetLogLevelDescript(logLevel), pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		va_list ap;
		va_start(ap, fmt);
		vfprintf(m_file , fmt, ap);
		va_end(ap);
		fflush(m_file);
	}
	
	/// @brief 写入日志，不带时间标记
	/// @param [in] logLevel 日志级别
	/// @param [in] fmt 日志的内容，格式与参见printf函数
	void LogNoTimeTag(unsigned int logLevel, const char *fmt, ...)
	{
		if (NULL == m_file)
		{
			return;
		}
		if (logLevel < m_logLevel)
		{
			return;
		}
		
		time_t timeNow;
		time(&timeNow);
		
		if (timeNow < m_timeLogBegin || timeNow < m_timeLogEnd)
		{
			CloseLog();
			OpenLog(m_logDir.c_str(), m_prefix.c_str());
			if (NULL == m_file)
			{
				return;
			}
		}
		fprintf(m_file, "%s ", GetLogLevelDescript(logLevel));
		va_list ap;
		va_start(ap, fmt);
		vfprintf(m_file , fmt, ap);
		va_end(ap);
		fflush(m_file);
	}
	
	/// @brief 设置日志级别
	/// @param [in] 日志级别
	void SetLogLevel(unsigned int level)
	{
		m_logLevel = level;
	}
	
	/// @brief 得到日志级别
	unsigned int GetLogLevel() const
	{
		return m_logLevel;
	}

	
private:
	// 得到日志级别的描述文字
	const char *GetLogLevelDescript(const unsigned int logLevel) const
	{
		switch (logLevel)
		{
			case LOG_DEBUG: return "DEBUG";
			case LOG_INFO: return "INFO";
			case LOG_WARN: return "WARN";
			case LOG_ERROR: return "ERROE";
			default: return "";
		}
	}
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
	// 得到完整的路径，并且创建目录,末尾包含分隔符
	bool MakeFullPath(const char *dir, char *fullPath, size_t fullPathLen) const
	{
		if (fullPathLen < 2)
		{
			return false;
		}

		size_t pathLen = 0;
		char filePath[512] = {0};

		if (NULL == dir)
		{
			_getcwd(filePath, sizeof(filePath));
		}
		else
		{
			bool bAbsolutePath = true;

			// 判断dir是绝对路径还是相对路径
#ifdef WIN32
			if (NULL == strchr(dir, ':'))
			{
				bAbsolutePath = false;
			}
#else
			if (dir[0] != '/')
			{
				bAbsolutePath = false;
			}
#endif
			_getcwd(filePath, sizeof(filePath));

			if (!bAbsolutePath) // 如果是相对路径，生成绝对路径
			{
				pathLen = strlen(filePath);
				if ('/' != filePath[pathLen] || '\\' != filePath[pathLen])
				{
					strcat(filePath, "/");
					++pathLen;
				}
				strncat(filePath, dir, sizeof(filePath) - pathLen);
			}
			//! 创建目录
			char *curDir = filePath;
			while ('\0' != *curDir)
			{
				if ('\\' == *curDir || '/' == *curDir)
				{
					*curDir = '\0';
					_mkdir(filePath);
					*curDir = '/';
				}
				++curDir;
			}
			_mkdir(filePath);
		}

		pathLen = strlen(filePath);
		if ('/' != filePath[pathLen])
		{
			strcat(filePath, "/");
			++pathLen;
		}
		if (pathLen < fullPathLen)
		{
			strcpy(fullPath, filePath);
			return true;
		}
		return false;
	}

	FILE *m_file; // 日志文件指针
	std::string m_logDir; // 日志文件路径，不含文件名
	std::string m_prefix; // 日志文件前缀
	// 这两个用来保证每天生成一个日志文件
	time_t m_timeLogBegin;// 写日志文件开始时间戳
	time_t m_timeLogEnd;// 写日志文件结束时间戳
	unsigned int m_logLevel; // 日志级别
};


#define tfInitLogLib(dir, prefix, logLevel) TFLog g_log(dir, prefix, logLevel);

#define tfImportLogLib extern TFLog g_log;

#define tfLog g_log.Log

#ifdef __cpluscpus
};
#endif

#ifdef USE_NAMESPACE
};
#endif

#endif // _TF_LOG_H_