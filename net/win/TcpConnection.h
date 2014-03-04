/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#ifndef WIN32
#else

#include <net/Buffer.h>
#include <base/MutexLock.h>
#include <net/InetAddress.h>
#include <base/net/win/IocpBuffer.h>

namespace thefox
{

namespace net
{

class TcpConnection
{
public:
	TcpConnection(const String &name, 
				SOCKET socket, 
				const InetAddress &localAddr, 
				const InetAddress &peerAddr)
		: _name(name)
		, _socket(socket)
		, _localAddr(localAddr)
		, _peerAddr(peerAddr)
	{}
	~TcpConnection()
	{}
	
	const String &name() const { return _name; }
	const InetAddress &localAddress() { return _localAddr; }
	const InetAddress &peerAddress() { return _peerAddr; }
	
	uint32_t enterIoLoop() 
	{ 
		MutexLockGuard lock(_lock);
		++_numberOfPendingIo;
		return _numberOfPendingIo;
	}

	uint32_t leaveIoLoop() 
	{
		MutexLockGuard lock(_lock);
		--_numberOfPendingIo;
		return _numberOfPendingIo;
	}

	IocpBufferPtr getNextReadBuffer(IocpBufferPtr buf = NULL)
	{
		IocpBufferPtr outBuf = NULL;
		if (buf) 
		{
			if (_currentReadSequence == buf->sequence())
			{
			    outBuf = buf;
				++_currentReadSequence;
			} 
			else if(_currentReadSequence < buf->sequence()) 
			{
				_readBufferMap[buf->sequence()] = buf;
				if (_readBufferMap.end() != _readBufferMap.find(_currentReadSequence)) 
				{
				    outBuf = _readBufferMap[_currentReadSequence];
					_readBufferMap.erase(_currentReadSequence);
					++_currentReadSequence;
				}
			}
		} 
		else 
		{
			if (_readBufferMap.end() != _readBufferMap.find(_currentReadSequence)) 
			{
			    outBuf = _readBufferMap[_currentReadSequence];
				_readBufferMap.erase(_currentReadSequence);
				++_currentReadSequence;
			}
		}
		return outBuf;
	}
	IoBuffer *getNextSendBuffer(IoBuffer *buf = NULL)
	{
		IocpBufferPtr outBuf = NULL;
		if (buf) 
		{
			if (_currentSendSequence == buf->sequence())
			{
			    outBuf = buf;
				++_currentSendSequence;
			} 
			else if(_currentSendSequence < buf->sequence()) 
			{
				_sendBufferMap[buf->sequence()] = buf;
				if (_sendBufferMap.end() != _sendBufferMap.find(_currentSendSequence)) 
				{
				    outBuf = _sendBufferMap[_currentSendSequence];
					_sendBufferMap.erase(_currentSendSequence);
					++_currentSendSequence;
				}
			}
		} 
		else 
		{
			if (_sendBufferMap.end() != _sendBufferMap.find(_currentsendSequence)) 
			{
			    outBuf = _sendBufferMap[_currentSendSequence];
				_sendBufferMap.erase(_currentSendSequence);
				++_currentSendSequence;
			}
		}
		return outBuf;
	}
private:
	SOCKET _socket; // 客户连接的SOCKET
	String _name;  // 客户连接的名称
	MutexLock _lock;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	Buffer _readBuffer; // 接收缓存
	uint32_t _numberOfPendingIo; // 正在处理的IoBuffer个数,避免销毁后出现访问越界

	// 读队列
	uint32_t _readSequence;
	uint32_t _currentReadSequence;
	IocpBufferMap _readBufferMap;

	// 发送队列
	uint32_t _sendSequence;
	uint32_t _currentSendSequence;
	IocpBufferMap _sendBufferMap;

	void *_any; // 绑定的任何数据
};

} // namespace net

} // namespace thefox

#endif // WIN32

#endif // _THEFOX_NET_TCPCONNECTION_H_