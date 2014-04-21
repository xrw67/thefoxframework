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
#include <time.h>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <base/MutexLock.h>
#include <base/scoped_ptr.h>

namespace thefox
{


class LogFile : noncopyable
{
public:
	class File;

	LogFile(const String &dir,
            const String &basename,
			size_t rollSize,
			bool threadSafe = true
			);
	~LogFile();

	void append(const char* logline, int len);
	void flush();

private:
	void append_unlocked(const char* logline, int len);

	static String getLogFileName(const String &dir, const String& basename, time_t* now);
	void rollFile();
    void makePath(String &dir);

	String _dir;
	const String _basename;
	const size_t _rollSize;///< 文件缓冲中达到这么多字节的数据后换一个文件

	scoped_ptr<MutexLock> _mutex;
	time_t _startOfPeriod;
	time_t _lastRoll;
	scoped_ptr<File> _file;

	static const int _kRollPerSeconds = 86400;
};

}

#endif // _THEFOX_LOGFILE_H_