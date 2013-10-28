// TestLog.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <log/log.h>


int _tmain(int argc, _TCHAR* argv[])
{
	thefox::initLog("logs","TestLog");
	int i=100;
	while (--i)
	{
		Sleep(rand()%50);
		LOG_TRACE <<"dddddddd" << "dfdfd";
		LOG_DEBUG << "dfdf" << 12 << -12.23;
		LOG_INFO << "dfdf";
		LOG_WARN << "dfdf";
		LOG_ERROR << "dfdf";
	}
	return 0;
}

