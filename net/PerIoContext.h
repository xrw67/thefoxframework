#ifndef _THEFOX_NET_PER_IO_CONTEXT_H_
#define _THEFOX_NET_PER_IO_CONTEXT_H_

namespace thefox
{
	
class PerIoContext
{
	
public:
	enum { kMaxBufLen = 8192 };
	
	PerIoContext() : _bytesUsed(0)
	{
		_wsaBuf.buf = _data;
		_wsaBuf.len = 0;
		resetOverlapped();
		resetBuffer();
	}
	
	virtual ~IoBuffer(void) { _bytesOfUsed = 0; }
	
	const char *getBuffer() const { return _data; }
	void resetOverlapped() { ZeroMemory(_overlapped, sizeof(OVERLAPPED)); }
	void resetBuffer() 
	{
		ZeroMemory(&_data, sizeof(_data));
		_wsaBuf.len = 0;
	}
	
	WSABUG *getWSABuffer() { return &_wsabuf; }

	bool addData(const char *data, size_t len)
	{
		if ((kMaxBufLen - _bytesUsed) < len)
			return false;
		
		memcpy(_data + _bytesUsed, data, len);
		_bytesOfUsed += len;
		_wsabuf.len = _bytesOfUsed;
		return true;
	}
	
private:
	OVERLAPPED _overlapped;
	WSABUF _wsabuf;
	char _data[kMaxBufLen];
	size_t _bytesOfUsed;
};

}

#endif // _THEFOX_NET_PER_IO_CONTEXT_H_