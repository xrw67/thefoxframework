#include <log/log_syslog.h>

using namespace thefox;

static LogSyslog *g_logSyslog = NULL;

void THEFOX_SET_LOG_STDOUT(const string& basename, const net::InetAddress &serverAddr)
{
	if (g_logSyslog) {
		delete g_logSyslog;
		g_logSyslog = NULL;
	}
	g_logSyslog = new LogSyslog(basename, serverAddr);
}

void logSyslogAppend(const string &message)
{
	if (g_logSyslog) {
		g_logSyslog->append(message);
	}
}

LogSyslog::LogSyslog(const string& basename, const net::InetAddress &serverAddr)
	: _basename(basename)
	, _serverAddr(serverAddr)

{
	setLogHandler(std::bind(&LogSyslog::append, this, _1));
}

LogSyslog::~LogSyslog()
{
}

void LogSyslog::append(const string &message)
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