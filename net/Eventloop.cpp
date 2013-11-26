#include <net/Eventloop.h>
#include <net/IoCompletionPort.h>
#include <net/Acceptor.h>
#include <net/TcpConnection.h>
#include <net/IoBuffer.h>
#include <net/AcceptIoBuffer.h>

using namespace thefox;

Eventloop::Eventloop(IoCompletionPort &iocp)
	: _iocp(iocp)
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
	OVERLAPPED *overlapped = NULL;
	ULONG_PTR completionKey = NULL;
	DWORD bytesTransfered = 0;
	
	while (_quit) {
		BOOL retCode = _iocp.getStatus(&bytesTransfered, &completionKey, &overlapped, INFINITE);
		// 收到退出标志，直接退出
		if (0 == completionKey)
			break;
		
		if (!retCode) {
			// if (!_errorCallback(conn, ::GetLastError()))
// 			{
// 				break;
// 			}
			continue;
		}
		
		if (retCode && completionKey && overlapped) {
			PerSocketContext *perSocketContext = reinterpret_cast<PerSocketContext *>(completionKey);
			switch (perSocketContext->getContextType()) {
				case PerSocketContext::ContextType::Acceptor: 
					Acceptor *acceptor = reinterpret_cast<Acceptor *>(perSocketContext);
					AcceptIoBuffer *buffer = CONTAINING_RECORD(overlapped, AcceptIoBuffer, _overlapped);
					acceptor->handleAccept(reinterpret_cast<AcceptIoBuffer *>(buffer));
					break;									  
				case PerSocketContext::ContextType::TcpConnection:
					TcpConnection *conn = reinterpret_cast<TcpConnection *>(perSocketContext);
					IoBuffer *buffer = CONTAINING_RECORD(overlapped, IoBuffer, _overlapped);
					conn->handleIoProcess(buffer);
					break;
			}
			
			
		}
			
			
						
			// 判断客户端是否断开连接
			if ((0 == bytesTransfered) && (RECV == ioBuf->ioType() || WRITE == ioBuf->ioType()) {
				_server->removeConnection(&conn);
				continue;
			} else {
				switch (ioBuf->getIoType()) {
					case IoBuffer::IoType::Accept: {
						Acceptor *acceptor = reinterpret_cast<Acceptor *>(*completionKey);
						acceptor->handleAccept(ioBuffer);
						break;
					}
					case IoBuffer::IoType::Recv: {
						_server->inBuffer.append(ioBuf->_wsaBuf.begin, ioBuf->_wsaBuf.len)
							_server->_messageCallback(conn, conn->_inBuffer);
						break;
					}
					case IoBuffer::IoType::Send:
						_server->_writeCompleteCallback(conn);
						default:
						break;
					
				}
			}
	}
	_looping = false;
}
