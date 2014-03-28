/*
* @filename TcpConnection.h
* @brief 表示一个客户连接
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/noncopyable.h>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/Buffer.h>
#include <net/InetAddress.h>

namespace thefox
{

#ifndef WIN32
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
	{
	}

	~TcpConnection()
	{
		MutexLockGuard lock(_mutex);
		if (INVALID_SOCKET != _socket) {
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}

	/// @brief 获取连接ID
	int connId() const { return _connId; }

	/// @brief 获取SOCKET句柄
	SOCKET socket() { return _socket; }

	/// @brief 获取对端ip地址
	const InetAddress &getPeerAddr() const { return _peerAddr; }

	/// @brief 获取Read缓冲区指针
	Buffer *readBuffer() { return &_readBuffer; }

	/// @brief 获取Write缓冲区指针
	Buffer *writeBuffer() { return &_writeBuffer; }

	/// @brief 获取连接状态
	StateT state() const { return _state; }

	/// @brief 
	void *any() const { return _any; }

	/// @brief 
	void setAny(void *any) { _any = any; }

	//以下为内部使用
	void setState(StateT state) { _state = state; }

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

private:
	int _connId;
	SOCKET _socket;
	InetAddress _peerAddr;
	Buffer _readBuffer;
	Buffer _writeBuffer;
	
	StateT _state;
	MutexLock _mutex;
	void *_any;
};

} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_