/*
 * @filename File.h
 * @brief 文件指针包装类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_BASE_FILE_H_
#define _THEFOX_BASE_FILE_H_

#include <base/Types.h>
#include <base/noncopyable.h>

namespace thefox 
{

// 包装FILE指针，符合RAII原则
class FileHandle : noncopyable
{
public:
    FileHandle(const String &filename, const String &mode)
        : _fp(::fopen(filename.cStr(), mode.cStr()))
    {}
    ~FileHandle()
    {
        if (_fp)
            fclose(_fp);
    }
    
    FILE *handle() const { return _fp; }
    
private:
    FILE *_fp
};

class File : noncopyable
{
public:
    File(const String &filename,const String &mode )
        : _file(filename, mode)
    {}
    
    ~File() 
    {}
    
    String ReadLine();
    bool WriteLine();

private:
    FileHandle _file;
};

} // namespace thefox

#endif // _THEFOX_BASE_FILE_H_