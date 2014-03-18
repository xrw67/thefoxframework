#ifndef _THEFOX_MEMORY_H_
#define _THEFOX_MEMORY_H_

namespace thefox
{

#define SAFE_DELETE(p) \
	delete p;\
	p = NULL;
	
#define SAFE_DELETE_ARRAY(p) \
	delete []p;\
	p = NULL;

} // namespace thefox

#endif // _THEFOX_MEMORY_H_