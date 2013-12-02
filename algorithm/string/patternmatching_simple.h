/*
* @brief 简单模式匹配算法
* @author macwe@qq.com
*/

size_t indexBF(const String &str, const String &patterns, size_t beginPos = 0)
{
	if (str.length() < patterns.length() || str.empty())
		return -1;
		
	int i = beginPos;
	
	while (str[i]) {
		for (int n = 0; str[i + n] == patterns[n]; ++n) {
			if (!patterns[n + 1])
				return i;
		}
		++i;
	}
	return -1;
}