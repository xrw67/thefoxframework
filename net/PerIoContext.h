#ifndef _THEFOX_NET_PER_IO_CONTEXT_H_
#define _THEFOX_NET_PER_IO_CONTEXT_H_

namespace thefox
{
	
class PerIoContext
{
	
public:
	OVERLAPPED _overlapped;
	enum { kMaxBufLen = 8192 };
	
	PerIoContext() : _bytesOfUsed(0)
	{
		_wsabuf.buf = _data;
		_wsabuf.len = 0;
		resetOverlapped();
		resetBuffer();
	}
	
	virtual ~PerIoContext() { _bytesOfUsed = 0; }
	
	const char *getBuffer() const { return _data; }
	void resetOverlapped() { ZeroMemory(&_overlapped, sizeof(OVERLAPPED)); }
	void resetBuffer() 
	{
		ZeroMemory(&_data, sizeof(_data));
		_wsabuf.len = 0;
	}
	
	WSABUF *getWSABuffer() { return &_wsabuf; }

	bool addData(const char *data, size_t len)
	{
		if ((kMaxBufLen - _bytesOfUsed) < len)
			return false;
		
		memcpy(_data + _bytesOfUsed, data, len);
		_bytesOfUsed += len;
		_wsabuf.len = _bytesOfUsed;
		return true;
	}
	
private:
	WSABUF _wsabuf;
	char _data[kMaxBufLen];
	size_t _bytesOfUsed;
};

}

#endif // _THEFOX_NET_PER_IO_CONTEXT_H_