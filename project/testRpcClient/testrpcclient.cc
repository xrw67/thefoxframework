#include <stdio.h>
#include <net/InetAddress.h>
#include <net/EventLoop.h>
#include <net/SocketOps.h>
#include <rpc/RpcChannel.h>
#include "../../rpc/test/echo.pb.h"

using namespace thefox;

int main(int argc, char **argv)
{
	SocketLibrary::startup();

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

	SocketLibrary::shutdown();
	return 0;
}