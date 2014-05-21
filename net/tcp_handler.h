#ifndef _THEFOX_NET_TCP_HANDLER_H_
#define _THEFOX_NET_TCP_HANDLER_H_

#include <net/callbacks.h>

namespace thefox
{

class TcpHandler
{
public:
	friend class TcpConnection;

	TcpHandler();
	virtual ~TcpHandler();

	/// @brief 设置连接状态改变回调函数
    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
	 /// @brief 设置连接关闭回调函数
    void setCloseCallback(const CloseCallback &cb)
    { _closeCallback = cb; }
	/// @brief 设置收到数据的回调函数
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
	/// @brief 设置缓冲区中数据发送完成后的回调函数
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }
protected:
	ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_TCP_HANDLER_H_