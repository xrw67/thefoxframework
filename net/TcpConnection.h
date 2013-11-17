/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/noncopyable.h>
#include <base/Mutexlock.h>
#include <base/scoped_ptr.h>
#include <net/winapi.h>
#include <net/Buffer.h>
#include <net/IoContext.h>
#include <net/InetAddress.h>

class IoCompletionPort;
class Socket;

namespace thefox
{

class TcpConnection : noncopyable
{
public:
	TcpConnection(const String &name,
	 			SOCKET socket, 
				const InetAddress &localAddr,
				const InetAddress &peerAddr);
	~TcpConnection();
	
private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	
	void postRecv(IoContext *ioContext);
	
	scoped_ptr<Socket> _socket;
	IoCompletionPort * const _iocpPtr;
	String _name;
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	std::vector<IoContext *> _ioConetxts;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	CloseCallback closeCallback_;
	Buffer _inBuffer;
	Buffer _outBuffer;

};

}

#endif // _THEFOX_NET_TCPCONNECTION_H_