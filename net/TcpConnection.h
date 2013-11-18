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
#include <net/Callbacks.h>
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
	void handleRead(IoContext *ioContext, Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	
    IoContext *getFreeIoContext()
    {
        IoContext *ioContext = NULL;
        if (!_freeIoConetxts.empty())
        {
            ioContext = _freeIoConetxts.front();
            _freeIoConetxts.pop_front();
        }
        if (NULL == ioContext)
        {
            ioContext = new IoContext();
        }
        return ioContext;
    }
	void addToIoContextList(IoContext *ioContext)
    {
        _freeIoConetxts.push_back(ioContext);
    }
    
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
    std::list<scoped_ptr<IoContext>> _freeIoConetxts;
	Buffer _inBuffer;
};

}

#endif // _THEFOX_NET_TCPCONNECTION_H_