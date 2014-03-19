#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpServer.h>
#include <net/TcpConnection.h>

using namespace thefox;

TcpServer svr("MyIocpDemo");

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
	printf("%s Message come[%d], size=%u\r\n", receiveTime.toFormatString().c_str(), connId, buf->readableBytes());
	svr.send(connId, buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);


	svr.setConnectionCallback(onConnection);
	svr.setCloseCallback(onClose);
	svr.setMessageCallback(onMessage);
	svr.start(InetAddress(7901));

    while ('q' != getchar())
        ;

	return 0;
}