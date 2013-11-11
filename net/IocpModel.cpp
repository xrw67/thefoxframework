


DWORD WINAPI WorkThread(LPVOID param)
{
	TcpServer *server = reinterpret_cast<TCPServer *>param;
	OVERLAPPED *overlapped = NULL;
	TcpConnection *tcpConnection;
	DWORD bytesTransfered = 0;
	while (1) {
		BOOL retCode = GetQueuedComplettionStatus(
			server->_ioCompletionPort,
			&bytesTransfered,
			&conn,
			&overlapped,
			INFINITE);
		
		// 收到退出标志，直接退出
		if (EXIT_CODE == static_cast<DWORD>(tcpConnection))
			break;
		
		if (!retCode) {
			DWORD errCode = GetLastError();
			if (!server->_errorCallback(conn, errCode))
				break;
			
			continue;
		} else {
			IoBuffer *ioBuf = CONTAINING_RECORD(overlapped, IoBuffer, _overlapped);
			
			// 判断客户端是否断开连接
			if ((0 == bytesTransfered) && 
					(RECV == ioBuf->ioType() || WRITE == ioBuf->ioType()) {
				server->removeConnection(&conn);
				continue;
			} else {
				switch (ioBuf->ioType()) {
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
	}
}