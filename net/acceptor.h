/*
 * @filename TcpServer.h
 * @brief tcp服务器类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

#include <map>
#include <list>
#include <base/types.h>
#include <base/mutex.h>
#include <base/Atomic_integer.h>
#include <net/callbacks.h>

namespace thefox
{

class Acceptor
{
public:
    TcpServer(EventLoop *eventloop, const std::string &nameArg);
    ~TcpServer(void);

    void handleNewConnection();

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Acceptor);

    ConnectionCallback _connectionCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_ACCEPTOR_H_