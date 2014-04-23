TheFox RPC framework
===============
猎狐软件 的 RPC框架
基于protobuf开发，支持同步和异步的调用方式

使用说明：
1.定义service描述,并生产代码

	package echo;
	
	option cc_generic_services = true;
	
	message EchoRequest {
	  required string message = 1;
	}
	
	message EchoResponse {
	  required string response = 1;
	}
	
	service EchoService {
	  rpc echo (EchoRequest) returns (EchoResponse);
	}

2.服务端：

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

2.客户端

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
