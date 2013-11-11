#ifndef _THEFOX_NET_IOCPSOCKET_H_
#define _THEFOX_NET_IOCPSOCKET_H_

namespace thefox
{
class IocpSocket
{
public:
	bool InitIocp();
	
	HANDLE _ioCompletionPort;	
};

}

#endif // _THEFOX_NET_IOCPMODEL_H_s