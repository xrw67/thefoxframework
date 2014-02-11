/*
* 顺序查找
*/

/// @brief 顺序查找算法
/// @param[in] list[] 待查找的数组
/// @param[in] length 待查找数组的长度
/// @param[in] keyNum 需要查找的关键词
/// @return 返回查找到的索引值，如果没有找到就返回-1
template<typename T>
inline int serachSequence(const T list[], const int length, const T keyNum)
{
	assert(length > 0);
	
	for (int i = 0; i < length; ++i)
		if (keyNum == list[index])
			return index;
	return -1;
}