/*
* @file IoBuffer.h
* @brief 表示一个IO操作
*/
#ifndef _THEFOX_NET_IOCONTEXT_H_
#define _THEFOX_NET_IOCONTEXT_H_

#include <net/PerIoContext.h>

namespace thefox
{

#define MAX_IO_BUFFER_SIZE 8192

enum IoType{
	None, 
	Init, 
	Read, 
	ReadCompleted, 
	Write, 
	WriteCompleted,
	ZeroByteRead, 
	ZeroReadCompleted
};

struct IoBuffer
{
	OVERLAPPED _overlapped;
	WSABUF _wsabuf;
	char _data[MAX_IO_BUFFER_SIZE];
	size_t _bytesOfUsed;
	
	PerIoContext() : _bytesOfUsed(0)
	{
		_wsabuf.buf = _data;
		_wsabuf.len = 0;
		resetOverlapped();
		resetBuffer();
	}
	
	~PerIoContext() { _bytesOfUsed = 0; }
	
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

	IoBuffer(IoType ioType = None) : _ioType(ioType) {}
	virtual ~IoBuffer(void) {}
	
	void setIoType(IoType type) { _ioType = type; }
	const IoType getIoType() const {return _ioType; }
	
	void setSequenceNumber(uint32_t num) { _sequenceNumber = num; }
	const uint32_t getSequenceNumber() const { return _sequenceNumber; }
};

}

#endif // _THEFOX_NET_IOCONTEXT_H_