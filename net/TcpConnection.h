#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/Types.h>
#include <net/Buffer.h>
#include <net/InetAddress.h>
#include <net/Callbacks.h>
#include <net/win32.h>

namespace thefox
{
namespace net
{

class Event;
class Socket;

class TcpConnection
{
public:
    TcpConnection(SOCKET s, 
                  const String &name, 
                  const InetAddress &localAddr,
                  const InetAddress &peerAddr);
    ~TcpConnection(void);


    void setConnectionCallback(const ConnectionCallback &cb);
    void setCloseCallback(const CloseCallback &cb);
    void setMessageCallback(const MessageCallback &cb);
    void setWriteCompletionCallback(const WriteCompleteCallback &cb);

    /// @brief connection accept complete
    void connectEstablished();

    void handleInit();
    void handleRead();
    void handleReadComplete();
    void handleWrite();
    void handleWriteComplete();
    void handleZeroByteRead();
    void handleZeroByteReadComplete();
    void handleClose();

    Event *getEvent();
private:
    TcpConnection();
    enum State {
        STATE_CONNECTING,
        STATE_CONNECTED,
        STATE_DISCONNECTING,
        STATE_DISCONNECTED,
        
    };

    const String _name;
    Socket *_socket;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    Buffer _readBuffer;
    Buffer _SendBuffer;

    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeComplateCallback;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_