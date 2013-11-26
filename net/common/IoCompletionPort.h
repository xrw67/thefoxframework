#ifndef _THEFOX_NET_IO_COMPLETIONPORT_H_
#define _THEFOX_NET_IO_COMPLETIONPORT_H_

#include <base/noncopyable.h>
#include <net/winapi.h>

namespace thefox
{

class IoCompletionPort : noncopyable
{
public:
	IoCompletionPort()
		: _iocp(INVALID_HANDLE_VALUE)
	{
		_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	}

	~IoCompletionPort()
	{
		if (INVALID_HANDLE_VALUE != _iocp)
			::CloseHandle(_iocp);
	}

	BOOL registerHandle(HANDLE handle, ULONG_PTR completionKey)
	{
		::CreateIoCompletionPort(_iocp, handle, completionKey, 0);
	}

	BOOL getStatus(LPDWORD lpNumberOfBytesTransferred, PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped, DWORD timeout = INFINITE)
	{
		return ::GetQueuedCompletionStatus(_iocp, 
			lpNumberOfBytesTransferred, lpCompletionKey, lpOverlapped, timeout);
	}

	BOOL post(DWORD dwCompletlonKey, LPOVERLAPPED lpoverlapped)
	{
		return ::PostQueuedCompletionStatus(_iocp, 0, dwCompletlonKey, lpoverlapped); 
	}

	HANDLE getHandle() const { return _iocp; }
private:
	HANDLE _iocp;
};

}

#endif // _THEFOX_NET_IO_COMPLETIONPORT_H_