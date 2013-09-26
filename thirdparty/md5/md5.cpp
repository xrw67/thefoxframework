#include "stdafx.h"

#include <cstdlib>
#include <cstring>
#include "MD5.h"

//Author:ArcherSC

//JLU

unsigned int MD5::m_T[64] = {
          0xD76aa478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
          0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
          0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
          0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
          0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
          0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
          0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
          0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
          0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
          0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
          0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
          0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
          0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
          0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
          0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
          0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
          };

void MD5::FF(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s)
{
    a = b + roateLeft(a + ((b & c) | ((~b) & d)) + m_Content[k] + m_T[i], s);
}

void MD5::GG(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s)
{
    a = b + roateLeft(a + ((b & d) | (c & (~d))) + m_Content[k] + m_T[i], s);
}
void MD5::HH(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s)
{
    a = b + roateLeft(a + (b ^ c ^ d) + m_Content[k] + m_T[i], s);
}
void MD5::II(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s)
{
    a = b + roateLeft(a + (c ^ (b | (~d))) + m_Content[k] + m_T[i], s);
}
unsigned int MD5::roateLeft(unsigned int a, unsigned int c)
{
    return ( (a << c) | (a >> (32-c)) );
}

void MD5::transform()
{
    unsigned int AA, BB, CC, DD;
    AA = m_A;
    BB = m_B;
    CC = m_C;
    DD = m_D;
    //1 round
    FF(AA, BB, CC, DD, 0, 0, 7);
    FF(DD, AA, BB, CC, 1, 1, 12);
    FF(CC, DD, AA, BB, 2, 2, 17);
    FF(BB, CC, DD, AA, 3, 3, 22);
    FF(AA, BB, CC, DD, 4, 4, 7);
    FF(DD, AA, BB, CC, 5, 5, 12);
    FF(CC, DD, AA, BB, 6, 6, 17);
    FF(BB, CC, DD, AA, 7, 7, 22);
    FF(AA, BB, CC, DD, 8, 8, 7);
    FF(DD, AA, BB, CC, 9, 9, 12);
    FF(CC, DD, AA, BB, 10, 10, 17);
    FF(BB, CC, DD, AA, 11, 11, 22);
    FF(AA, BB, CC, DD, 12, 12, 7);
    FF(DD, AA, BB, CC, 13, 13, 12);
    FF(CC, DD, AA, BB, 14, 14, 17);
    FF(BB, CC, DD, AA, 15, 15, 22);
    //2 round
    GG(AA, BB, CC, DD, 1, 16, 5);
    GG(DD, AA, BB, CC, 6, 17, 9);
    GG(CC, DD, AA, BB, 11, 18, 14);
    GG(BB, CC, DD, AA, 0, 19, 20);
    GG(AA, BB, CC, DD, 5, 20, 5);
    GG(DD, AA, BB, CC, 10, 21, 9);
    GG(CC, DD, AA, BB, 15, 22, 14);
    GG(BB, CC, DD, AA, 4, 23, 20);
    GG(AA, BB, CC, DD, 9, 24, 5);
    GG(DD, AA, BB, CC, 14, 25, 9);
    GG(CC, DD, AA, BB, 3, 26, 14);
    GG(BB, CC, DD, AA, 8, 27, 20);
    GG(AA, BB, CC, DD, 13, 28, 5);
    GG(DD, AA, BB, CC, 2, 29, 9);
    GG(CC, DD, AA, BB, 7, 30, 14);
    GG(BB, CC, DD, AA, 12, 31, 20);

    //3 round
    HH(AA, BB, CC, DD, 5, 32, 4);
    HH(DD, AA, BB, CC, 8, 33, 11);
    HH(CC, DD, AA, BB, 11, 34, 16);
    HH(BB, CC, DD, AA, 14, 35, 23);
    HH(AA, BB, CC, DD, 1, 36, 4);
    HH(DD, AA, BB, CC, 4, 37, 11);
    HH(CC, DD, AA, BB, 7, 38, 16);
    HH(BB, CC, DD, AA, 10, 39, 23);
    HH(AA, BB, CC, DD, 13, 40, 4);
    HH(DD, AA, BB, CC, 0, 41, 11);
    HH(CC, DD, AA, BB, 3, 42, 16);
    HH(BB, CC, DD, AA, 6, 43, 23);
    HH(AA, BB, CC, DD, 9, 44, 4);
    HH(DD, AA, BB, CC, 12, 45, 11);
    HH(CC, DD, AA, BB, 15, 46, 16);
    HH(BB, CC, DD, AA, 2, 47, 23);

    //4 round
    II(AA, BB, CC, DD, 0, 48, 6);
    II(DD, AA, BB, CC, 7, 49, 10);
    II(CC, DD, AA, BB, 14, 50, 15);
    II(BB, CC, DD, AA, 5, 51, 21);
    II(AA, BB, CC, DD, 12, 52, 6);
    II(DD, AA, BB, CC, 3, 53, 10);
    II(CC, DD, AA, BB, 10, 54, 15);
    II(BB, CC, DD, AA, 1, 55, 21);
    II(AA, BB, CC, DD, 8, 56, 6);
    II(DD, AA, BB, CC, 15, 57, 10);
    II(CC, DD, AA, BB, 6, 58, 15);
    II(BB, CC, DD, AA, 13, 59, 21);
    II(AA, BB, CC, DD, 4, 60, 6);
    II(DD, AA, BB, CC, 11, 61, 10);
    II(CC, DD, AA, BB, 2, 62, 15);
    II(BB, CC, DD, AA, 9, 63, 21);
    m_A = m_A + AA;
    m_B = m_B + BB;
    m_C = m_C + CC;
    m_D = m_D + DD;
}
//≥ı ºªØ
void MD5::beginCal()
{
    memset(m_Content, 0, 64);
    m_ContentLen = 0;
    m_TotalLen = 0;
    m_A = 0x67452301;
    m_B = 0xEFCDAB89;
    m_C = 0x98BADCFE;
    m_D = 0x10325476;
}

