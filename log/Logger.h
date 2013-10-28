#ifndef _THEFOX_LOGGER_H_
#define _THEFOX_LOGGER_H_

#pragma warning(disable:4996)

#include <log/LogStream.h>
#include <base/Timestamp.h>

namespace thefox
{

class Logger
{
public:
	enum LogLevel
	{
		LOGLEVEL_TRACE,
		LOGLEVEL_DEBUG,
		LOGLEVEL_INFO,
		LOGLEVEL_WARN,
		LOGLEVEL_ERROR,
		LOGLEVEL_FATAL,
		LOGLEVEL_NUM_LOG_LEVELS
	};

	// 在编译期计算原文件名
	class SourceFile
	{
	public:
		template<int N>
		SourceFile(char (&arr)[N])
			: _data(arr)
			, _size(N-1)
		{
			char* slash = strrchr(_data, '/'); // builtin function
			if (0 == slash)
			{
				slash = strrchr(_data, '\\');
			}
			if (slash)
			{
				_data = slash + 1;
				_size -= static_cast<int>(_data - arr);
			}
		}

		explicit SourceFile(char* filename)
			: _data(filename)
		{
			const char* slash = strrchr(filename, '/');
			if (0 == slash)
			{
				slash = strrchr(_data, '\\');
			}
			if (slash)
			{
				_data = slash + 1;
			}
			_size = static_cast<int>(strlen(_data));
		}

		const char *_data;
		int _size;
	};

	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char* func);
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
		LineImpl(LogLevel level, const SourceFile& file, int line);
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

#define LOG_TRACE if (thefox::Logger::logLevel() <= thefox::Logger::LOGLEVEL_TRACE) \
  thefox::Logger(thefox::Logger::SourceFile(__FILE__), __LINE__, thefox::Logger::LOGLEVEL_TRACE, __FUNCTION__).stream()
#define LOG_DEBUG if (thefox::Logger::logLevel() <= thefox::Logger::LOGLEVEL_DEBUG) \
  thefox::Logger(thefox::Logger::SourceFile(__FILE__), __LINE__, thefox::Logger::LOGLEVEL_DEBUG, __FUNCTION__).stream()
#define LOG_INFO if (thefox::Logger::logLevel() <= thefox::Logger::LOGLEVEL_INFO) \
  thefox::Logger(thefox::Logger::SourceFile(__FILE__), __LINE__).stream()
#define LOG_WARN thefox::Logger(thefox::Logger::SourceFile(__FILE__), __LINE__, thefox::Logger::LOGLEVEL_WARN).stream()
#define LOG_ERROR thefox::Logger(thefox::Logger::SourceFile(__FILE__), __LINE__, thefox::Logger::LOGLEVEL_ERROR).stream()
#define LOG_FATAL thefox::Logger(thefox::Logger::SourceFile(__FILE__), __LINE__, thefox::Logger::LOGLEVEL_FATAL).stream()


}

#endif // _THEFOX_LOGGER_H_