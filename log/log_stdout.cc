#include <log/log_stdout.h>

using namespace thefox;

static LogStdout *g_logStdout = NULL;

void THEFOX_SET_LOG_STDOUT(const string& basename)
{
	if (g_logStdout) {
		delete g_logStdout;
		g_logStdout = NULL;
	}
	g_logStdout = new LogStdout(basename);
}

void logStdoutAppend(const string &message)
{
	if (g_logStdout) {
		g_logStdout->append(message);
	}
}

LogStdout::LogStdout(const string& basename)
	: _basename(basename)
{
	thefoxSetLogHandler(logStdoutAppend);
}

LogStdout::~LogStdout() 
{
	thefoxSetLogHandler(NULL);
}

void LogStdout::append(const string &message)
{
	MutexGuard lock(_mutex);
	if (message.empty()) {
		std::cout << message << '\n';
	} else {
		std::cout << _basename << " " << message << '\n';
	}
}