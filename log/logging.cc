#include <log/logging.h>
#include <algorithm>
#include <base/timestamp.h>
#include <base/thread_util.h>

namespace thefox
{
namespace detail
{
// 日志级别
const char* LogLevelName[LOGLEVEL_NUM_LOG_LEVELS] =
{
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

LogLevel initLogLevel()
{
    return LOGLEVEL_TRACE;
}

//默认的日志输出函数
void nullLogHandler(const std::string &message)
{
	// Nothing.
}

static LogLevel g_logLevel = initLogLevel();
static LogHandler g_logHandler = nullLogHandler;

// 高效的整型转字符串算法 by Matthew Wilson.
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
const char digitsHex[] = "0123456789ABCDEF";

template<typename T>
inline size_t convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

	do {
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0)
		*p++ = '-';
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

inline size_t convertHex(char buf[], uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do {
		int lsd = static_cast<int>(i % 16);
		i /= 16;
		*p++ = digitsHex[lsd];
	} while (i != 0);

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

LogMessage::LogMessage(LogLevel level, const char *filename, int line)
	: _level(level)
{
	*this << "[" 
		<< Timestamp::now().toFormatString() << " "
		<< currentThreadId() << " "
		<< LogLevelName[level] << " " 
		<< filename << ":" << line 
		<< "] ";
}

LogMessage::~LogMessage()
{}

LogMessage &LogMessage::operator<<(const std::string &value)
{
	_message += value;
	return *this;
}

LogMessage &LogMessage::operator<<(const char* value)
{
	_message += value;
	return *this;
}

#undef DECLARE_STREAM_FORMAT_INTEGER
#define DECLARE_STREAM_FORMAT_INTEGER(TYPE)                \
	LogMessage& LogMessage::operator<<(TYPE value)         \
	{                                                      \
		char buffer[128];                                  \
		convert<TYPE>(buffer, value);     \
		buffer[sizeof(buffer) - 1] = '\0';                 \
		_message += buffer;                                \
		return *this;                                      \
	}

DECLARE_STREAM_FORMAT_INTEGER(char)
DECLARE_STREAM_FORMAT_INTEGER(int)
DECLARE_STREAM_FORMAT_INTEGER(unsigned int)
DECLARE_STREAM_FORMAT_INTEGER(long)
DECLARE_STREAM_FORMAT_INTEGER(unsigned long)
DECLARE_STREAM_FORMAT_INTEGER(long long)
DECLARE_STREAM_FORMAT_INTEGER(unsigned long long)
#undef DECLARE_STREAM_OPERATOR

LogMessage &LogMessage::operator<<(double value)
{
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%g", value);
	buffer[sizeof(buffer) - 1] = '\0';
	_message += buffer;
	return *this;
}

LogMessage &LogMessage::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	char buffer[128];
	buffer[0] = '0';
	buffer[1] = 'x';
	convertHex(buffer + 2, v);
	buffer[sizeof(buffer) - 1] = '\0';
	_message += buffer;
	return *this;
}

void LogMessage::Finish() 
{
	g_logHandler(_message);

	if (LOGLEVEL_FATAL == _level)
		abort();
}

void LogFinisher::operator=(LogMessage& other)
{
	other.Finish();
}

LogTraceFunction::LogTraceFunction(const char *func, const char *file, int line)
	: _func(func)
	, _file(getFileNameFromPath(file))
	, _line(line)
{}

LogTraceFunction::~LogTraceFunction()
{ 
	if (thefox::logLevel() <= LOGLEVEL_TRACE)
		*this = LogMessage(LOGLEVEL_TRACE, _file, _line) 
							<< _func << "() end";
}

void LogTraceFunction::operator=(LogMessage& other)
{
	other.Finish();
}

} // namespace detail

// 设置日志写函数
void setLogHandler(const LogHandler &newFunc)
{
	detail::g_logHandler = newFunc;
}

// 设置日志级别
void setLogLevel(LogLevel level)
{
	detail::g_logLevel = level;
}

LogLevel logLevel()
{
	return detail::g_logLevel;
}

} // namespace thefox
