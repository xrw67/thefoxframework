#include <stdio.h>
#include "TcpServer.h"


void onConnection(const TcpConnectionPtr &conn)
{

}

void onClose(const TcpConnectionPtr &conn)
{
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{

}

void onWriteComplete(const TcpConnectionPtr &conn)
{

}


int main(int argc, char *argv[])
{
    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	InetAddress listenAddr(7901);
	TcpServer svr(listenAddr, "MyIocpDemo");

	svr.setConnectionCallback(onConnection);
	svr.setCloseCallback(onClose);
	svr.setMessageCallback(onMessage);
	svr.setWriteCompleteCallback(onWriteComplete);
	
	svr.start();

	return 0;
}