/*
* 从boost中引用
*/
#ifndef _THEFOX_NONCOPYABLE_H_
#define _THEFOX_NONCOPYABLE_H_

namespace thefox 
{

class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}
private:  // emphasize the following members are private
	noncopyable( const noncopyable& );
	const noncopyable& operator=( const noncopyable& );
};

typedef thefox::noncopyable noncopyable;

} // namespace thefox

#endif // _THEFOX_NONCOPYABLE_H_
