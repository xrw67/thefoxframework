#include <stdio.h>
#include <net/inet_Address.h>
#include <net/event_loop.h>
#include <net/socket_ops.h>
#include <rpc/rpc_client.h>
#include <rpc/rpc_channel.h>
#include <rpc/rpc_controller.h>
#include <rpc/test/echo.pb.h>

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
	RpcController controller;
	echoChannel.echo(&controller, &request, &response, NULL);
	
	loop.join();

	SocketLibrary::shutdown();
	return 0;
}