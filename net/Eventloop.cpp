#include <net/Eventloop.h>

using namespace thefox;

Eventloop::Eventloop(const TcpServer *server)
	: _server(server)
	, _threadId(::GetCurrentThreadId());
	, _loop(false)
	, _quit(false)
{
}

Eventloop::~Eventloop()
{

}

Eventloop::loop()
{
	OVERLAPPED *overlapped = NULL;
	PULONG_PTR completionKey = NULL;
	DWORD bytesTransfered = 0;
	while (_quit) 
	{
		BOOL retCode = ::GetQueuedComplettionStatus(_server->_completionPort,
													&bytesTransfered,
													completionKey,
													&overlapped,
													INFINITE);
		
		// 收到退出标志，直接退出
		if (EXIT_CODE == static_cast<DWORD>(completionKey))
		{
			break;
		}	
		
		if (!retCode) 
		{
			DWORD errCode = GetLastError();
			if (!_server->_errorCallback(conn, errCode))
				break;
			
			continue;
		}
		else
		{
			IoContext *ioContext = CONTAINING_RECORD(overlapped, IoContext, _overlapped);
			
			// 判断客户端是否断开连接
			if ((0 == bytesTransfered) && 
				(RECV == ioBuf->ioType() || WRITE == ioBuf->ioType())
			{
				_server->removeConnection(&conn);
				continue;
			} 
			else 
			{
				switch (ioBuf->ioType())
				{
				case ACCEPT:
				{
					Acceptor *acceptor = reinterpret_cast<Acceptor *>(*completionKey);
					acceptor->handleAccept(ioContext);
					break;
				}
				case RECV:
					_server->inBuffer.append(ioBuf->_wsaBuf.begin, ioBuf->_wsaBuf.len)
					_server->_messageCallback(conn, conn->_inBuffer);
					break;
				case SEND:
				_server->_writeCompleteCallback(conn);
				case TIMER:
				default:
					break;
					
				}
			}
		}
	}
	_looping = false;
}
