/*
*
*
*
*/

#ifndef _TF_TIME_H_
#define _TF_TIME_H_

class TFTime
{
public:
	TFTime()
	{}
	TFTime(const time_t t)
	{}
	TFTime(const TFTime &time)
	{}
	const TFTime &operator=(const TFTime &)
	{
	}
	
private:
	uint64 m_msTimestamp;
};

#endif // _TF_TIME_H_