#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <base/noncopyable.h>

namespace thefox
{

/// http://msdn.microsoft.com/en-us/library/vstudio/x98tx3cf.aspx
class CrtDebugger : noncopyable
{
public:
    CrtDebugger()
    {
        ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
        ::_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
    }
};

}