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
	TcpConnection(const string &name, SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr);
	~TcpConnection();
	
private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	  
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
	string name;
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	std::list<IoContextPtr> _ioConetxts;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	CloseCallback closeCallback_;
	Buffer _inBuffer;
	Buffer _outBuffer

};
}

#endif // _THEFOX_NET_TCPCONNECTION_H_