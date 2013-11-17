#include <stdio.h>
#include <net/InetAddress.h>
#include <net/TcpServer.h>

using namespace thefox;

int _tmain(int argc, char **argv)
{
	//InitSocket sockLib;
	InetAddress listenAddr(12345);
	TcpServer server(listenAddr, "macwe");
	server.start();

	return 0;
}

