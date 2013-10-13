#ifndef _THEFOX_PROCESSINFO_H_
#define _THEFOX_PROCESSINFO_H_

namespace thefox 
{

class ProcessInfo : noncopyable
{
public:
	ProcessInfo() {}
	~ProcessInfo() {}
    const int pid() const { return static_cast<int>(GetCurrentProcessId()); }
        
    
};

} // namespace thefox

#endif // _THEFOX_PROCESSINFO_H_
