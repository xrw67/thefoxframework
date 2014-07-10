#ifndef _THEFOX_DEBUG_WIN_H_
#define _THEFOX_DEBUG_WIN_H_

#ifndef WIN32
#error "debug_win.h is Windows only"
#endif

#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>

#pragma comment(lib, "Dbghelp.lib")

namespace thefox {
          
LONG WINAPI thefoxUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
    LONG ret = EXCEPTION_EXECUTE_HANDLER;
    
    char szFileName[64];
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    _snprintf(szFileName, sizeof(szFileName), "%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d.dmp", 
                         st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand()%100);
    
    HANDLE hFile = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hFile != INVALID_HANDLE_VALUE) {
       MINIDUMP_EXCEPTION_INFORMATION ExInfo;
    
       ExInfo.ThreadId = ::GetCurrentThreadId();
       ExInfo.ExceptionPointers = lpExceptionInfo;
       ExInfo.ClientPointers = false;
    
       // write the dump
       MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
      
       ::CloseHandle(hFile);
    }
    
    return ret;
}

} // namespace thefox

#define THEFOX_SET_UNHANDLED_EXCEPTION_FILTER ::SetUnhandledExceptionFilter(thefox::thefoxUnhandledFilter)

#endif // _THEFOX_DEBUG_WIN_H_
