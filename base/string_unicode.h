#ifndef _THEFOX_BASE_STRING_UNIDOCE_H_
#define _THEFOX_BASE_STRING_UNIDOCE_H_

#include <base/common.h>

namespace thefox {

inline string bgkToUtf8(const string &str)
{

}

inline string utf8ToGbk(const string &str)
{

}

inline string gbkToUnicode(const string &str)
{


}

inline string unicodeToGbk(const string &str)
{
    
}

inline bool isUtf8(const char* str, size_t length)
{
	size_t i;
	int32_t nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	uchar chr;
	bool bAllAscii = true; //如果全部都是ASCII, 说明不是UTF-8

	for(i=0;i<length;i++) {
		chr = *(str+i);
		if( (chr & 0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii = false;
		if(0 == nBytes) {//如果不是ASCII码,应该是多字节符,计算字节数
			if(chr >= 0x80) {
				if(chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if(chr >= 0xF8)
					nBytes = 5;
				else if(chr >= 0xF0)
					nBytes = 4;
				else if(chr >= 0xE0)
					nBytes = 3;
				else if(chr >= 0xC0)
					nBytes = 2;
				else
					return false;

				nBytes--;
			}
		} else { //多字节符的非首字节,应为 10xxxxxx
			if((chr & 0xC0) != 0x80)
				return false;
			nBytes--;
		}
	}
	if(nBytes > 0) { //违返规则
		return false;

	return !bAllAscii;
}

}

#endif // _THEFOX_BASE_STRING_UNIDOCE_H_