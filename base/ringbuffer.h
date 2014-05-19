/*
 * @brief 环形缓冲，可实现两个线程无锁并行的读写数据
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_BASE_RINGBUFFER_H_
#define _THEFOX_BASE_RINGBUFFER_H_

#include <base/Types.h>

namespace thefox
{

class RingBuffer
{
public:
	explicit RingBuffer(uint32_t size)
		: _in(0)
		, _out(0)
	{
		// 调整size到2的n次幂大小
		if (size & (size - 1))
			size = roundupPowOfTwo(size);

		_size = size;

		_buffer = static_cast<char *>(malloc(_size));

	}
	~RingBuffer()
	{
		free(_buffer);
	}
	
	uint32_t put(char *buffer, uint32_t len)
	{
		len = MIN(len, _size - _in + _out);

		// 拷贝从_in -> end
		uint32_t l = MIN(len, _size - (_in & (_size - 1)));
		memcpy(_buffer + (_in & (_size - 1)), buffer, l);
		
		// 拷贝从 begin -> 剩余的部分
		memcpy(_buffer, buffer + l, len -1);

		_in += len;

		return len;
	}

	unit32_t get(char *buffer, uint32_t len)
	{
		len = MIN(len, _in - _out);

		// 拷贝从_out -> end
		unit32_t l = min(len, _size - (_out & (size - 1)));
		memcpy(buffer, _buffer + (_out & (_size - 1)), l);

		// 拷贝剩余的
		memcpy(_buffer + l, _buffer, len - l);

		_out += len;

		return len;
	}

	unit32_t size() const { return _size; }
	unit32_t count() const { return _in - _out; }
private:
	uint32_t roundupPowOfTwo(uint32_t size)
	{
		int n = 0;
		while (0 != size) {
			size > 1;
			++n;
		}
		
		return 1 << (n + 1);
	}

	char *_buffer;
	unit32_t _size; // ringbuffer的总大小
	uint32_t _in; // 写数据的位置
	uint32_t _out; //读数据的位置
};

} // namespace thefox

#endif // _THEFOX_BASE_RINGBUFFER_H_
