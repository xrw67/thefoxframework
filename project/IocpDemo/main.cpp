#include <stdio.h>
#include <net/TcpServer.h>
#include <net/Callbacks.h>

using namespace thefox;
using namespace thefox::net;

void onConnection(const TcpConnectionPtr &conn)
{

}

void onClose(const TcpConnectionPtr &conn)
{
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{

}

void onWriteComplete(const TcpConnectionPtr &conn)
{

}

int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	InetAddress listenAddr(7901);
	TcpServer svr("MyIocpDemo", listenAddr);

	svr.setConnectionCallback(onConnection);
	svr.setCloseCallback(onClose);
	svr.setMessageCallback(onMessage);
	svr.setWriteCompleteCallback(onWriteComplete);
	
	svr.start();

    while ('q' != getchar())
        ;

	return 0;
}