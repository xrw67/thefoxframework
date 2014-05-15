#ifndef _THEFOX_BASE_TIME_H_
#define _THEFOX_BASE_TIME_H_

#include <time.h>
#include <base/Types.h>

namespace thefox
{

class TimeUtil
{
public:
	static std::string fromUnixTime(time_t t)
	{
		char buf[32] = {0};
		tm *tm_time = localtime(&t);
		_snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
			tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
			tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
		return buf;
	}

	static time_t unixTimestamp(const std::string &timeStr)
	{
		tm tm_time;
		memset(&tm_time, 0, sizeof(tm_time));
		
		tm_time.tm_year = atoi(timeStr.substr(0, 4).c_str()) - 1900;
		tm_time.tm_mon = atoi(timeStr.substr(5, 2).c_str()) - 1;
		tm_time.tm_mday = atoi(timeStr.substr(8, 2).c_str());
		tm_time.tm_hour = atoi(timeStr.substr(11, 2).c_str());
		tm_time.tm_min = atoi(timeStr.substr(14, 2).c_str());
		tm_time.tm_sec = atoi(timeStr.substr(17, 2).c_str());
		
		return mktime(&tm_time);
	}

#ifndef _WIN32
    static int32_t getAbsTimespec(struct timespec *ts, int32_t millisecond)
	{
        if (NULL == ts)
            return EINVAL;

		struct timeval tv;
        int32_t ret = gettimeofday(&tv, NULL);
        if (0 != ret)
            return ret;

        ts->tv_sec = tv.tv_sec;
        ts->tv_nsec = tv.tv_usec * 1000UL;

        ts->tv_sec += millisecond / 1000UL;
        ts->tv_nsec += millisecond % 1000UL * 1000000UL;

        ts->tv_sec += ts->tv_nsec/(1000UL * 1000UL *1000UL);
        ts->tv_nsec %= (1000UL * 1000UL *1000UL);

        return 0;
    }
#endif
};

} // namespace thefox

#endif // _THEFOX_BASE_TIME_H_