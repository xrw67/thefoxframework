/*
* @file log_syslog.h
* @brief log to syslog server
*
* @ahthor macwe1024 at gmail dot com
*/

#ifndef _THEFOX_LOGSYSLOG_H_
#define _THEFOX_LOGSYSLOG_H_

#include <iostream>
#include <log/logging.h>
#include <base/common.h>

namespace thefox {

class LogStdout
{
public:
	explicit LogStdout(const string& basename);
	~LogStdout();

	void append(const string &message);
	
private:
	const string _basename;
};

} // namespace thefox

void THEFOX_SET_LOG_STDOUT(const thefox::string &basename = "");

#endif // _THEFOX_LOGSYSLOG_H_
