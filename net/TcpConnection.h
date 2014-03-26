#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_


#include <base/noncopyable.h>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/Buffer.h>
#include <net/InetAddress.h>


namespace thefox
{

#ifdef WIN32
class IoContext;
#else
typedef int SOCKET
#endif

class TcpConnection : noncopyable
{
public:
	enum StateT { kDisconnected, kConnecting, kConnected, kDisconnecting };

	TcpConnection(SOCKET socket, int connId, const InetAddress &peerAddr)
		: _socket(socket)
		, _connId(connId)
		, _peerAddr(peerAddr)
	{}
	~TcpConnection()
	{
		MutexLockGuard lock(_mutex);
		if (INVALID_SOCKET != _socket) {
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}
	int connId() const { return _connId; }
	SOCKET socket() { return _socket; }
	const InetAddress &getPeerAddr() const { return _peerAddr; }
	Buffer *readBuffer() { return &_readBuffer; }
	Buffer *writeBuffer() { return &_writeBuffer; }
	void appendReadBuffer(const char *data, size_t len) 
	{
		MutexLockGuard lock(_mutex);
		_readBuffer.append(data, len);
	}
	void appendWriteBuffer(const char *data, size_t len)
	{
		MutexLockGuard lock(_mutex);
		_writeBuffer.append(data, len);
	}
	void setState(StateT state) { _state = state; }
	void setAny(void *any) { _any = any; }
private:
	int _connId;
	SOCKET _socket;
	InetAddress _peerAddr;
	Buffer _readBuffer;
	Buffer _writeBuffer;
	
	StateT _state;
	MutexLock _mutex;;
	void *_any;
};

} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_