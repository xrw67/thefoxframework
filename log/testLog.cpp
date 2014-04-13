#include <log/log.h>
#include <stdio.h>
#include <base/Types.h>

using namespace thefox;

int main(int argc,char * argv[])
{
thefox::initLog(String("logs"),String("TestLog"));
LOG_TRACE <<"dddddddd" << "dfdfd";
LOG_DEBUG << "dfdf" << 12 << -12.23;
LOG_INFO << "dfdf";
LOG_WARN << "dfdf";
LOG_ERROR << "dfdf";
return 0;
}
