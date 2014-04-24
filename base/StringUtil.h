#ifndef _THEFOX_BASE_STRING_UTIL_H_
#define _THEFOX_BASE_STRING_UTIL_H_

#include <stdlib.h>
#include <stdarg.h>
#include <base/Types.h>

namespace thefox
{

class StringUtil
{
public:
    static const int kMaxStringLen = 100 * 1024;

    /// @brief 转换成int类型
    static int stoi(const String &str) 
    { 
        return (0 == str.length()) ? 0 : atoi(str.c_str());
    }

    /// @brief 转换成unsigned int
    static unsigned int stoui(const String &str) 
    { 
        return (0 == str.length()) ? 0 : static_cast<unsigned int>(atoi(str.c_str()));
    } 

    /// @brief 转换成long
    static long stol(const String &str) 
    { 
        return (0 == str.length()) ? 0L : atol(str.c_str()); 
    }

    /// @brief 转换成float
    static float stof(const String &str)
    { 
        return (0 == str.length()) ? 0.0f : static_cast<float>(atof(str.c_str())); 
    }

    /// @brief 转换成double
    static double stod(const String &str) 
    { 
        return (0 == str.length()) ? 0.0 : atof(str.c_str()); 
    }

    /// @brief 转换成bool
    static bool stob(const String &str) 
    { 
        return (0 == str.length() || str == "0" || str == "false" || str == "FALSE") ? false : true; 
    }

    /// @brief 将int类型数据转成字符串
    static String toString(const int val)
    {
        char buf[32] = {0};
		snprintf(buf, sizeof(buf), "%d", val);
		return buf;
    }

    /// @brief 将unsigned int类型数据转成字符串
    static String toString(const unsigned int val)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%u", val);
        return buf;
    }

    /// @brief 将long类型数据转成字符串
    static String toString(const long val)
    {
        char buf[32] = {0};
		snprintf(buf, sizeof(buf), "%ld", val);
		return buf;
    }

    /// @brief 将long long类型数据转成字符串
    static String toString(const long long val)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%lld", val);
        return buf;
    }

    /// @brief 将double类型数据转成字符串
    static String toString(const double val)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%f", val);
        return buf;
    }

    /// @brief 将bool类型数据转成字符串
    static String toString(const bool val)
    {
        return val ? "1" : "0";
    }

    /// @brief 格式化字符串
    static String &format(String &str, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);

        char *buf = (char *)malloc(kMaxStringLen);
        if (buf) {
            vsnprintf(buf, kMaxStringLen, format, ap);
            str = buf;
            free(buf);
            va_end(ap);
        }
        return str;
    }

    /// @brief 移除左侧的空格、换行符和制表符
    static String trimLeft(const String &str)
    {
        String::size_type index = str.find_first_not_of("\n\r\t");
        if (index != String::npos)
            return str.substr(index);
        return str;
    }

    /// @brief 移除右侧的空格、换行符和制表符
    static String trimRight(const String &str)
    {
        String::size_type index = str.find_last_not_of("\n\r\t");
        if (index != String::npos)
            return str.substr(0, index + 1);
        return str;
    }

    /// @brief 移除左右两侧的空格、换行符和制表符
    static String trim(const String &str)
    {
        return trimRight(trimLeft(str));
    }

    /// @brief 判断字符串是否都是可显示的字符
    static bool canDisplay(const String &str)
    {
        for (size_t i=0; i < str.length();i++) {
            if (!(str[i] >= 0x20 && str[i] <= 127))
                return false;
        }
        return true;
    }

    /// @brief 是否包含大写字母
    static bool isContainUppercase(const String &str)
    {
        for (size_t i=0; i < str.length();i++) {
            if (str[i] >= 'A' && str[i] <= 'Z')
                return true;
        }
        return false;
    }

    /// @brief 是否包含小写字母
    static bool isContainLowercase(const String &str)
    {
        for (size_t i=0; i < str.length();i++) {
            if (str[i] >= 'a' && str[i] <= 'z')
                return true;
        }
        return false;
    }

    /// @brief 是否是整型数据
	static bool isInteger(const String &str)
    {
        for (size_t i=0; i < str.length();i++) {
             if (str[i] >= '9' || str[i] <= '0')
                return false;
        }
        return true;

    }

    /// @brief 转成小写字母
    static String toLower(String &str)
    {
        for (String::size_type i = 0; i < str.length(); ++i)
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] += 0x20;
        return str;
    }

    /// @brief 转成大写字母
    static String toUpper(String &str)
    {
        for (String::size_type i = 0; i < str.length(); ++i)
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] -= 0x20;
        return str;
    }

    /// @brief 字符是不是字母
    static bool charIsLetter(char c)
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            return true;
        return false;
    }

    /// @brief 两字符串是否相等，忽略大小写
    static bool equalsIgnoreCase(const String &s1, const String &s2)
    {
        if (s1.length() != s2.length())
            return false;
        
        for (String::size_type i = 0; i < s1.length(); ++i) {
            if (s1[i] == s2[i])
                continue;
            if (!charIsLetter(s1[i]) || !charIsLetter(s2[i]))
                return false;
            if (0x20 != abs(s1[i] - s2[i]))
                return false;
        }
        return true;
    }

    /// @brief 替换字符串中的文字
    static String replace(String &str, const String &from, const String &to)
    {
        String::size_type pos = 0;
        while ((pos = str.find(from, pos)) != -1) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
        return str;
    }

    static void swap(String &ls, String &rs)
    {
        String temp(ls);
        ls = rs;
        rs = temp;
    }
};

inline String operator+(const char *ls, const String &rs)
{
    String buf(ls);
    buf += rs;
    return buf;
}

inline bool operator==(const char *ls, const String &rs)
{
    return !rs.compare(ls);
}

inline bool operator==(const String &ls, const char *rs)
{
    return !ls.compare(rs);
}
    
} // namespace thefox

#endif // _THEFOX_BASE_STRING_UTIL_H_
