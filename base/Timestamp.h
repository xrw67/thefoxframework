/*
* @filename Timestamp.h
* @brief 时间戳，精确到1微妙,格式：20131001 08:30:00.0000000
* @author macwe@qq.com
*/

#ifndef _THEFOX_BASE_TIMESTAMP_H_
#define _THEFOX_BASE_TIMESTAMP_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <time.h>
#include <base/Types.h>

namespace thefox
{

class Timestamp
{
public:
    Timestamp()
        : _usTimestamp(0)
    {}
    Timestamp(int64_t usTimestamp)
        : _usTimestamp(usTimestamp)
    {}
    Timestamp(const Timestamp &ts)
        : _usTimestamp(ts._usTimestamp)
    {}

    std::string toString()
    {
        char buf[32] = {0};
        int64_t seconds = _usTimestamp / kMicroSecondsPerSecond;
        int64_t microseconds = _usTimestamp % kMicroSecondsPerSecond;
        snprintf(buf, sizeof(buf), "%lld.%06lld", seconds, microseconds);
        return buf;
    }

    std::string toFormatString()
    {
        char buf[32] = {0};
        time_t seconds = static_cast<time_t>(_usTimestamp / kMicroSecondsPerSecond);
        int microseconds = static_cast<int>(_usTimestamp % kMicroSecondsPerSecond);
        tm *tm_time = localtime(&seconds);

        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
            tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
            tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec,
            microseconds);
        return buf;
    }

    time_t toUnixTimestamp() const 
	{ return static_cast<time_t>(_usTimestamp / kMicroSecondsPerSecond); }
	
    int64_t timestamp() const { return _usTimestamp; }

    bool valid() const { return _usTimestamp > 0; }

#ifdef WIN32
    LARGE_INTEGER toLargeInt() const 
    {
        LARGE_INTEGER li;
        li.QuadPart = (_usTimestamp * 10) + 116444736000000000;
        return li;
    }
#endif

    static Timestamp now()
    {
#ifdef WIN32
        FILETIME ft;
        ULARGE_INTEGER ui;  
        GetSystemTimeAsFileTime(&ft); // 精确到100ns
        ui.LowPart = ft.dwLowDateTime;  
        ui.HighPart = ft.dwHighDateTime;
        return Timestamp(static_cast<int64_t>(ui.QuadPart - 116444736000000000) / 10);
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
        int64_t seconds = tv.tv_sec;
        return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
#endif
    }

    static Timestamp invalid() { return Timestamp(); }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t _usTimestamp;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.timestamp() < rhs.timestamp();
}

inline bool operator<=(Timestamp lhs, Timestamp rhs)
{
    return lhs.timestamp() <= rhs.timestamp();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.timestamp() == rhs.timestamp();
}

} // namespace thefox

#endif // _THEFOX_BASE_TIMESTAMP_H_