void MD5::endCal()
{
    memset((char*)m_Content + m_ContentLen, 0x80, 1);
    if (m_ContentLen < 56){
     memset((char*)m_Content + m_ContentLen + 1, 0, 55 - m_ContentLen);
    }
    else{
     memset((char*)m_Content + m_ContentLen + 1, 0, 63 - m_ContentLen);
     transform();
     memset((char*)m_Content, 0, 56);
    }
    unsigned int h, l;
    h = 0;
    l = m_TotalLen;
    h |= ((l & 0x80000000) >> 31);
    h <<= 1;
    l <<= 1;
    h |= ((l & 0x80000000) >> 31);
    h <<= 1;
    l <<= 1;
    h |= ((l & 0x80000000) >> 31);
    h <<= 1;
    l <<= 1;
    memcpy(m_Content + 14, &l, 4);
    memcpy(m_Content + 15, &h, 4);
    transform();
}

void MD5::addData(const char* data, unsigned int size)
{
    unsigned int i = 0;
    unsigned int remain = 64 - m_ContentLen;
    if (remain > size)
     remain = size;
    memcpy(((char*)m_Content) + m_ContentLen, data, remain);
    i += remain;
    m_ContentLen += remain;
    m_TotalLen += remain;
    if (m_ContentLen < 64)
     return;

    transform();

    while (i + 64 <= size){
     memcpy(m_Content, data + i, 64);
     transform();
     i += 64;
     m_TotalLen += 64;
    }

    m_ContentLen = size - i;
    m_TotalLen += m_ContentLen;
    memcpy(m_Content, data + i, m_ContentLen);
}

char* MD5::getCode()
{
    char * code = new char[16];
    memcpy(code, &m_A, 4);
    memcpy(code + 4, &m_B, 4);
    memcpy(code + 8, &m_C, 4);
    memcpy(code + 12, &m_D, 4);
    return code;
}

void    MD5::clearMemoryHelper(char* p)
{
    delete []p;
}

char* MD5::getCode(char* retStr){
    memcpy(retStr, &m_A, 4);
    memcpy(retStr + 4, &m_B, 4);
    memcpy(retStr + 8, &m_C, 4);
    memcpy(retStr + 12, &m_D, 4);
	return retStr;
}

char*  MD5::getMD5(char* data,unsigned int size,char* retStr){
	beginCal();
	addData(data,size);
	endCal();
	return getCode(retStr);
}

char* MD5::toHex(char* str,char* hexStr){
	int len=strlen(str);
	for(int i=0;i<16;i++){
		sprintf(hexStr+2*i,"%02X",(BYTE)str[i]);
	}
	return hexStr;
}
