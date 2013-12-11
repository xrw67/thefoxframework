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

template<typename numeric_type> class NumericBlock
{
public:
	/// 一个ip地址块数据结构
	typedef struct Block *BlockPtr;
	struct Block
	{
		numeric_type _begin;
		numeric_type _end;
		BlockPtr _next;
		
		Block(const numeric_type begin, const numeric_type end)
			: _begin(begin)
			, _end(end)
			, _next(NULL);
		{}
	};
	
	typedef Value ValueType;
	struct Value
	{
		BlockPtr _block;
		numeric_type _value;
	};
	
	/// 迭代器
	class Iterator
	{	
	public:
		explicit Iterator(BlockPtr block,numeric_type curNum)
			: _block(block)
			, _curNum(curNum)
		{}
		
		Iterator(const Iterator &it)
		{
			_block = it._block;
			_curNum = it._curNum;
		}
		
		void operator=(const Iterator &it)
		{
			_block = it._block;
			_curNum = it._curNum;
		}
		numeric_type operator->()
		{
			return _curNum;
		}
		void operator++()
		{
			if (_curNum < _block->_end) {
				++curNum;
			} else {
				_block = _block->_next;
				if (NULL != _block)
					_curNum = _block->_begin;
				else
					_curNum = -1;
			}
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
	void add(const numeric_type begin, const numeric_type end)
	{
		// 还没有块
		if (NULL == _head) {
			_head = new Block(begin, end);
			return;
		}
		
		
	}
	
	void remove(const numeric_type &begin, const numeric_type &end)
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
	
	Iterator find(const numeric_type &ip)
	{
		
	}
	bool isExist(const numeric_type &ip)
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
