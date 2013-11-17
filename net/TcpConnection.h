/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/noncopyable.h>
#include <base/Mutexlock.h>
#include <net/inc.h>
#include <net/Buffer.h>
#include <net/IoContext.h>
#include <net/InetAddress.h>

namespace thefox
{

class TcpConnection : noncopyable
{
public:
	TcpConnection(const String &name, SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr);
	~TcpConnection();
	
private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	
	void postRecv(IoContext *ioContext);
	
	SOCKET _sock;
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