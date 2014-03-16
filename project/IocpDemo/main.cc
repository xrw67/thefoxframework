#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpServer.h>

using namespace thefox;

void onConnection(int32_t connId)
{
	printf("Connection success connID=%d\r\n", connId);
}

void onClose(int32_t connId)
{
	printf("Connection close connID=%d\r\n", connId);
}

void onMessage(int32_t connId, Buffer *buf, Timestamp receiveTime)
{
	printf("Message come, size=%u\r\n", buf->readableBytes());
	buf->retrieveAll();
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
	svr.start();

    while ('q' != getchar())
        ;

	return 0;
}