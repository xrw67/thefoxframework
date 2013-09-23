#ifndef _TF_LOG_H_
#define _TF_LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>

class TFLog
{
public:

	enum LogLevel
	{
		Debug,///< 指出细粒度信息事件对调试应用程序是非常有帮助的
		info,///< 表明 消息在粗粒度级别上突出强调应用程序的运行过程
		warning,///< 表明会出现潜在错误的情形
		error,///< 指出虽然发生错误事件，但仍然不影响系统的继续运行
		fatal///< 指出每个严重的错误事件将会导致应用程序的退出
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
		
		if (timeNow < m_timeLogBegin || timeNow < m_timeLogEnd)
		{
			ClostLog();
			OpenLog(m_logDir.c_str(), m_prefix.c_str());
			if (NULL == m_file)
			{
				return;
			}
		}
		
		tm *pTm = localtime(&timeNow);
		fprintf(m_file, "%02d:%02d:%02d > ", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		va_list ap;
		va_start(ap, fmt)
		vfprintf(m_file , fmt, ap);
		va_end(ap);
		fflush(m_file);
	}
	
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
			ClostLog();
			OpenLog(m_logDir.c_str(), m_prefix.c_str());
			if (NULL == m_file)
			{
				return;
			}
		}
		va_list ap;
		va_start(ap, fmt)
		vfprintf(m_file , fmt, ap);
		va_end(ap);
		fflush(m_file);
	}
	void SetLogLevel(unsigned int level)
	{
		m_logLevel = level
	}
	
	unsigned int GetLogLevel() const
	{
		return m_logLevel;
	}

	
private:
	bool OpenLog(const char *logDir, const char *prefix)
	{
		if (NULL != m_file)
		{
			ClsoeLog();
		}
		
		


	}
	
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
	/// @brief 
	bool MakeFullPath(const char *dir, char *fullPath, int fullPathLen)
	{
		if (fullPathLen < 2)
		{
			return false;
		}

		int pathLen = 0;
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
			char *curDir = filePath
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

	FILE *m_file;
	std::string m_logDir;
	std::string m_prefix;
	time_t m_timeLogBegin;
	time_t m_timeLogEnd;
	unsigned int m_logLevel;
};


#define tfInitLogLib(dir, prefix, logLevel) TFLog g_log(dir, prefix, logLevel);

#define tfImportLogLib extern TFLog g_log;

#define tfLog g_Log.Log

#endif // _TF_LOG_H_