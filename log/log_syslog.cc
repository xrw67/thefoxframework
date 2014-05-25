#include <log/log_syslog.h>

using namespace thefox;

LogSyslog::LogSyslog(const std::string& basename, const InetAddress &serverAddr)
	: _basename(basename)
	, _serverAddr(serverAddr)

{
	setLogHandler(std::bind(&LogSyslog::append, this, _1));
}

LogSyslog::~LogSyslog()
{
}

void LogSyslog::append(const std::string &message)
{
	MutexGuard lock(_mutex);
	if (_sockfd >= 0) {
		::sendto(_sockfd, message.c_str(), message.length(), 0, 
				(struct sockaddr *)&_serverAddr.getSockAddrInet(), 
				sizeof(struct sockaddr_in));
	}
}

bool LogSyslog::init()
{
	_sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
	return _sockfd >= 0;
}