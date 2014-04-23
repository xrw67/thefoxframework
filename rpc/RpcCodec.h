/*
 * @brief 处理报文格式
 *
 * 消息报文格式
 * +--------+---------+---------------+
 * |[0xacac]| bodyLen |body (protobuf)|
 * +--------+---------+---------------+
 * 0        2         6            6 + bodylen
 *
 *
 */

#ifndef _THEFOX_RPC_RPCCODEC_H_
#define _THEFOX_RPC_RPCCODEC_H_

#include <base/Types.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rpc/rpc.pb.h>
#include <rpc/common.h>

namespace thefox
{

#define PROTOCOL_ID 0xacac

class RpcCodec
{
public:
	// 解码
	static bool parseFromArray(const char *buf, size_t &bufLen, rpc::Box *msg)
	{
		assert(NULL != msg || NULL != buf);

		if (bufLen <= sizeof(int16_t) + sizeof(int32_t))
			return false;

		if (PROTOCOL_ID != (uint16_t)asInt16(buf))
			return false;
		
		int32_t bodyLen = asInt32(buf + sizeof(int16_t));
		
		if ((bodyLen + sizeof(int16_t) + sizeof(int32_t)) > bufLen)
			return false;
		
		const char *body = buf + sizeof(int16_t) + sizeof(int32_t);
			
		if (msg->ParseFromArray(body, bodyLen)) {
			bufLen = bodyLen + sizeof(int16_t) + sizeof(int32_t);
			return true;
		}
		
		return false;
	}

	// 编码
	static String encode(const rpc::Box &msg)
	{
		String result;
		result.resize(sizeof(uint16_t) + sizeof(int32_t));
		
		if (msg.AppendToString(&result)) {
			uint16_t protocolId = ::htons(PROTOCOL_ID);
			int32_t bodyLen = ::htonl(result.size() - sizeof(uint16_t) - sizeof(int32_t));
		
			std::copy(reinterpret_cast<char*>(&protocolId),
					reinterpret_cast<char*>(&protocolId) + sizeof(protocolId),
					result.begin());
			std::copy(reinterpret_cast<char*>(&bodyLen),
					reinterpret_cast<char*>(&bodyLen) + sizeof(bodyLen),
					result.begin() + sizeof(protocolId));
		} else {
			result.clear();
		}
		
		return result;
	}
	
	static bool isValid(const char *buf, size_t bufLen) 
	{
		if (PROTOCOL_ID != (uint16_t)asInt16(buf))
			return false;
		int32_t bodyLen = asInt32(buf + sizeof(int16_t));
		
		if ((bodyLen + sizeof(int16_t) + sizeof(int32_t)) <= bufLen)
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
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcCodec);
};

} // namesapce thefox

#endif // _THEFOX_RPC_RPCCODEC_H_