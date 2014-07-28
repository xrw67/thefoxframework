/**
* @file log_stdout.h
* @brief log to stdout
* @ahthor macwe1024 at gmail dot com
*/

#ifndef _THEFOX_LOGSYSLOG_H_
#define _THEFOX_LOGSYSLOG_H_

#include <iostream>
#include <log/logging.h>
#include <base/common.h>
#include <base/mutex.h>

namespace thefox {

class LogStdout
{
public:
	explicit LogStdout(const string& basename);
	~LogStdout();

	void append(const string &message);
	
private:
	Mutex _mutex;
	const string _basename;
};

} // namespace thefox

void THEFOX_SET_LOG_STDOUT(const thefox::string &basename = "");

#endif // _THEFOX_LOGSYSLOG_H_
