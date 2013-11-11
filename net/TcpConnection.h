/*
	
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/noncopyable.h>
#include <base/Mutexlock.h>

namespace thefox
{

class TcpConnection : noncopyable
{
public:
	TcpConnection();
	~TcpConnection();
	
private:
	SOCKET _sock;
	MutexLock _lock;
	
	Buffer _inBuffer;
	Buffer _outBuffer

};
}

#endif // _THEFOX_NET_TCPCONNECTION_H_