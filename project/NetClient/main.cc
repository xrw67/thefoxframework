#include <stdio.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/Buffer.h>
#include <net/TcpClient.h>
#include <net/TcpConnection.h>

using namespace thefox;


TcpClient client("MyIocpClientDemo");

void onConnection(int32_t connId)
{
	printf("Connection success connID=%d\r\n", connId);
	client.send("Hello!", strlen("Hello!"));
}

void onClose(int32_t connId)
{
	printf("Connection close connID=%d\r\n", connId);
}

void onMessage(int32_t connId, Buffer *buf, Timestamp receiveTime)
{
	printf("%s Message come[%d], size=%u\r\n", receiveTime.toFormatString().c_str(), connId, buf->readableBytes());
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
