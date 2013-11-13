#include <net/TcpServer.h>

using namespace thefox;

TcpServer::TcpServer(const int port,
					const string &nameArg,
					Option option)
	: _hostport(port)
	, _name(nameArg)
	, _connectionCallback(defaultConnectionCallback)
	, _messageCallback(defaultMessageCallback)
	, _nextConnId(1)
{
	
	
}

TcpServer::~TcpServer()
{
	for (ConnectionMap::iterator it(_connections.begin()));
	it != _connections.end(); ++it)
	{
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		
	}
}

void TcpServer::start()
{
	if (_started)
	{
		return;
	}
	
		_started = TRUE;
		
		ioCompletionPort = ::CreateIoCompletePort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (NULL == ioCompletionPort)
			return;
		
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int threads = 2 * si.dwNumberOfProcessors;
		
		for (int i = 0; i < threads; ++i)
		{
			EventloopThread *loop = new EventloopThread(this);
			loop->strarLoop();
		}
		
		// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
		GUID GuidAcceptEx = WSAID_ACCEPTEX;  
		GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

		// 服务器地址信息，用于绑定Socket
		struct sockaddr_in serverAddress;

		// 生成用于监听的Socket的信息
		m_pListenContext = new PER_SOCKET_CONTEXT;

		// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
		m_pListenContext->m_Socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == m_pListenContext->m_Socket) 
		{
			this->_ShowMessage("初始化Socket失败，错误代码: %d.\n", WSAGetLastError());
			return false;
		}

		// 将Listen Socket绑定至完成端口中
		if( NULL== ::CreateIoCompletionPort((HANDLE)m_pListenContext->m_Socket, m_hIOCompletionPort,(DWORD)m_pListenContext, 0))  
		{  
			this->_ShowMessage("绑定 Listen Socket至完成端口失败！错误代码: %d/n", WSAGetLastError());  
			RELEASE_SOCKET( m_pListenContext->m_Socket );
			return false;
		}

		// 填充地址信息
		ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
		ServerAddress.sin_family = AF_INET;
		// 这里可以绑定任何可用的IP地址，或者绑定一个指定的IP地址 
		//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
		ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.GetString());         
		ServerAddress.sin_port = htons(m_nPort);                          

		// 绑定地址和端口
		if (SOCKET_ERROR == ::bind(m_pListenContext->m_Socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
		{
			this->_ShowMessage("bind()函数执行错误.\n");
			return false;
		}

		// 开始进行监听
		if (SOCKET_ERROR == listen(m_pListenContext->m_Socket,SOMAXCONN))
		{
			this->_ShowMessage("Listen()函数执行出现错误.\n");
			return false;
		}


		// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
		// 所以需要额外获取一下函数的指针，
		// 获取AcceptEx函数指针
		DWORD dwBytes = 0;  
		if(SOCKET_ERROR == WSAIoctl(
			m_pListenContext->m_Socket, 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidAcceptEx, 
			sizeof(GuidAcceptEx), 
			&m_lpfnAcceptEx, 
			sizeof(m_lpfnAcceptEx), 
			&dwBytes, 
			NULL, 
			NULL))  
		{  
			this->_ShowMessage("WSAIoctl 未能获取AcceptEx函数指针。错误代码: %d\n", WSAGetLastError()); 
			this->_DeInitialize();
			return false;  
		}  

		// 获取GetAcceptExSockAddrs函数指针，也是同理
		if(SOCKET_ERROR == WSAIoctl(
			m_pListenContext->m_Socket, 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidGetAcceptExSockAddrs,
			sizeof(GuidGetAcceptExSockAddrs), 
			&m_lpfnGetAcceptExSockAddrs, 
			sizeof(m_lpfnGetAcceptExSockAddrs),   
			&dwBytes, 
			NULL, 
			NULL))  
		{  
			this->_ShowMessage("WSAIoctl 未能获取GuidGetAcceptExSockAddrs函数指针。错误代码: %d\n", WSAGetLastError());  
			this->_DeInitialize();
			return false; 
		}  


		// 为AcceptEx 准备参数，然后投递AcceptEx I/O请求
		for( int i=0;i<MAX_POST_ACCEPT;i++ )
		{
			// 新建一个IO_CONTEXT
			PER_IO_CONTEXT* pAcceptIoContext = m_pListenContext->GetNewIoContext();

			if( false==this->_PostAccept( pAcceptIoContext ) )
			{
				m_pListenContext->RemoveContext(pAcceptIoContext);
				return false;
			}
		}
	}
}
