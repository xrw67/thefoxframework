#ifndef _THEFOX_NET_PER_IO_CONTEXT_H_
#define _THEFOX_NET_PER_IO_CONTEXT_H_

namespace thefox
{
class PerIoContext
{
public:
	enum { kMaxBufLen = 8192 };

	OVERLAPPED _overlapped;
	WSABUF _wsaBuf;
	char _data[kMaxBufLen];
	SOCKET _socket;
public:
	enum IoType {Init, Accept, Read, Write};
	
	IoBuffer(IoType ioType)
	: _ioType(ioType)
	, _socket(INVALID_SOCKET)
	{
		resetBuffer();
		_wsaBuf.buf = _data;
		_wsaBuf.len = kMaxBufLen;
	}
	
	~IoBuffer(void)
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

#endif // _THEFOX_NET_PER_IO_CONTEXT_H_