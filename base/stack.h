/**
* @filename stack.h
* @brief 使用数组实现的栈
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_BASE_STACK_H_
#define _THEFOX_BASE_STACK_H_

#include <vector>
#include <base/mutex.h>

namespace thefox {

/// @brief 栈的实现类
template<typename T>
class Stack
{
public:
    Stack()
    {}
    ~Stack()
    {}

    /// @brief 入栈
    /// param[in] value 需要入栈的值
    void push(const T &value)
    {
		MutexGuard lock(_mutex);
		_data.push_back(value);
	}
    
    /// @brief 弹出栈顶
    void pop()
    {
		MutexGuard lock(_mutex);
        if (!_data.empty())
            _data.pop_back();
    }
    
    /// @brief 得到栈顶数据
    /// @param[out] value 得到的栈顶的值
    /// @return 成功返回true，否则返回false
    bool getTop(T &value)
    {
		MutexGuard lock(_mutex);
        if (!_data.empty()) {
            value = _data[_data.size() - 1];
            return true;
        }
        return false;
    }
    
    /// @brief 得到栈中指定位置的值
    /// @param[in] position 需要得到值的位置
    ///                     从正数1开始递增，表示从栈顶向下遍历的位置
    ///                     从负数-1开始递减，表示从栈底向上遍历的位置
    /// @param[out] value 得到position位置上的值
    /// @return 成功返回true，否则返回false
    bool getAt(const int position, T &value)
    {
		MutexGuard lock(_mutex);
        size_t index = 0;
        if (getIndexByPosition(position, index)) {
            value = _data[index];
            return true;
        }
        return false;
    }
    
    /// @brief 给栈中指定位置赋值
    /// @param[in] position 需要赋值的位置
    ///                     从正数1开始递增，表示从栈顶向下遍历的位置
    ///                     从负数-1开始递减，表示从栈底向上遍历的位置
    /// @param[out] value 待设置的值
    /// @return 成功返回true，否则返回false
    bool setAt(const int position, const T &value)
    {
		MutexGuard lock(_mutex);
        size_t index = 0;
        if (getIndexByPosition(position, index)) {
            _data[index] = value;
            return true;
        }
        return false;
    }
    
    /// @brief 得到栈中数据个数
    /// @return 返回栈中数据个数
    size_t size()
    {
		MutexGuard lock(_mutex);
		return _data.size(); 
	}
    
    /// @brief 判断是否是空栈
    /// @return 如果栈不为空返回true，否则返回false
    bool empty()
    { 
		MutexGuard lock(_mutex);
		return _data.empty();
	}
    
private:
    // 通过Position得到在vector中的索引
    bool getIndexByPosition(const int position, size_t &index) const
    {
        if (_data.empty()) {
            return false;
        }
        
        if (position > 0) { // 从栈顶向下遍历
            int i = static_cast<int>(_data.size() - position);
            if (i >= 0) {
                index = i;
                return true;
            }
        } else if (position < 0) { //从栈底向上遍历
            if (static_cast<int>(_data.size()) + position >= 0) {
                index = (position * -1) - 1;
                return true;
            }
        }
        return false;
    }
    
    std::vector<T> _data;
	Mutex _mutex;
};

} // namespace thefox

#endif // _THEFOX_BASE_STACK_H_
