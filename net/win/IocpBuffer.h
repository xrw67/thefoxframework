#ifndef _THEFOX_NET_WIN_IOCP_BUFFER_H_
#define _THEFOX_NET_WIN_IOCP_BUFFER_H_

#ifndef WIN32
#else

#include <map>
#include <base/Types.h>

nameapace thefox
{

namespace net
{

enum OperationType
{
	IoInitialize,
	IoRead, 
	IoReadCompleted,
	IoWrite,
	IoWriteCompleted,
	IoZeroByteRead, // 为了避免Blocking错误
	IoZeroReadCompleted,
};

struct IocpBuffer
{
public:
	OVERLAPPED _overlapped;
	WSABUF _wasBuf;
	
	IocpBuffer()
	{


	}
	~IocpBuffer()
	{

	}

	int uint32_t sequence() const { return _sequence; }
private:
	static const int kMaxBufSize = 8192;

	char _buffer[kMaxBufSize];
	OperationType _operation;

	uint32_t _sequence; // 序号
};

typedef IocpBuffer *IocpBufferPtr;
typedef std::map<uint32_t, IocpBufferPtr> IocpBufferMap;

} // namespace net

} // namespace thefox

#endif // WIN32

#endif // _THEFOX_NET_WIN_IOCP_BUFFER_H_