#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpServer.h>
#include <net/TcpConnection.h>

using namespace thefox;

InetAddress listenAddr(7901);
TcpServer svr("MyIocpDemo", listenAddr);

void onConnection(const TcpConnectionPtr &conn)
{
	printf("Connection success connID=%d\r\n", conn->getConnId());
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("Connection close connID=%d\r\n", conn->getConnId());
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	printf("%s Message come[%d], size=%u\r\n", receiveTime.toFormatString().c_str(), conn->getConnId(), buf->readableBytes());
	svr.send(conn, buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);


	svr.setConnectionCallback(onConnection);
	svr.setCloseCallback(onClose);
	svr.setMessageCallback(onMessage);
	svr.start();

    while ('q' != getchar())
        ;

	return 0;
}