#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpServer.h>
#include <net/TcpConnection.h>
#include <net/EventLoop.h>

using namespace thefox;

TcpServer *svr = NULL;

void onConnection(const TcpConnectionPtr &conn)
{
	printf("Connection success connID=%d\r\n", conn->connId());
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("Connection close connID=%d\r\n", conn->connId());
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	buf->appendInt8(0);
	printf("%s", buf->peek());
	svr->send(conn, buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	EventLoop loop;
	svr = new TcpServer(&loop, "MyIocpDemo");
	svr->setConnectionCallback(onConnection);
	svr->setCloseCallback(onClose);
	svr->setMessageCallback(onMessage);
	svr->start(InetAddress(7901));
	loop.exec();

    while ('q' != getchar())
        ;

	WSACleanup();
	delete svr;
	return 0;
}