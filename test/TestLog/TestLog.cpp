// TestLog.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <log/log.h>

int _tmain(int argc, _TCHAR* argv[])
{
	thefox::initLog("ffffff");

	LOG_TRACE <<"dddddddd";
	return 0;
}

