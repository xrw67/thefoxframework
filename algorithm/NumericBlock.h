/*
*
* @brief 数字块列表
* @author macwe@qq.com
*/
#ifndef _THEFOX_NUMERIC_BLOCK_H_
#define _THEFOX_NUMERIC_BLOCK_H_

#include <base/Types.h>

namespace thefox
{

template<typename NumericType> class NumericBlock
{
public:
	/// 一个ip地址块数据结构
	typedef struct Block *BlockPtr;
	struct Block
	{
		NumericType begin;
		NumericType end;
		BlockPtr next;
		
		Block(const NumericType begin, const NumericType end)
			: begin(begin)
			, end(end)
			, next(NULL);
		{}
	};
	
	typedef Value ValueType;
	struct Value
	{
		BlockPtr _block;
		NumericType _num;
	};
	
	/// 迭代器
	class Iterator : public std::iterator<std::input_iterator_tag, ValueType>
	{	
	public:
		Iterator(const ValueType &v)
		 :_value(v)
		 {}
		 
		Iterator(BlockPtr block,NumericType num)
		{
			_value._block = block;
			_value._num = num;
		}
		
		NumericType &operator*()
		{
			return _value._num;
		}
		Iterator &operator++()
		{
			BlockPtr &block = _value._block;
			NumericType &num = _value._num;
			
			if (num < block->_end) {
				++num;
			} else {
				block = block->_next;
				if (NULL != block)
					num = block->_begin;
				else
					num = -1;
			}
			return _value;
		}

	private:
		ValueType _value;
	};
	
	NumericBlock()
		: _head(NULL);
	{
	}
	~NumericBlock()
	{
		if (NULL != _head)
			remove(_head);
	}
	void add(const NumericType begin, const NumericType end)
	{
		// 还没有块
		if (NULL == _head) {
			_head = new Block(begin, end);
			return;
		}
		
		BlockPtr cur = _head;
		
		while (1) {
			if (begin < cur->begin) {
				cur = cur->next;
				continue;
			}
		
		}
	}
	
	void remove(const NumericType &begin, const NumericType &end)
	{
		
	
	}
	
	Iterator begin()
	{
		if (_head)
			return Iterator(_head, _head->_begin);
		else
			return Iterator(NULL, -1);
	}
	
	Iterator end()
	{
		return Iterator(NULL, -1);
	}
	
	Iterator find(const NumericType &ip)
	{
		
	}
	bool isExist(const NumericType &ip)
	{
		Iterator it
	}
	
private:
	void 
	void remove(BlockPtr *block)
	{
		if (NULL != block->next)
			remove(block->next);
		delete block;
	}
	
	BlockPtr *_head;
};

}

#endif //! _THEFOX_NUMERIC_BLOCK_H_
