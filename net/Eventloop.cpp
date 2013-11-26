#include <net/Eventloop.h>
#include <net/TcpServer.h>
#include <net/Acceptor.h>
#include <net/TcpConnection.h>
#include <net/IoBuffer.h>


using namespace thefox;

Eventloop::Eventloop(TcpServer *server)
	: _server(server)
	, _threadId(::GetCurrentThreadId())
	, _looping(false)
	, _quit(false)
{
}

Eventloop::~Eventloop()
{
}

void Eventloop::loop()
{
	LPOVERLAPPED lpOverlapped = NULL;
	TcpConnectionPtr tcpConnection = NULL;
	IoBuffer *ioBuffer = NULL;
	DWORD bytesTransfered = 0;
	
	while (!_quit) {
		lpOverlapped = NULL;
		conn = NULL;
		BOOL retCode = _iocp.getStatus(&bytesTransfered, &tcpConnection, &lpOverlapped, INFINITE);
		
		if (!retCode) {
			DWORD errCode = ::GetLastError();
			if (WAIT_TIMEOUT != errCode && NULL != tcpConnection)
				_server->removeConnection(tcpConnection);
			continue;
		}
		
		if (retCode && tcpConnection && lpOverlapped) {
			ioBuffer = CONTAINING_RECORD(lpOverlapped, IoBuffer, _overlapped);
			if (ioBuffer)
				tcpConnection->handleIoProcess(ioBuffer);
		}
			
		// 收到退出标志，直接退出
		if (NULL == completionKey &&  NULL == lpOverlapped)
			_quit = true;
	}
	_looping = false;
}
