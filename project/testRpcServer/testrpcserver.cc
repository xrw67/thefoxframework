#include <stdio.h>
#include <net/InetAddress.h>
#include <net/EventLoop.h>
#include <rpc/RpcServer.h>
#include "../../rpc/test/echo.pb.h"

using namespace thefox;

class EchoSerivceImpl : public echo::EchoService
{
	virtual void echo(gpb::RpcController *controller,
						const echo::EchoRequest *request,
						echo::EchoResponse *response,
						gpb::Closure *done)
	{
		response->set_response(request->message());
		done->Run();
	}
};

int main(int argc, char **argv)
{
	WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);

	EventLoop loop;
	loop.start();

	RpcServer rpcServer(&loop);

	EchoSerivceImpl echoService;
	rpcServer.registerService(&echoService);

	rpcServer.start(InetAddress(7903));

	loop.join();

	WSACleanup();
	return 0;
}