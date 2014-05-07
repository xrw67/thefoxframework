/*
* @file LogFile.h
* @brief log to file
*
* @ahthor macwe@qq.com
*/

#ifndef _THEFOX_LOGFILE_H_
#define _THEFOX_LOGFILE_H_

#ifdef WIN32
#else
#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <log/Logging.h>

namespace thefox
{


class LogFile
{
public:
	class File;

	LogFile(const std::string &dir,
            const std::string &basename,
			size_t rollSize = 100*1024*1024);
	~LogFile();

	void append(const std::string &message);
	void flush();

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(LogFile);

	static std::string getLogFileName(const std::string &dir, const std::string& basename, time_t* now);
	void rollFile();
    void makePath(std::string &dir);

	std::string _dir;
	const std::string _basename;
	size_t _rollSize;
	
	MutexLock _mutex;
	time_t _startOfPeriod;
	time_t _lastRoll;
	std::unique_ptr<File> _file;

	static const int _kRollPerSeconds = 86400;
};

}

#endif // _THEFOX_LOGFILE_H_
