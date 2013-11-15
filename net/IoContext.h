/*
* @file IoContext.h
* @brief 表示一个IO操作
*/
#ifndef _THEFOX_NET_IOCONTEXT_H_
#define _THEFOX_NET_IOCONTEXT_H_

namespace thefox
{
	
class IoContext
{
public:
	OVERLAPPED _overlapped;
	WSABUF _wsaBuf;
	char _data[kMaxBufLen];
	
public:
	enum IoType {Init,Accept,Read,ReadComplete,Write,WriteComplete};
	
	IoContext(IoType ioType)
	: _ioType(ioType)
	{
		_wsaBuf.buf = _data;
		_wsaBuf.len = kMaxBufLen;
	}
	~IoContext(void);
private:
	static const size_t kMaxBufLen = 8192;
	IoType _ioType
};

}

#endif // _THEFOX_NET_IOCONTEXT_H_