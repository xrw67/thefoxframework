#ifndef _THEFOX_NET_NETMSG_H_
#define _THEFOX_NET_NETMSG_H_

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <net/netmsg/Head.pb.h>
#include <base/Types.h>

namespace thefox
{

namespace gpb = ::google::protobuf;

// 消息报文格式
// +--------+---------+---------+---------------+---------------+
// |[010101]| headLen | bodyLen |head (protobuf)|body (protobuf)|
// +--------+---------+---------+---------------+---------------+
// 0        2         4         8         8 + headLen
class NetMsg
{
public:
	NetMsg(void *any = NULL)
	: _head(NULL)
	, _body(NULL)
	, _any(any)
	{}

	~NetMsg()
	{
		if (NULL != _head) {
			delete _head;
			_head = NULL;
		}
		if (NULL != _body) {
			delete _body;
			_body = NULL;
		}
	}
	
	const gpb::Descriptor * getDescriptor()
	{
		if (_body)
			return _body->GetDescriptor();
		return NULL;
	}
	// 解码
	bool parseFromArray(const char *buf, size_t bufLen)
	{
		if (0xabcd != (uint16_t)asInt16(buf))
			return false;
		int16_t headLen = asInt16(buf + sizeof(int16_t));
		int32_t bodyLen = asInt32(buf + sizeof(int16_t) * 2);
		
		if ((headLen + bodyLen + sizeof(int16_t) * 2 + sizeof(int32_t)) > bufLen)
			return false;
		
		const char *head = buf + 8;
		const char *body = head + headLen;
		
		if (NULL == _head)
		_head = new Head();
		if (_head->ParseFromArray(head, headLen)) {
			if ((_body = createMessage(_head->body_type())) != NULL) {
				if (_body->ParseFromArray(body, bodyLen))
					return true;
			}
		}
		
		return false;
	}
	
	// 编码
	static String encode(Head &head, const gpb::Message &message)
	{
		String result;
		
		result.resize(8);
		
		head.set_body_type(message.GetTypeName());
		
		if (head.AppendToString(&result)) {
			int16_t headLen = ::htons(result.size() - 8);
			if (message.AppendToString(&result)) {
				int32_t bodyLen = ::htonl(result.size() - headLen - 8);
				uint16_t protocolId = 0xabcd;
				std::copy(reinterpret_cast<char*>(&protocolId),
						reinterpret_cast<char*>(&protocolId) + sizeof(protocolId),
						result.begin());
				std::copy(reinterpret_cast<char*>(&headLen),
						reinterpret_cast<char*>(&headLen) + sizeof(headLen),
						result.begin() + sizeof(protocolId));
				std::copy(reinterpret_cast<char*>(&bodyLen),
						reinterpret_cast<char*>(&bodyLen) + sizeof(bodyLen),
						result.begin() + sizeof(protocolId) + sizeof(headLen));
			}
		} else {
			result.clear();
		}
		
		return result;
	}
	
	static bool peek(const char *buf, size_t bufLen) 
	{
		if (0xabcd != (uint16_t)asInt16(buf))
			return false;
		int16_t headLen = asInt16(buf + sizeof(int16_t));
		int32_t bodyLen = asInt32(buf + sizeof(int16_t) * 2);
		
		if ((headLen + bodyLen + sizeof(int16_t) * 2 + sizeof(int32_t)) <= bufLen)
			return true;
		else
			return false;
	}

	static gpb::Message *createMessage(const String& type_name)
	{
		gpb::Message *message = NULL;
		const gpb::Descriptor* descriptor =
		gpb::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
		if (descriptor) {
			const gpb::Message* prototype =
			gpb::MessageFactory::generated_factory()->GetPrototype(descriptor);
			if (prototype)
				message = prototype->New();
		}
		return message;
	}
	
	static int16_t asInt16(const char *buf)
	{
		int16_t be16 = 0;
		::memcpy(&be16, buf, sizeof(be16));
		return ::ntohs(be16);
	}
	
	static int32_t asInt32(const char *buf)
	{
		int32_t be32 = 0;
		::memcpy(&be32, buf, sizeof(be32));
		return ::ntohl(be32);
	}
private:
	Head *_head;
	gpb::Message *_body;
	void *_any;
};

} // namespace thefox

#endif // _THEFOX_NET_NETMSG_H_