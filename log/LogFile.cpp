#include <log/LogFile.h>
#include <base/noncopyable.h>
#include <base/ProcessInfo.h>

using namespace thefox;

// not thread safe
class LogFile::File : noncopyable
{
public:
	explicit File(const string& filename)
		: _fp(::fopen(filename.data(), "ae"))
		, _writtenBytes(0)
	{
		assert(fp_);
		::setbuffer(_fp, _buffer, sizeof(_buffer));
	}

	~File()
	{
		::fclose(_fp);
	}

	void append(const char* logline, const size_t len)
	{
		size_t n = write(logline, len);
		size_t remain = len - n;
		while (remain > 0)
		{
			size_t x = write(logline + n, remain);
			if (x == 0)
			{
				int err = ferror(_fp);
				if (err)
				{
					TRACE("LogFile::File::append() failed \r\n");
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
		::fflush(_fp);
	}

	size_t writtenBytes() const { return _writtenBytes; }

private:
	size_t write(const char* logline, size_t len)
	{
		return ::fwrite(logline, 1, len, _fp);
	}

  FILE* _fp;
  char _buffer[64*1024];
  size_t _writtenBytes;
};


LogFile::LogFile(const string& basename, size_t rollSize, bool threadSafe, int flushInterval)
	: basename_(basename)
	, rollSize_(rollSize)
	, flushInterval_(flushInterval)
	, count_(0)
	, mutex_(threadSafe ? new MutexLock : NULL)
	, startOfPeriod_(0)
	, lastRoll_(0)
	, lastFlush_(0)
{
	assert(basename.find('/') == string::npos);
	rollFile();
}

LogFile::~LogFile()
{
}

void LogFile::append(const char* logline, int len)
{
	if (_mutex)
	{
		MutexLockGuard lock(*_mutex);
		append_unlocked(logline, len);
	}
	else
	{
		append_unlocked(logline, len);
	}
}

void LogFile::flush()
{
	if (_mutex)
	{
		MutexLockGuard lock(*_mutex);
		_file->flush();
	}
	else
	{
		_file->flush();
	}
}

void LogFile::append_unlocked(const char* logline, int len)
{
	_file->append(logline, len);

	if (file_->writtenBytes() > _rollSize)
	{
		rollFile();
	}
	else
	{
		if (_count > _kCheckTimeRoll)
		{
			_count = 0;
			time_t now = ::time(NULL);
			time_t _thisPeriod = now / _kRollPerSeconds * _kRollPerSeconds;
			if (_thisPeriod != _startOfPeriod)
			{
				rollFile();
			}
			else if (now - _lastFlush > _flushInterval)
			{
				_lastFlush = now;
				_file->flush();
			}
		}
		else
		{
			++_count;
		}
	}
}

void LogFile::rollFile()
{
	time_t now = 0;
	string filename = getLogFileName(_basename, &now);
	time_t start = now / _kRollPerSeconds * _kRollPerSeconds;

	if (now > _lastRoll)
	{
		_lastRoll = now;
		_lastFlush = now;
		_startOfPeriod = start;
		file_.reset(new File(filename));
	}
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
  string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  char pidbuf[32];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm); // FIXME: localtime_r ?
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;
  snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
  filename += pidbuf;
  filename += ".log";

  return filename;
}

