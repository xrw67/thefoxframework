/*
* 二分查找
*/

/// @brief 二分查找非递归算法,要求数组升序排序
/// @param[in] list[] 待查找的数组
/// @param[in] length 待查找数组的长度
/// @param[in] keyNum 需要查找的关键词
/// @return 返回查找到的索引值，如果没有找到就返回-1
template<typename T>
inline int serachBinary(const T list[], const int length, const T keyNum)
{
	assert(length > 0);
	
	int low = 0;
	int high = length - 1;
	int mid;
	while (low < high)
	{
		mid = list[(high + low) / 2];
		if (mid == keyNum)
			return mid;
		else if (mid > keyNum)
			high = mid - 1;
		else 
			low = mid + 1;
	}
	return -1;
}