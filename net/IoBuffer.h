/*
* @file ioBuffer.h
* @brief 表示一个IO操作
*/
#ifndef _THEFOX_NET_IOBUFFER_H_
#define _THEFOX_NET_IOBUFFER_H_

namespace thefox
{
	
class IoBuffer
{
public:
	OVERLAPPED _overlapped;
	WSABUF _wasBuf;
	char _data[kMaxBufLen];
	
public:
	enum IoType {Init,Read,ReadComplete,Write,WriteComplete};
	
	IoBuffer(void);
	~IoBuffer(void);
private:
	static const size_t kMaxBufLen = 8192;
	IoType _ioType
}

}

#endif // _THEFOX_NET_IOBUFFER_H_