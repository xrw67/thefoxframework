
#ifndef _THEFOX_NET_BUFFER_H_
#define _THEFOX_NET_BUFFER_H_

#ifdef WIN32
#pragma warning(disable:4996)
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <base/types.h>

namespace thefox
{

///
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// beginPtr   <=    readerPtr    <=    writerPtr    <=     endPtr
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer()
    {
		size_t len = kCheapPrepend + kInitialSize;
		_beginPtr = static_cast<char *>(malloc(len));
		_endPtr = _beginPtr + len;
		_readerPtr = _writerPtr = _beginPtr + kInitialSize;
    }
	Buffer(const Buffer &buf)
	{
		size_t len = buf._endPtr - buf._beginPtr;
		_beginPtr = static_cast<char *>(malloc(len));
		_endPtr = _beginPtr + len;
		_readerPtr = _beginPtr + (buf._readerPtr - buf._beginPtr);
		_writerPtr = _beginPtr + (buf._writerPtr - buf._beginPtr);
	}

	~Buffer()
	{
		if (NULL != _beginPtr)
			free(_beginPtr);
		_beginPtr = _endPtr = _readerPtr = _writerPtr = NULL;
	}

	const Buffer &operator=(const Buffer &buf)
	{
		size_t readable = buf.readableBytes();
		if (static_cast<size_t>(_endPtr - _beginPtr) < (readable + kCheapPrepend)) {
			size_t newSize = readable + kCheapPrepend;
			free(_beginPtr);
			_beginPtr = static_cast<char *>(malloc(newSize));
			_endPtr = _beginPtr + newSize;
		}

		memcpy(_beginPtr + kCheapPrepend, buf._readerPtr, readable);
		_readerPtr = _beginPtr + kCheapPrepend;
		_writerPtr = _readerPtr + readable;
	}

    size_t readableBytes() const
    { return _writerPtr - _readerPtr; }

    size_t writableBytes() const
    { return _endPtr - _writerPtr; }

    size_t prependableBytes() const
    { return _readerPtr - _beginPtr; }

    const char* peek() const
    { return _readerPtr; }

    void retrieve(size_t len)
    {
        if (len < readableBytes())
            _readerPtr += len;
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
        _readerPtr = _beginPtr + kCheapPrepend;
        _writerPtr = _beginPtr + kCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void append(const void *data, size_t len)
    {
        ensureWritableBytes(len);
        memcpy(beginWrite(), (void *)data, len);
        hasWritten(len);
    }

	void append(const char *data, size_t len)
    { append((void *)data, len); }

    char* beginWrite()
    { return _writerPtr; }

    const char* beginWrite() const
    { return _writerPtr; }

    void hasWritten(size_t len)
    { _writerPtr += len; }

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
        _readerPtr -= len;
        memcpy(_readerPtr, data, len);
    }
private:
	void ensureWritableBytes(size_t len)
    {
		if (writableBytes() < len) {
			if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
				size_t readable = readableBytes();
				size_t newSize = (_writerPtr - _beginPtr) + len;
				char *temp = static_cast<char *>(malloc(newSize));
				memcpy(temp + kCheapPrepend, _readerPtr, readable);
				
				free(_beginPtr);
				_beginPtr = temp;
				_readerPtr = _beginPtr + kCheapPrepend;
				_writerPtr = _readerPtr + readable;
				_endPtr = _beginPtr + newSize;
			} else {
				size_t readable = readableBytes();
				memmove(_beginPtr + kCheapPrepend, _readerPtr, readable);
				_readerPtr = _beginPtr + kCheapPrepend;
				_writerPtr = _readerPtr + readable;
			}	
		}
    }

	// buffer
    char *_beginPtr;
	char *_readerPtr;
	char *_writerPtr;
	char *_endPtr;
};

} // namespace thefox

#endif  // _THEFOX_NET_BUFFER_H_
