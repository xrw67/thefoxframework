#include <log/log_file.h>
#include <base/common.h>
#include <base/files.h>

using namespace thefox;

static LogFile *g_logFile = NULL;

void THEFOX_SET_LOG_FILE(const string &dir, const string &basename, size_t rollSize)
{
	if (g_logFile) {
		delete g_logFile;
		g_logFile = NULL;
	}
	g_logFile = new LogFile(dir, basename, rollSize);
}

void logFileAppend(const string &message)
{
	if (g_logFile) {
		g_logFile->append(message);
	}
}

// not thread safe
class LogFile::File
{
public:
	explicit File(const string& filename)
		: _fp(fopen(filename.data(), "a"))
		, _writtenBytes(0)
	{
		setvbuf(_fp, _buffer, _IOFBF, sizeof(_buffer));
	}

	~File()
	{
		fclose(_fp);
	}

	void append(const char* logline, const size_t len)
	{
		size_t n = write(logline, len);
		size_t remain = len - n;
		while (remain > 0) {
			size_t x = write(logline + n, remain);
			if (x == 0) {
				int err = ferror(_fp);
				if (err) {
					//TODO::错误处理
				}
				break;
			}
			n += x;
			remain = len - n;
		}

		_writtenBytes += len;
	}

	void flush()
	{
		fflush(_fp);
	}

	size_t writtenBytes() const { return _writtenBytes; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(File);
	size_t write(const char* logline, size_t len)
	{
		return ::fwrite(logline, 1, len, _fp);
	}

	FILE* _fp;
	char _buffer[64*1024];
	size_t _writtenBytes;
};

LogFile::LogFile(const string &dir, const string& basename, size_t rollSize)
	: _dir(dir)
	, _basename(basename)
	, _rollSize(rollSize)
	, _startOfPeriod(0)
	, _lastRoll(0)
	, _file(NULL)
{
	Files::makePath(_dir);
	rollFile();
	thefoxSetLogHandler(logFileAppend);
}

LogFile::~LogFile()
{
	thefoxSetLogHandler(NULL);

	if (NULL != _file)
		delete _file;
}

void LogFile::append(const string &message)
{
	MutexGuard lock(_mutex);
	_file->append(message.c_str(), message.length());
	_file->append("\n", 1);

	if (_file->writtenBytes() > _rollSize) {
		rollFile();
	} else {
		time_t now = ::time(NULL);
		time_t _thisPeriod = now / _kRollPerSeconds * _kRollPerSeconds;
		if (_thisPeriod != _startOfPeriod)
			rollFile();
		else 
			_file->flush();
	}
}

void LogFile::flush()
{
	MutexGuard lock(_mutex);
	_file->flush();
}

void LogFile::rollFile()
{
	time_t now = 0;
	string filename = getLogFileName(_dir, _basename, &now);
	time_t start = now / _kRollPerSeconds * _kRollPerSeconds;

	if (now > _lastRoll) {
		_lastRoll = now;
		_startOfPeriod = start;
		if (NULL != _file)
			delete _file;
		_file = new File(filename);
	}
}

string LogFile::getLogFileName(const string &dir, const string& basename, time_t *now)
{
    string filename;
    
    filename.reserve(dir.size() + basename.size() + 64);
    filename = dir + basename;

    char timebuf[32];
    char pidbuf[32];
  
    *now = time(NULL);
    tm *tm_time = localtime(now);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", tm_time);
    filename += timebuf;

    snprintf(pidbuf, sizeof pidbuf, ".%d", 
#ifdef WIN32
    ::GetCurrentProcessId()
#else
    getpid()
#endif
    );

    filename += pidbuf;
    filename += ".log";

    return filename;
}
