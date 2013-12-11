/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <net/winapi.h>
#include <net/Buffer.h>
#include <net/IoBuffer.h>
#include <net/InetAddress.h>
#include <net/Socket.h>

namespace thefox
{

class TcpConnection
{
public:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	typedef std::map<uint32_t, IoBuffer *> IoBufferMap;

	TcpConnection(const String &name, 
				SOCKET socket, 
				const InetAddress &localAddr, 
				const InetAddress &peerAddr)
		: _name(name)
		, _socket(socket)
		, _localAddr(localAddr)
		, _peerAddr(peerAddr)
		, _state(kDisconnected)
	{}
	~TcpConnection();
	
	const String &name() const {return _name; }
	const InetAddress &localAddress() {return _localAddr; }
	const InetAddress &peerAddress() {return _peerAddr; }
	bool connected() const { return _state == kConnected; }
	
    void setState(StateE s) { _state = s; }

	IoBuffer *getNextReadBuffer(IoBuffer *buf = NULL)
	{
		IoBuffer *outBuf = NULL;
		if (buf) {
			if (_currentReadSequence == buf->getSequence()) {
			    outBuf = buf;
				++_currentReadSequence;
			} else if(_currentReadSequence < buf->getSequence()) {
				_readBufferMap[buf->getSequence()] = buf;
				if (_readBufferMap.end() != _readBufferMap.find(_currentReadSequence)) {
				    outBuf = _readBufferMap[_currentReadSequence];
					_readBufferMap.erase(_currentReadSequence);
					++_currentReadSequence;
				}
			}
		} else {
			if (_readBufferMap.end() != _readBufferMap.find(_currentReadSequence)) {
			    outBuf = _readBufferMap[_currentReadSequence];
				_readBufferMap.erase(_currentReadSequence);
				++_currentReadSequence;
			}
		}
		return outBuf;
	}
	IoBuffer *getNextSendBuffer(IoBuffer *buf = NULL)
	{
	
	}
	
	StateE _state;
	SOCKET _socket; // 客户连接的SOCKET
	String _name;  // 客户连接的名称
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	Buffer _inBuffer; // 接收缓存

	uint32_t _numberOfPendingIo;
	// 读队列
	uint32_t _readSequence;
	uint32_t _currentReadSequence;
	IoBufferMap _readBufferMap;
	// 发送队列
	uint32_t _sendSequence;
	uint32_t _currentSendSequence;
	IoBufferMap _sendBufferMap;
};

}

#endif // _THEFOX_NET_TCPCONNECTION_H_