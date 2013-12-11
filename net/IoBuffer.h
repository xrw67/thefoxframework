/*
* @file IoBuffer.h
* @brief 表示一个IO操作
*/
#ifndef _THEFOX_NET_IOCONTEXT_H_
#define _THEFOX_NET_IOCONTEXT_H_

#include <base/Types.h>
#include <net/winapi.h>

namespace thefox
{

#define MAX_IO_BUFFER_SIZE 8192

enum IoOperation{
	None, 
	Init,
	Accept,
	Read, 
	ReadCompleted, 
	Write, 
	WriteCompleted,
	ZeroByteRead, 
	ZeroReadCompleted
};

class IoBuffer
{
public:
	OVERLAPPED _overlapped;
	WSABUF _wsabuf;
	SOCKET _socket; // 只有Accept类型使用
	char _data[MAX_IO_BUFFER_SIZE];
	size_t _bytesOfUsed;
	IoOperation _op;
	uint32_t _sequence;

	IoBuffer(IoOperation op = None) 
		: _op(_op)
		, _bytesOfUsed(0)
	{
		_wsabuf.buf = _data;
		_wsabuf.len = 0;
		resetOverlapped();
		resetBuffer();
	}
	
	~IoBuffer() { _bytesOfUsed = 0; }
	
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
		if ((MAX_IO_BUFFER_SIZE - _bytesOfUsed) < len)
			return false;
		
		memcpy(_data + _bytesOfUsed, data, len);
		_bytesOfUsed += len;
		_wsabuf.len = _bytesOfUsed;
		return true;
	}
	
	void setOperation(IoOperation op) { _op = op; }
	const IoOperation getIoOperation() const {return _op; }
	
	void setSequence(uint32_t num) { _sequence = num; }
	const uint32_t getSequence() const { return _sequence; }
};

}

#endif // _THEFOX_NET_IOCONTEXT_H_