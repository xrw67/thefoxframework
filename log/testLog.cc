#include <stdio.h>
#include <log/LogFile.h>
#include <base/Types.h>

using namespace thefox;

int main(int argc,char * argv[])
{
	thefox::LogFile log("logs","TestLog");
	int i=100;
	while (--i)
	{
		Sleep(rand()%50);
		THEFOX_LOG(TRACE) <<"dddddddd" << "dfdfd";
		THEFOX_LOG(DEBUG) << "dfdf" << 12 << -12.23;
		THEFOX_LOG(INFO) << "dfdf";
		THEFOX_LOG(WARN) << "dfdf";
		THEFOX_LOG(ERROR) << "dfdf";
	}
	return 0;
}
