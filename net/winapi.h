#ifndef _THEFOX_NET_WINAPI_H_
#define _THEFOX_NET_WINAPI_H_

#include <map>
#include <list>
#include <vector>
#include <memory>
#include <functional>

// winsock2 和 windows的顺序不能颠倒
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>

#include <base/Timestamp.h>
#include <base/MutexLock.h>

#endif // _THEFOX_NET_WINAPI_H_