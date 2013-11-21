#ifndef _THEFOX_TIME_H_
#define _THEFOX_TIME_H_

#include <time.h>
#include <base/Types.h>
#include <base/copyable.h>

namespace thefox
{

class Time : copyable
{
public:
	static Time now() { return ::time(0); }
	Time()
		: _timestamp(0)
	{}
	Time(time_t t)
		: _timestamp(t)
	Time(const String &formatTime)
	{
		getFromFormat(formatTime);
	}
	Time (const Time &t)
		: _timestamp(t._timestamp)
	{}
	~Time()
	{}
	
	/// 格式必须是"yyyy-mm-dd HH:MM:SS"
	bool getFormatString(const String &formatStr)
	{
		if (formatStr.size() != 19)
		{
			return false;
		}
		
		tm tm_time;
		memset(tm_time, 0, sizeof(tm_time);
		
		tm_time.tm_year = atoi(formatStr.substr(0, 4).c_str()) - 1900;
		tm_time.tm_mon = atoi(formatStr.substr(5, 2).c_str()) - 1;
		tm_time.tm_mday = atoi(formatStr.substr(8, 2).c_str());
		tm_time.tm_hour = atoi(formatStr.substr(11, 2).c_str());
		tm_time.tm_min = atoi(formatStr.substr(14, 2).c_str());
		tm_time.tm_sec = atoi(formatStr.substr(17, 2).c_str());
	
		_timestamp = mktime(&tm_time)
	}
	
	String toFormatString()
	{
		char buf[32] = {0};
		tm *tm_time = localtime(&_timestamp);
		_snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
			tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
			tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
		return buf;
	}
	
	time_t timestamp() const { return _timestamp; }
	
	bool valid() const { return _timestamp > 0; }
	
	time_t()
	{
		return _timestamp;
	}
	
private:
	time_t _timestamp;
};

}

#endif // _THEFOX_TIME_H_