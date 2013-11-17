#include <net/TcpConnection.h>

using namespace thefox;

TcpConnection::TcpConnection(const String &name, 
							SOCKET socket, 
							const InetAddress &localAddr, 
							const InetAddress &peerAddr)
	: _name(name)
	, _socket(socket)
	, _localAddr(localAddr)
	, _peerAddr(peerAddr)
{
	IoContext *ioContext = new IoContext(IoContext::IoType::Recv);
	if (postRecv(ioContext))
	{
		_ioConetxts.puch_back(ioContext);
	}
	else
	{
		delete ioContext;
	}
}

bool TcpConnection::postRecv(IoContext *ioContext)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	ioContext->ResetBuffer();
	ioContext->setIoType(IoContext::IoType::recv);

	int nBytesRecv = WSARecv(_socket, &ioContext->_wsaBuf, 1, &dwBytes, &dwFlags, &ioContext->_overlapped, NULL );

	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		return false;
	}
	return true;	
}