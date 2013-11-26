#ifndef _THEFOX_NET_PER_SOCKET_CONTEXT_H_
#define _THEFOX_NET_PER_SOCKET_CONTEXT_H_

#include <base/noncopyable.h>
#include <net/winapi.h>

namespace thefox
{

class PerSocketContext : noncopyable
{
public:
	enum ContextType { Acceptor, TcpConnection };
	
	PerSocketContext()
	{}
	virtual ~PerSocketContext()
	{}
	
	void setContextType(ContextType type) { _contextType = type; }
	const ContextType getContextType() const { return _contextType; }
	void setSocketHandle(SOCKET socket) { _socket = socket; }
	const SOCKET getSocketHandle() const { return _socket; }
	MutexLock &getMutexLock() { return _mutex; }
	
private:
	ContextType _contextType;
	SOCKET _socket;
	MutexLock _mutex;
};

}

#endif // _THEFOX_NET_PER_SOCKET_CONTEXT_H_