
#ifndef _THEFOX_NET_BUFFER_H_
#define _THEFOX_NET_BUFFER_H_

#include <vector>
#include <algorithm>

#ifdef WIN32
#pragma warning(disable:4996)
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <base/Types.h>
#include <base/copyable.h>

namespace thefox
{

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer : public copyable
{
public:
	static const size_t kCheapPrepend = 8;
	static const size_t kInitialSize = 1024;

	Buffer()
		: _buffer(kCheapPrepend + kInitialSize)
		, _readerIndex(kCheapPrepend)
		, _writerIndex(kCheapPrepend)
	{
	}

	void swap(Buffer& rhs)
	{
		_buffer.swap(rhs._buffer);
		std::swap(_readerIndex, rhs._readerIndex);
		std::swap(_writerIndex, rhs._writerIndex);
	}

	size_t readableBytes() const
	{ return _writerIndex - _readerIndex; }

	size_t writableBytes() const
	{ return _buffer.size() - _writerIndex; }

	size_t prependableBytes() const
	{ return _readerIndex; }

	const char* peek() const
	{ return begin() + _readerIndex; }

	void retrieve(size_t len)
	{
		if (len < readableBytes())
			_readerIndex += len;
		else
			retrieveAll();
	}

	void retrieveUntil(const char* end)
	{
		retrieve(end - peek());
	}

	void retrieveInt32()
	{
		retrieve(sizeof(int32_t));
	}

	void retrieveInt16()
	{
		retrieve(sizeof(int16_t));
	}

	void retrieveInt8()
	{
		retrieve(sizeof(int8_t));
	}

	void retrieveAll()
	{
		_readerIndex = kCheapPrepend;
		_writerIndex = kCheapPrepend;
	}

	String retrieveAllAsString()
	{
		return retrieveAsString(readableBytes());;
	}

	String retrieveAsString(size_t len)
	{
		String result(peek(), len);
		retrieve(len);
		return result;
	}

	void append(const char *data, size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data, data+len, beginWrite());
		hasWritten(len);
	}

	void append(const void *data, size_t len)
	{
		append(static_cast<const char *>(data), len);
	}

	void ensureWritableBytes(size_t len)
	{
		if (writableBytes() < len)
			makeSpace(len);
	}

	char* beginWrite()
	{ return begin() + _writerIndex; }

	const char* beginWrite() const
	{ return begin() + _writerIndex; }

	void hasWritten(size_t len)
	{ _writerIndex += len; }

	///
	/// Append int32_t using network endian
	///
	void appendInt32(int32_t x)
	{
		int32_t be32 = htonl(x);
		append(&be32, sizeof(be32));
	}

	void appendInt16(int16_t x)
	{
		int16_t be16 = htons(x);
		append(&be16, sizeof(be16));
	}

	void appendInt8(int8_t x)
	{
		append(&x, sizeof x);
	}

	///
	/// Read int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t readInt32()
	{
		int32_t result = peekInt32();
		retrieveInt32();
		return result;
	}

	int16_t readInt16()
	{
		int16_t result = peekInt16();
		retrieveInt16();
		return result;
	}

	int8_t readInt8()
	{
		int8_t result = peekInt8();
		retrieveInt8();
		return result;
	}

	///
	/// Peek int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t peekInt32() const
	{
		int32_t be32 = 0;
		::memcpy(&be32, peek(), sizeof(be32));
		return ntohl(be32);
	}

	int16_t peekInt16() const
	{
		int16_t be16 = 0;
		::memcpy(&be16, peek(), sizeof(be16));
		return ntohs(be16);
	}

	int8_t peekInt8() const
	{
		int8_t x = *peek();
		return x;
	}

	void prependInt32(int32_t x)
	{
		int32_t be32 = htonl(x);
		prepend(&be32, sizeof(be32));
	}

	void prependInt16(int16_t x)
	{
		int16_t be16 = htons(x);
		prepend(&be16, sizeof(be16));
	}

	void prependInt8(int8_t x)
	{
		prepend(&x, sizeof x);
	}

	void prepend(const void* data, size_t len)
	{
		_readerIndex -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d+len, begin()+_readerIndex);
	}

	size_t internalCapacity() const
	{
		return _buffer.capacity();
	}
private:

	char* begin()
	{ return &*_buffer.begin(); }

	const char* begin() const
	{ return &*_buffer.begin(); }

	void makeSpace(size_t len)
	{
		if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
			_buffer.resize(_writerIndex+len);
		} else {
			size_t readable = readableBytes();
			std::copy(begin()+_readerIndex, begin()+_writerIndex, begin()+kCheapPrepend);
			_readerIndex = kCheapPrepend;
			_writerIndex = _readerIndex + readable;
		}
	}

	std::vector<char> _buffer;
	size_t _readerIndex;
	size_t _writerIndex;
};

} // namespace thefox

#endif  // _THEFOX_NET_BUFFER_H_
