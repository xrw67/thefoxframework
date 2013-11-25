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
	
	IoBuffer(IoType ioType = None) : _ioType(ioType) {}
	virtual ~IoBuffer(void) {}
	
	void setIoType(IoType type) { _ioType = type; }
	const IoType getIoType() const {return _ioType; }
	
	void setSequenceNumber(uint32_t num) { _sequenceNumber = num; }
	const unit32_t getSequenceNumber() const { return _sequenceNumber; }
	
private:
	IoType _ioType;
	uint32_t _sequenceNumber;
};

}

#endif // _THEFOX_NET_IOCONTEXT_H_