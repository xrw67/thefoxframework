#include <stdio.h>
#include <net/InetAddress.h>
#include <net/EventLoop.h>
#include <rpc/RpcChannel.h>
#include "../../rpc/test/echo.pb.h"

using namespace thefox;

int main(int argc, char **argv)
{
	WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	EventLoop loop;
	loop.start();

	RpcChannel rpcChannel(&loop);
	rpcChannel.open(InetAddress("127.0.0.1", 7903));
	

	echo::EchoService::Stub echoChannel(&rpcChannel);
	echo::EchoRequest request;
	request.set_message("hello");
	echo::EchoResponse response;

	echoChannel.echo(NULL, &request, &response, NULL);

	loop.join();

	WSACleanup();
	return 0;
}