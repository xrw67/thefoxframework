#ifndef _THEFOX_STRONG_PTR_H_
#define _THEFOX_STRONG_PTR_H_

namespace thefox
{

template <typename T>
class StrongPtr
{
public:
	StrongPtr();
	StringPtr(
	StrongPtr(const StrongPtr &ptr)
	Strong &operator=(StrongPtr *ptr);

private:
	T *_value;
	long _refCount;
};

} // namespace

#endif // _THEFOX_STRONG_PTR_H_