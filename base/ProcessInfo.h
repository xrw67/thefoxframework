#ifndef _TF_PROCESSINFO_H_
#define _TF_PROCESSINFO_H_

namespace thefox
{
class ProcessInfo : noncopyable
{
public:
	ProcessInfo() {}
	~ProcessInfo() {}
    const int pid() const { return static_cast<int>(GetCurrentProcessId()); }
        
    
};

}

#endif // _TF_PROCESSINFO_H_
