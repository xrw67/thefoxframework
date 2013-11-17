#include <net/Eventloop.h>
#include <net/IoContext.h>
#include <net/Acceptor.h>
#include <net/TcpConnection.h>
#include <net/IoCompletionPort.h>

using namespace thefox;

Eventloop::Eventloop(IoCompletionPort * const iocp)
	: _iocp(iocp)
	, _threadId(::GetCurrentThreadId())
	, _loop(false)
	, _quit(false)
{
}

Eventloop::~Eventloop()
{

}

void Eventloop::loop()
{
	OVERLAPPED *overlapped = NULL;
	PULONG_PTR completionKey = NULL;
	DWORD bytesTransfered = 0;
	
	while (_quit) 
	{
		BOOL retCode = _iocp->waitAndGet(&bytesTransfered,
								completionKey, &overlapped, INFINITE);
		// 收到退出标志，直接退出
		if (EXIT_CODE == static_cast<DWORD>(completionKey))
		{s
			break;
		}	
		
		if (!retCode) 
		{
			// if (!_errorCallback(conn, ::GetLastError()))
// 			{
// 				break;
// 			}
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
				switch (ioBuf->getIoType())
				{
				case IoContext::IoType::Accept:
				{
					Acceptor *acceptor = reinterpret_cast<Acceptor *>(*completionKey);
					acceptor->handleAccept(ioContext);
					break;
				}
				case IoContext::IoType::Recv:
					_server->inBuffer.append(ioBuf->_wsaBuf.begin, ioBuf->_wsaBuf.len)
					_server->_messageCallback(conn, conn->_inBuffer);
					break;
				case IoContext::IoType::Send:
				_server->_writeCompleteCallback(conn);
				default:
					break;
					
				}
			}
		}
	}
	_looping = false;
}
