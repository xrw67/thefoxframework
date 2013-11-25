#ifndef _THEFOX_NET_PER_IO_CONTEXT_H_
#define _THEFOX_NET_PER_IO_CONTEXT_H_

namespace thefox
{
	
class PerIoContext
{
	
public:
	enum { kMaxBufLen = 8192 };
	
	PerIoContext()
		: _bytesUsed(0)
	{
		_wsaBuf.buf = _data;
		_wsaBuf.len = 0;
		resetBuffer();
	}
	
	virtual ~IoBuffer(void)
	{}
	
	void resetBuffer() { memset(_data, 0, kMaxBufLen); }
	
	bool addData(const char *data, size_t len)
	{
		if ((kMaxBufLen - _bytesUsed) < len)
			return false;
		
		memcpy(_data + _bytesUsed, data, len);
		return true;
	}
	
private:
	IoType _ioType;
	OVERLAPPED _overlapped;
	WSABUF _wsaBuf;
	char _data[kMaxBufLen];
	size_t _bytesUsed;
};

}

#endif // _THEFOX_NET_PER_IO_CONTEXT_H_