#include <net/Acceptor.h>

using namespace thefox

Acceptor::Acceptor(const InetAddress &listenAddr)
	: _socket(WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))
	, _listening(false)
{
	GUID guidAcceptEx = WSAID_ACCEPTEX;  
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	
	// 获取AcceptEx函数指针
	DWORD dwBytes = 0;  
	if (SOCKET_ERROR == WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guidAcceptEx, sizeof(guidAcceptEx), 
			&_lpfnAcceptEx, sizeof(_lpfnAcceptEx), 
			&dwBytes, NULL, NULL))  
	{  
		// failed
	}  
	
	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if (SOCKET_ERROR == WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guidGetAcceptExSockAddrs, sizeof(guidGetAcceptExSockAddrs), 
			&_lpfnGetAcceptExSockAddrs, sizeof(_lpfnGetAcceptExSockAddrs),   
			&dwBytes, NULL, NULL))  
	{  
		// failed
	}
	
	// 填充地址信息
	struct sockaddr_in serverAddress;
	ZeroMemory((char *)&serverAddress, sizeof(serverAddress));
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
}

Acceptor::~Acceptor()
{
	
}

Acceptor::listen()
{
	_listening = true;
	
	if (SOCKET_ERROR == listen(m_pListenContext->m_Socket,SOMAXCONN))
	{
		
	}
	
	for (int i = 0; i < kMaxPostAccept; ++i)
	{
		IoContext *acceptIoContext = new;
		
		
		
	}
}