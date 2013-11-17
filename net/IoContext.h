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
	static const size_t kMaxBufLen = 8192;

	OVERLAPPED _overlapped;
	WSABUF _wsaBuf;
	char _data[kMaxBufLen];
	SOCKET _socket;
public:
	enum IoType {Init, Accept, Read, Write};
	
	IoContext(IoType ioType)
	: _ioType(ioType)
	, _socket(INVALID_SOCKET)
	{
		resetBuffer();
		_wsaBuf.buf = _data;
		_wsaBuf.len = kMaxBufLen;
	}
	
	~IoContext(void)
	{
	}
	
	void SetIoType(IoType type)
	{ _ioType = type; }
	
	IoType getIoType() const {return _ioType; }
	
	void resetBuffer() { memset(_data, 0, kMaxBufLen); }
	
private:
	IoType _ioType;
};

}

#endif // _THEFOX_NET_IOCONTEXT_H_