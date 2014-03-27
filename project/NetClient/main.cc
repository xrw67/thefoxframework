#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpClient.h>
#include <net/TcpConnection.h>
#include <net/EventLoop.h>

using namespace thefox;


TcpClient *client = NULL;

void onConnection(const TcpConnectionPtr &conn)
{
	printf("Connection success\r\n");
	client->send("Hello!", strlen("Hello!"));
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("Connection close\r\n");
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	printf("%s Message come, size=%u\r\n", receiveTime.toFormatString().c_str(), buf->readableBytes());
	client->send(buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	EventLoop loop;
	client = new TcpClient(&loop, "MyIocpClientDemo");
	client->setConnectionCallback(onConnection);
	client->setCloseCallback(onClose);
	client->setMessageCallback(onMessage);
	client->open(InetAddress("127.0.0.1", 7901));
	loop.exec();

    while ('q' != getchar())
        ;
	WSACleanup();
	delete client;
	return 0;
}
