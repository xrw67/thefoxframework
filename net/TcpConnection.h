#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <Winsock2.h>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <net/Buffer.h>
#include <net/InetAddress.h>
#include <net/Callbacks.h>


namespace thefox
{
namespace net
{

class Event;
class Socket;

class TcpConnection : noncopyable
{
public:
    TcpConnection(SOCKET s, 
                  const String &name, 
                  const InetAddress &localAddr,
                  const InetAddress &peerAddr);
    ~TcpConnection(void);


    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb)
    { _closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }

    /// @brief connection accept complete
    void connectEstablished();

    void handleRead();
    void handleReadComplete();
    void handleWrite();
    void handleWriteComplete();
    void handleZeroByteRead();
    void handleZeroByteReadComplete();
    void handleClose();

    Event *getEvent();
private:
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
    WriteCompleteCallback _writeCompleteCallback;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_