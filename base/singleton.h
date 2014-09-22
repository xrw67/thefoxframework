/**
* @filename singleton.h
* @brief 单例模式
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_BASE_SINGLETION_H_
#define _THEFOX_BASE_SINGLETION_H_

namespace thefox {

template <typename T>
class singleton : private T
{
public:
	static T &instance()
	{
		static singleton<T> s_oT;
		return s_oT;
	}

private:
	singleton() {}
	~singleton() {}
};

} // namespace thefox

#endif // _THEFOX_BASE_SINGLETION_H_