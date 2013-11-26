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

namespace thefox
{

class Socket;

class TcpConnection
{
public:
	TcpConnection(const String &name,
	 			SOCKET socket, 
				const InetAddress &localAddr,
				const InetAddress &peerAddr);
	~TcpConnection();
	
	void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback = cb; }
	void setMessageCallback(const MessageCallback &cb) { _messageCallback = cb; }
	void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback = cb; }
	void setCloseCallback(const CloseCallback &cb){ _closeCallback = cb; }
	
	void handleIoProcess();
private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handleRead(IoBuffer *ioBuffer, Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
    
	typedef std::map<IoBuffer *> IoBufferMap;

	scoped_ptr<Socket> _socket;
	String _name;
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;

	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	CloseCallback _closeCallback;
	
	uint32_t _numberOfPendingIoBuffer;

	Buffer _readBuffer;

	uint32_t _readSequenceNumber;
	uint32_t _currentReadSequenceNumber;
	IoBufferMap _readBufferMap;

	uint32_t _sendSequenceNumber;
	uint32_t _currentSendSequenceNumber;
	IoBufferMap _sendBufferMap;

	static MutexLock freeIoBufferLock;
	static std::list<IoBuffer *> freeIoBuffers;
};

}

#endif // _THEFOX_NET_TCPCONNECTION_H_