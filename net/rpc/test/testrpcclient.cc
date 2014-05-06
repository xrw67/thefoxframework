#include <stdio.h>
#include <net/InetAddress.h>
#include <net/EventLoop.h>
#include <net/SocketOps.h>
#include <net/rpc/RpcClient.h>
#include <net/rpc/RpcChannel.h>
#include <net/rpc/test/echo.pb.h>

using namespace thefox;

int main(int argc, char **argv)
{
	SocketLibrary::startup();

	EventLoop loop;
	loop.start();

	RpcClient client(&loop);
	RpcChannel channel(&client, InetAddress("127.0.0.1", 7903));
	

	echo::EchoService::Stub echoChannel(&channel);
	echo::EchoRequest request;
	request.set_message("hello");
	echo::EchoResponse response;

	echoChannel.echo(NULL, &request, &response, NULL);
	
	loop.join();

	SocketLibrary::shutdown();
	return 0;
}