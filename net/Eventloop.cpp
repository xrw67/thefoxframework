#include <net/Eventloop.h>

using namespace thefox;

namespace
{
	const DWORD kPollTimeMs = 1000; // 轮询超时时间，单位:毫秒

}

Eventloop::Eventloop(const HANDLE &completionPort)
	: _completionPort(completionPort)
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
	OVERLAPPED *overlapped = nullptr;
	TcpConnection *tcpConnection = nullptr;
	DWORD bytesTransfered = 0;
	while (_quit) 
	{
		BOOL retCode = ::GetQueuedComplettionStatus(_completionPort,
													&bytesTransfered,
													&tcpConnection,
													&overlapped,
													kPollTimeMs);
		
		// 收到退出标志，直接退出
		if (EXIT_CODE == static_cast<DWORD>(tcpConnection))
		{
			break;
		}	
		
		if (!retCode) 
		{
			DWORD errCode = GetLastError();
			if (!server->_errorCallback(conn, errCode))
				break;
			
			continue;
		}
		else
		{
			IoBuffer *ioBuf = CONTAINING_RECORD(overlapped, IoBuffer, _overlapped);
			
			// 判断客户端是否断开连接
			if ((0 == bytesTransfered) && 
				(RECV == ioBuf->ioType() || WRITE == ioBuf->ioType())
			{
				server->removeConnection(&conn);
				continue;
			} 
			else 
			{
				switch (ioBuf->ioType())
				{
				case ACCEPT:
					server->_acceptCallback(conn, ioBuf->_sock);
					break;
				case RECV:
					conn->inBuffer.append(ioBuf->_wsaBuf.begin, ioBuf->_wsaBuf.len)
					server->_messageCallback(conn, conn->_inBuffer);
					break;
				case SEND:
				default:
					break;
					
				}
			}
		}
		//doPengingFunctors();
	}
	_looping = false;
}
