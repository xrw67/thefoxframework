#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <map>
#include <base/Types.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/win32.h>

namespace thefox
{
namespace thefox
{

class Iocp;
class Socket;
class TcpConnection;

class TcpServer
{
public:
	TcpServer(const InetAddress &listenAddr,
              const String &nameArg);
	~TcpServer(void);

    void start();
    
    void setConnectionCallback(ConnectionCallback &cb);
    void setCloseCallback(CloseCallback &cb);
    void setMessageCallback(MessageCallback &cb);
    void setWriteCompletionCallback(WriteCompletionCallback &cb);

private:
    TcpServer();
    void newConnection(SOCKET s, const InetAddress &peerAddr);
    static DWORD WorkerThreadProc(LPVOID param);
    static DWORD ListenerThreadProc(LPVOID param);

    typedef std::map<string, TcpConnection *> ConnectionMap;
    Iocp *_iocp;
    Socket *_listenSocket;
    const String _hostport;
    const String _name;
    bool _started;
    volatile _nextConnId;

    HANDLE _acceptEvent;

    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeComplateCallback;

    ConnectionMap _connections;
    //std::list<TcpConnection *> _freeConnections;
    EventPool _freeEvents;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_TCPSERVER_H_