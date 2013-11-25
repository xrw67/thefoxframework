/*
* @file IoBuffer.h
* @brief 表示一个IO操作
*/
#ifndef _THEFOX_NET_IOCONTEXT_H_
#define _THEFOX_NET_IOCONTEXT_H_

#include <net/PerIoContext.h>

namespace thefox
{
	
class IoBuffer : public PerIoContext
{
public:
	enum IoType { None, Init, Read, Write };
	
	IoBuffer(IoType ioType = None)
	: PerIoContext()
	, _ioType(ioType)
	{
		
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

#endif // _THEFOX_NET_IOCONTEXT_H_