#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <map>
#include <base/noncopyable.h>
#include <base/Types.h>
#include <Winsock2.h>
#include <net/InetAddress.h>

namespace thefox
{


class IoContext;
class ClientContext;

class Iocp : noncopyable
{
public:
	Iocp(const String &nameArg, InetAddress listenAddr);
	~Iocp(void);
    bool start();
    void stop();
    bool started();
    void setConnectionCallback(const ConnectionCallback &cb);
    void setCloseCallback(const CloseCallback &cb);
    void setMessageCallback(const MessageCallback &cb);
private:
	/// @brief associate a socket to this iocp;
	bool registerSocket(SOCKET s, TcpConnection *conn);
    bool postCompletion(IoContext *buf, TcpConnection *conn, DWORD bytesTransferred);

    // 工作者循环
	void workerLoop();
    // 接受者循环
    void acceptorLoop();
    
    SOCKET _listenSocket;
	HANDLE _hIocp;
    HANDLE _hShuwdownEvent;
    bool _quit;
    
    const String nameArg;
    
    
    
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_