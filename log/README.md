日志操作库
===============

使用：
	#include "stdafx.h"
	#include <log/LogFile.h>

	int main(int argc, char* argv[])
	{
		thefox::LogFile log("logs","TestLog"); // 在该变量生命周期内使用才有效
		Sleep(rand()%50);
		THEFOX_LOG(TRACE) <<"dddddddd" << "dfdfd";
		THEFOX_LOG(DEBUG) << "dfdf" << 12 << -12.23;
		THEFOX_LOG(INFO) << "dfdf";
		THEFOX_LOG(WARN) << "dfdf";
		THEFOX_LOG(ERROR) << "dfdf";
		return 0;
	}