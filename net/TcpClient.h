/*
 * @filename TcpClient.h
 * @brief tcp客户端连接类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_NET_TCPCLIENT_H_
#define _THEFOX_NET_TCPCLIENT_H_

#include <base/Types.h>
#include <base/noncopyable.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>

namespace thefox
{
#ifdef WIN32
    class WsaEventClient;
#else
    class PollClient;
#endif

class TcpClient : noncopyable
{
public:
    TcpClient(InetAddress serverAddr);
    TcpClient(void);
        
    /// @brief 连接服务器
    /// @return 成功返回true，否则返回false
    bool open();
        
    /// @brief 关闭连接
    void close();
        
    /// @brief 查看是否已经连接
    /// @return 已经连接返回true，否则返回false
    bool isOpen();
        
    /// @brief 设置连接状态改变回调函数
    void setConnectionCallback(const ConnectionCallback &cb)
        
    /// @brief 设置连接关闭回调函数
    void setCloseCallback(const CloseCallback &cb)
        
    /// @brief 设置收到数据的回调函数
    void setMessageCallback(const MessageCallback &cb)
private:
#ifdef WIN32
    WsaEventClient *_model;
#else
    PollClient *_model;
#endif
    };
    
} // namespace thefox

#endif // _THEFOX_NET_TCPCLIENT_H_