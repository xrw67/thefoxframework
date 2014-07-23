#include <stdio.h>
#include <net/net_asio.h>
#include <net/tcp_server.h>

using namespace thefox;

void onConnection(const TcpConnectionPtr &conn)
{
	switch (conn->state()) {
	case TcpConnection::kConnected:
		printf("Connection success connID=%d\r\n", conn->id());
		//conn->forceClose();
		break;
	case TcpConnection::kDisconnected:
		printf("Connection disconnect connID=%d\r\n", conn->id());
		break;
	default:
		printf("Connection connection state=%d connID=%d\r\n", conn->state(), conn->id());
		break;
	}
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("Connection close connID=%d\r\n", conn->id());
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	printf("收到数据 总共读:%u 总共写:%u\r\n", conn->readBytes(), conn->writeBytes());
	conn->send(buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
	io_service io;

	ip::tcp::endpoint listenAddr(ip::tcp::v4(), 8888);

	TcpServer server(io, listenAddr, "fdf");
	
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	server.start();
	
	//for(;;) {
		io.run();
	//}

	return 0;
}