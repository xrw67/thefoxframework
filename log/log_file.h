/*
* @file LogFile.h
* @brief log to file
*
* @ahthor macwe1024 at gmail dot com
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
#include <base/common.h>
#include <base/mutex.h>
#include <log/logging.h>

namespace thefox {

class LogFile
{
public:
	class File;

	LogFile(const string &dir, const string &basename, size_t rollSize);
	~LogFile();

	void init();
	void append(const string &message);
	void flush();

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(LogFile);

	static string getLogFileName(const string &dir, const string& basename, time_t* now);
	void rollFile();

	string _dir;
	const string _basename;
	size_t _rollSize;
	
	Mutex _mutex;
	time_t _startOfPeriod;
	time_t _lastRoll;
	File *_file;

	static const int _kRollPerSeconds = 86400;
};

} // namespace thefox

void THEFOX_SET_LOG_FILE(const thefox::string &dir,
							const thefox::string &basename,
							size_t rollSize = 100*1024*1024);
#endif // _THEFOX_LOGFILE_H_
