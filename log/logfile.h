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
#include <base/scoped_ptr.h>

namespace thefox
{


class LogFile : noncopyable
{
public:
	LogFile(const string &dir,
            const string &basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3);
	~LogFile();

	void append(const char* logline, int len);
	void flush();

private:
	void append_unlocked(const char* logline, int len);

	static string getLogFileName(const string &dir, const string& basename, time_t* now);
	void rollFile();
    void makePath(string &dir);

	string _dir;
	const string _basename;
	const size_t _rollSize;///< 文件缓冲中达到这么多字节的数据后写到文件里
	const int _flushInterval;///< 每隔这个时间将文件缓冲区中的数据写到文件中

	int _count;

	scoped_ptr<MutexLock> _mutex;
	time_t _startOfPeriod;
	time_t _lastRoll;
	time_t _lastFlush;
	class File;
	scoped_ptr<File> _file;

	const static int _kCheckTimeRoll = 1024; ///< 统计达到这么多行后写到文件
	const static int _kRollPerSeconds = 60*60*24;
};

}

#endif // _THEFOX_LOGFILE_H_