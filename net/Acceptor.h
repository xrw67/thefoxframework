#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

namespace thefox
{

class Acceptor : noncopyable
{
public:
	OVERLAPPED _overlapped;
	
private:
	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; 
	SOCKET _sock;
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_