#pragma once

//Author:ArcherSC

//JLU


class MD5
{
private:
    unsigned int m_Content[16];
    unsigned int m_ContentLen;
    unsigned int m_TotalLen;
    static unsigned int m_T[64];
    unsigned int m_A, m_B, m_C, m_D;
private:
    inline void FF(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s);
    inline void GG(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s);
    inline void HH(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s);
    inline void II(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, int k, int i, int s);
    inline unsigned int roateLeft(unsigned int a, unsigned int c);

    void transform();
public:
    //初始化
    void beginCal();
    //表明不再输入数据了，将添加填充位
    void endCal();
    //初始化后，输入数据，在调用endCal前，都可以继续添加数据
    void addData(const char* data, unsigned int size);
    //结束运算后，取得编码
    char* getCode();
    void  clearMemoryHelper(char* p);
	char* getCode(char* retStr);
	char* getMD5(char* data,unsigned int size,char* retStr);
	char* toHex(char* str,char* hexStr);
};
