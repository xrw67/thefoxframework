#ifndef _THEFOX_PARAMLIST_H_
#define _THEFOX_PARAMLIST_H_

#include <base/types.h>
#include <base/copyable.h>

namespace thefox
{

class ParamList : copyable
{
public:
	typedef std::map<String, String> ParamListType;

	ParamList(const ParamList &list)
		: _list(list)
	{
	}
	const ParamList &operator=(const ParamList &list)
	{
		_list = list;
		return *this;
	}

	const bool find(const String &key)
	{
		return std::map::end != _list.find(key);
	}
	const String get(const String &key)
	{
		return _list[key];
	}
	const int getInt(const String &key)
	{
	
	}

	void set(const String &key, const String &value)
	{
		_list[key] = value;
	}
private:
	ParamListType _list;
};

inline void swap(ParamList &a, ParamList &b)
{
	ParamList temp(a);
	a = b;
	b = temp;
}

}

#endif // _THEFOX_PARAMLIST_H_