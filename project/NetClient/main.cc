#include <stdio.h>
#include <net/net_asio.h>
#include <net/tcp_client.h>

using namespace thefox;

char text[81920];

void onConnection(const TcpConnectionPtr &conn)
{
	conn->send(text, strlen(text));
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("Connection close\r\n");
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	printf("%s Message come, size=%u\r\n", receiveTime.toFormatString().c_str(), buf->readableBytes());
	conn->send(buf->peek(), buf->readableBytes());
	buf->retrieveAll();
}

int main(int argc, char *argv[])
{
	for (int i = 0; i < sizeof(text); ++i ) {
		text[i] = 'a';
	}
	text[sizeof(text)] = '\0';

	io_service io;

	ip::tcp::endpoint serverAddr(ip::address::from_string("127.0.0.1"), 8888);

	TcpClient client(io, "fdf");
	client.setConnectionCallback(onConnection);
	client.setMessageCallback(onMessage);
	client.open(serverAddr);
	
	//for(;;) {
		io.run();
	//}
	
	return 0;
}
