#ifndef _THEFOX_LOGGER_H_
#define _THEFOX_LOGGER_H_

#include <log/LogStream.h>
#include <base/Timestamp.h>

namespace thefox
{

class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERR,
		FATAL,
		NUM_LOG_LEVELS
	};

	// 在编译期计算原文件名
	class SourceFile
	{
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N])
			: data_(arr)
			, size_(N-1)
		{
			const char* slash = strrchr(_data, '/'); // builtin function
			if (slash)
			{
				_data = slash + 1;
				_size -= static_cast<int>(_data - arr);
			}
		}

		explicit SourceFile(const char* filename)
			: _data(filename)
		{
			const char* slash = strrchr(filename, '/');
			if (slash)
			{
				_data = slash + 1;
			}
			_size = static_cast<int>(strlen(_data));
		}

		const char* _data;
		int _size;
	};

	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	LogStream& stream() { return _lineImpl._stream; }

	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char* msg, int len);
	typedef void (*FlushFunc)();
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);

private:
	class LineImpl
	{
	public:
		typedef Logger::LogLevel LogLevel;
		LineImpl(LogLevel level, int old_errno, const SourceFile& file, int line);
		void finish();

		Timestamp _time;
		LogStream _stream;
		LogLevel _level;
		int _line;
		SourceFile _basename;
	};

  LineImpl _lineImpl;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
	return g_logLevel;
}

#define LOG_TRACE if (thefox::Logger::logLevel() <= thefox::Logger::TRACE) \
  thefox::Logger(__FILE__, __LINE__, thefox::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (thefox::Logger::logLevel() <= thefox::Logger::DEBUG) \
  thefox::Logger(__FILE__, __LINE__, thefox::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (thefox::Logger::logLevel() <= thefox::Logger::INFO) \
  thefox::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN thefox::Logger(__FILE__, __LINE__, thefox::Logger::WARN).stream()
#define LOG_ERROR thefox::Logger(__FILE__, __LINE__, thefox::Logger::ERROR).stream()
#define LOG_FATAL thefox::Logger(__FILE__, __LINE__, thefox::Logger::FATAL).stream()
#define LOG_SYSERR thefox::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL thefox::Logger(__FILE__, __LINE__, true).stream()

}

#endif // _THEFOX_LOGGER_H_