/*
 * @brief 这里放一些最常用的头文件、宏、函数等，需要C++11的支持才行
 * @author macwe1024 at gmail dot com
 *
 */

#ifndef _THEFOX_BASE_COMMON_H_
#define _THEFOX_BASE_COMMON_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <functional>

#ifndef NDEBUG
#include <assert.h>
#endif

#ifdef WIN32
	#pragma warning(disable:4819)
	#pragma warning(disable:4996)
	#define snprintf _snprintf

	#if _MSC_VER < 1500 // MSVC 2008
		#define vsnprintf _vsnprintf
	#endif
#endif

namespace thefox 
{

using std::string;
using namespace std::placeholders; //std::bind可替换参数的占位符

// 指定是否允许copy ctor 和 assign opt.
#undef THEFOX_DISALLOW_EVIL_CONSTRUCTORS
#define THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
    TypeName(const TypeName&);                         \
    void operator=(const TypeName&)

// 安全删除对象
#define SAFE_DELETE(p)        \
    if (NULL != p) {          \
        delete p;             \
        p = NULL;             \
    }
    
// 安全删除对象数组
#define SAFE_DELETE_ARRAY(p)  \
    if (NULL != p) {          \
        delete []p;           \
        p = NULL;             \
    }

#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define MAX(a,b) ((a)>(b)) ? (a) : (b)

} // namespace thefox

#endif // _THEFOX_BASE_COMMON_H_
