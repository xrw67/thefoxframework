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

namespace thefox
{

class LogStdout
{
public:
	LogStdout(const string& basename = "")
		: _basename(basename)
	{
		setLogHandler(std::bind(&LogStdout::append, this, std::placeholders::_1));
	}
	~LogStdout() {}

	void append(const string &message)
	{
		if (message.empty()) {
			std::cout << message << '\n';
		} else {
			std::cout << _basename << " " << message << '\n';
		}
	}
	
private:
	const string _basename;
};

} // namespace thefox

#endif // _THEFOX_LOGSYSLOG_H_
