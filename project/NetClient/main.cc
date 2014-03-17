#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpClient.h>
#include <net/TcpConnection.h>

using namespace thefox;


TcpClient client("MyIocpClientDemo");

void onConnection(const TcpConnectionPtr &conn)
{
	printf("Connection success connID=%d\r\n", conn->getConnId());
	client.send("Hello!", strlen("Hello!"));
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("Connection close connID=%d\r\n", conn->getConnId());
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	printf("%s Message come[%d], size=%u\r\n", receiveTime.toFormatString().c_str(), conn->getConnId(), buf->readableBytes());
	client.send(buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);


	client.setConnectionCallback(onConnection);
	client.setCloseCallback(onClose);
	client.setMessageCallback(onMessage);
	client.open(InetAddress("127.0.0.1", 7901));

    while ('q' != getchar())
        ;

	return 0;
}
