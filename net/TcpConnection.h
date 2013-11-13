/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/noncopyable.h>
#include <base/Mutexlock.h>

#include <net/IoContext>

namespace thefox
{

class TcpConnection : noncopyable
{
public:
	TcpConnection();
	~TcpConnection();
	
private:
	typedef IoContext *IoContextPtr;
	
	IoContextPtr createNewIoContext()
	{ 
		MutexLockGuard lock(_lock);
		IoContextPtr io = new IoContext;
		_ioContexts.push_back(io);
		return io;
	}
	
	bool removeIoContext(IoContextPtr io)
	{
		MutexLockGuard lock(_lock);
		for (std::list<IoContextPtr>::iterator it = _ioContexts.begin();
				it != _ioContexts.end(); ++it) 
		{
			if (*it == io)
			{
				_ioContext.erase(it);
				delete io;
				io = nullptr;
			}
		}
	}
	
	SOCKET _sock;
	int _connId;
	string name;
	MutexLock _lock;
	std::list<IoContextPtr> _ioConetxts;
	Buffer _inBuffer;
	Buffer _outBuffer

};
}

#endif // _THEFOX_NET_TCPCONNECTION_H_