#ifndef WIN32
#else

#include <net/win/Iocp.h>

using namespcace thefox;
using namespace thefox::net;

Iocp::Iocp()
{


}

Iocp::~Iocp()
{


}

static DWORD Iocp::listenThreadProc(LPVOID param)
{

}

static DWORD Iocp::ioWorkerThreadProc(LPVOID param)
{
	Iocp *pThis = reinterpret_cast<Iocp *>(param);
	if (NULL == pThis)
		return -1;

	DWORD ioSize;
	TcpConntctionPtr conn;
	IocpBufferPtr buf;
	
	LPOVERLAPPED overlapped;

	while (pThis->started())
	{
		overlapper = NULL;
		conn = NULL;

		BOOL result = GetQueuedCompletionStatus(
			pThis->_completionPort,
			&ioSize,
			(LPDWORD)&connPtr,
			&overlapped, 
			INFINITE);

		if (!result) // 错误处理
		{
			DWORD errCode = GetLastError();
			if (WAIT_TIMEOUT != errCode) // 上面使用了INFINITE，所有不会超时错误
			{
				if (NULL != conn)
				{
					pThis->removeConnection(conn);

					buf = NULL;
					if (NULL != overlapped)
					{
						buf = CONTAINING_RECORD(overlapped, IocpBuffer, _overlapped);
					}
					if (NULL != buf)
					{
						pThis->releaseBuffer(buf);
					}
					continue;
				}

				// 代码运行到此处，说明线程产生了一场，需要重启线程
				buf = NULL;
				if (NULL != overlapped)
				{
					buf = CONTAINING_RECORD(overlapped, IocpBuffer, _overlapped);
				}
				if (NULL != buf)
				{
					pThis->releaseBuffer(buf);
				}
				break;
			}
		}

		if (result && (NULL != overlapped) && (NULL != conn))
		{
			buf = CONTAINING_RECORD(overlapped, IocpBuffer, _overlapped);
			if (NULL != buf)
			{
				pThis->processIoMessage(buf, conn, ioSize);
			}
		}

		if ((NULL == conn) && (NULL == buf))
		{
			break;
		}
	}
}

#endif // WIN32