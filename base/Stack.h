/*
 * @filename Stack.h
 * @brief 使用数组实现的栈
 * @author macwe@qq.com
 */
#ifndef _THEFOX_STACK_H_
#define _THEFOX_STACK_H_

namespace thefox
{
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
    { _data.push_back(value); }
    
    /// @brief 弹出栈顶
    void pop()
    {
        if (!_data.empty())
            _data.pop_back();
    }
    
    /// @brief 得到栈顶数据
    /// @return 成功返回栈顶的数据，否则返回NULL
    const T *top() const
    {
        if (_data.empty())
            return NULL;
        return _data[_data.size() - 1];
    }
    
    /// @brief 得到栈中指定位置的值
    /// @param[in] position 需要得到值的位置
    ///                     从正数1开始递增，表示从栈顶向下遍历的位置
    ///                     从负数-1开始递减，表示从栈底向上遍历的位置
    /// @return 成功返回对应的值，失败则返回NULL
    const T *at(const int position) const
    {
        if (_data.empty())
            return NULL;
        
        if (position > 0) // 从栈顶向下遍历
        {
            const int index = static_cast<int>(_data.size() - position);
            if (index >= 0)
                return _data[index];
        }
        else if (position < 0) //从栈底向上遍历
        {
            if (static_cast<int>(_data.size()) + position >= 0)
                return _data[(index * -1) - 1];
        }

        return NULL;
    }
    
    /// @brief 得到栈中数据个数
    /// @return 返回栈中数据个数
    const size_t size() const
    { return _data.size(); }
    
    /// @brief 判断是否是空栈
    /// @return 如果栈不为空返回true，否则返回false
    const bool empty() const
    { return _data.empty(); }
    
    
private:
	std::vector<T> _data;
};
} // namespace thefox

#endif
