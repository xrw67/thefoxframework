#include <log/Logger.h>

namespace thefox
{

Logger::LogLevel initLogLevel()
{
    return Logger::TRACE;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",
};

// helper class for known string length at compile time
class T
{
public:
	T(const char* str, unsigned len)
		: _str(str)
		, _len(len)
	{
		assert(strlen(_str) == _len);
	}

	const char* _str;
	const unsigned _len;
};

inline LogStream& operator<<(LogStream& s, T v)
{
  s.append(v._str, v._len);
  return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
  s.append(v._data, v._size);
  return s;
}

void defaultOutput(const char* msg, int len)
{
	size_t n = fwrite(msg, 1, len, stdout);
}

void defaultFlush()
{
	fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

}

using namespace thefox;

Logger::LineImpl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
	: _time(Timestamp::now())
	, _stream()
	, _level(level)
	, _line(line)
	, _basename(file)
{
  _stream << T(_time.toFormatString.cStr(), 26);
  _stream << T(CurrentThread::tidString(), 6);
  _stream << T(LogLevelName[level], 6);
  if (savedErrno != 0)
  {
    _stream << strerror(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::finish()
{
	stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line)
  : impl_(INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
	: impl_(level, 0, file, line)
{
	impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
	: impl_(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
	: impl_(toAbort ? FATAL : ERROR, errno, file, line)
{
}

Logger::~Logger()
{
	_lineImpl.finish();
	const LogStream::Buffer& buf(stream().buffer());
	g_output(buf.data(), buf.length());
	if (_lineImpl._level == FATAL)
	{
		g_flush();
		abort();
	}
}

void Logger::setLogLevel(Logger::LogLevel level)
{
	g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
	g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
	g_flush = flush;
}
