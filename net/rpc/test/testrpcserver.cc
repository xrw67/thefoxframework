#include <stdio.h>
#include <net/InetAddress.h>
#include <net/EventLoop.h>
#include <net/SocketOps.h>
#include <net/rpc/RpcServer.h>
#include <net/rpc/test/echo.pb.h>

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
	SocketLibrary::startup();

	EventLoop loop;
	loop.start();

	RpcServer rpcServer(&loop);

	EchoSerivceImpl echoService;
	rpcServer.registerService(&echoService);

	rpcServer.start(InetAddress(7903));

	loop.join();

	SocketLibrary::shutdown();
	return 0;
}