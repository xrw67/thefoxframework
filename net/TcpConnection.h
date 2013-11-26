/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/noncopyable.h>
#include <base/scoped_ptr.h>
#include <net/winapi.h>
#include <net/Buffer.h>
#include <net/IoBuffer.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/PerSocketContext.h>

namespace thefox
{

class IoCompletionPort;
class Socket;

class TcpConnection : public PerSocketContext
{
public:
	TcpConnection(const String &name,
	 			SOCKET socket, 
				const InetAddress &localAddr,
				const InetAddress &peerAddr);
	~TcpConnection();
	
private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handleRead(IoBuffer *ioBuffer, Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
    
	scoped_ptr<Socket> _socket;
	IoCompletionPort * const _iocpPtr;
	String _name;
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	CloseCallback _closeCallback;
    std::list<scoped_ptr<IoBuffer>> _freeIoConetxts;
	Buffer _inBuffer;
};

}

#endif // _THEFOX_NET_TCPCONNECTION_H_