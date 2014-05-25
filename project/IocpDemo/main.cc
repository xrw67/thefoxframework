#include <stdio.h>
#include <net/callbacks.h>
#include <net/inet_address.h>
#include <net/buffer.h>
#include <net/tcp_server.h>
#include <net/tcp_connection.h>
#include <net/event_loop.h>
#include <log/log_stdout.h>
#include <log/log_file.h>

using namespace thefox;

void onConnection(TcpConnection *conn)
{
	switch (conn->state()) {
	case TcpConnection::kConnected:
		printf("Connection success connID=%d\r\n", conn->id());
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
	printf("收到数据 总共读:%u\r\n", conn->readBytes());
	conn->send(buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

void onWriteComplete(TcpConnection *conn)
{
	printf("写数据完成 总共写:%u\r\n", conn->writeBytes());
}

int main(int argc, char *argv[])
{
	//LogStdout log("tcp_server");
	LogFile log(".", "iocp");

	THEFOX_LOG(INFO) << "tcp_server bigin";
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	EventLoop loop;
	TcpServer *svr = new TcpServer(&loop, InetAddress(7901), "MyIocpDemo");
	svr->setConnectionCallback(onConnection);
	svr->setMessageCallback(onMessage);
	svr->setWriteCompleteCallback(onWriteComplete);
	svr->start();
	loop.start();
	loop.join();

	WSACleanup();
	return 0;
}