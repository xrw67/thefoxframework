/*
 * @filename File.h
 * @brief 文件指针包装类
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_BASE_FILE_H_
#define _THEFOX_BASE_FILE_H_

#include <base/common.h>

namespace thefox 
{

// 包装FILE指针，符合RAII原则
class FileHandle
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
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(FileHandle);
    FILE *_fp
};

class File
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
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(File);
    FileHandle _file;
};

} // namespace thefox

#endif // _THEFOX_BASE_FILE_H_