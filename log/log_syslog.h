/*
* @file log_syslog.h
* @brief log to syslog server
*
* @ahthor macwe1024 at gmail dot com
*/

#ifndef _THEFOX_LOGSYSLOG_H_
#define _THEFOX_LOGSYSLOG_H_

#ifdef WIN32
#include <Winsock2.h>
#else
#include <sys/socket.h>
#define SOCKET int
#endif

#include <base/common.h>
#include <base/mutex.h>
#include <log/logging.h>
#include <net/inet_address.h>

namespace thefox
{

class LogSyslog
{
public:
	LogSyslog(const std::string& basename, const InetAddress &serverAddr);
	~LogSyslog();

	void append(const std::string &message);
	bool init();
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(LogSyslog);

	const std::string _basename;
	InetAddress _serverAddr;
	SOCKET _sockfd;
	Mutex _mutex;
};

} // namespace thefox

#endif // _THEFOX_LOGSYSLOG_H_
