// TimerTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <net/Eventloop.h>
#include <Windows.h>

using namespace thefox;

int _tmain(int argc, _TCHAR* argv[])
{
    HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    EventLoop loop;
    loop.start();
    loop.registerHandle(hTimer);

	return 0;
}

