#include <stdio.h>
#include <net/callbacks.h>
#include <net/inet_address.h>
#include <net/buffer.h>
#include <net/tcp_client.h>
#include <net/tcp_connection.h>
#include <net/event_loop.h>
#include <log/log_file.h>

using namespace thefox;
using namespace thefox::net;

void onConnection(TcpConnection *conn)
{
	switch (conn->state()) {
	case TcpConnection::kConnected:
		printf("Connection success connID=%d\r\n", conn->id());
		conn->send("ping!", strlen("ping!"));
		break;
	case TcpConnection::kDisconnected:
		printf("Connection disconnect connID=%d\r\n", conn->id());
		break;
	default:
		printf("Connection connection state=%d connID=%d\r\n", conn->state(), conn->id());
		break;
	}
}

void onMessage(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	printf("onMessage readBytes: %u\r\n", conn->readBytes());
	conn->send(buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

void onWriteComplete(TcpConnection *conn)
{
	printf("onWriteComplete writeBytes: %u\r\n", conn->writeBytes());
}

int main(int argc, char **argv)
{
	LogFile log("./logs", "client_test");

	THEFOX_SOCKET_STARTUP;

	EventLoop loop;
	TcpClient *client = new TcpClient(&loop, "MyIocpClientDemo");
	client->setConnectionCallback(onConnection);
	client->setMessageCallback(onMessage);
	client->setWriteCompleteCallback(onWriteComplete);

	loop.start();

	client->open(InetAddress("127.0.0.1", 7901));
	
	loop.join();

	THEFOX_SOCKET_CLEANUP;

	delete client;
	return 0;
}
