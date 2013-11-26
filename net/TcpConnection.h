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

#include <boost/shared_ptr.hpp>

namespace thefox
{

class Socket;

class TcpConnection
{
public:
	TcpConnection(TcpServer *server,
				const String &name,
	 			SOCKET socket, 
				const InetAddress &localAddr,
				const InetAddress &peerAddr);
	~TcpConnection();
	
	const String &name() const {return _name; }
	const InetAddress &localAddress() {return _localAddr; }
	const InetAddress &peerAddress() {return _peerAddr; }
	bool connected() const { return _state == kConnected; }
	
	void send(const void *message, size_t len);
	void shotdown();
	void setTcpNoDelay(bool on);
	
	void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback = cb; }
	void setMessageCallback(const MessageCallback &cb) { _messageCallback = cb; }
	void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback = cb; }
	void setCloseCallback(const CloseCallback &cb){ _closeCallback = cb; }
	
	void handleIoProcess(IoBuffer *iobuf, DWORD bytesTransfered);
	
	void connectEstablished();
	void connectDestroyed(); 
	
private:
	typedef std::map<uint32_t, IoBuffer *> IoBufferMap;
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	
	void handleRead(IoBuffer *ioBuffer, Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
    void setState(StatE s) { _state = s; }
	
	inline IoBuffer *getNextReadBuffer(IoBuffer *buf = NULL);
	inline IoBuffer *getNextSendBuffer(IoBuffer *buf = NULL);
	
	
	TcpServer *_server;
	StateE _state;
	Socket _socket;
	String _name;
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	MutexLock _lock;
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	CloseCallback _closeCallback;
	Buffer _inBuffer; // 接收缓存

	uint32_t _numberOfPendingIo;
	// 读队列
	uint32_t _readSequenceNumber;
	uint32_t _currentReadSequenceNumber;
	IoBufferMap _readBufferMap;
	// 发送队列
	uint32_t _sendSequenceNumber;
	uint32_t _currentSendSequenceNumber;
	IoBufferMap _sendBufferMap;
	
	static MutexLock freeIoBufferLock;
	static std::list<IoBuffer *> freeIoBuffers;
};

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
}

#endif // _THEFOX_NET_TCPCONNECTION_H_