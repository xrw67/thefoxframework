#ifndef _THEFOX_OBJECT_H_
#define _THEFOX_OBJECT_H_

namespace thefox
{

template<typename T>
class Object
{
public:
	Object()
		:_marked(false)
	{}
	
	void mark() 
	{
		if (!_marked)
			_marked = true;
	}
	const bool isMarked() const { return _marked; }
private:
	T _t;
	bool _marked;
};

} // namespace thefox
#endif // _THEFOX_OBJECT_H_