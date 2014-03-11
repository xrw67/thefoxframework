#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <map>
#include <Winsock2.h>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <net/InetAddress.h>
#include <net/Callbacks.h>

namespace thefox
{
namespace net
{

class Iocp;
class Socket;

class TcpServer :noncopyable
{
public:
	TcpServer(const String &nameArg, InetAddress listenAddr);
	~TcpServer(void);

    void start();
    bool started() const { return _started; }
    
    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb)
    { _closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }

private:
    void newConnection(SOCKET s, InetAddress peerAddr);
    static DWORD WINAPI WorkerThreadProc(LPVOID param);
    static DWORD WINAPI ListenerThreadProc(LPVOID param);

    typedef std::map<String, TcpConnectionPtr> ConnectionMap;
    Iocp *_iocp;
    Socket *_listenSocket;
    InetAddress _listenAddr;
    const String _name;
    bool _started;
    uint32_t _nextConnId;

    HANDLE _acceptEvent;

    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;

    ConnectionMap _connections;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_TCPSERVER_H_