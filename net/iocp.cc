#pragma warning(disable:4996) // 禁用warning 4996

#include <net/iocp.h>
#include <net/buffer.h>
#include <net/cp_event.h>
#include <net/event_loop.h>
#include <net/tcp_connection.h>

namespace thefox
{



using namespace thefox;


void Iocp::close()
{    
    _started = false;

    if (!_connections.empty())
        postCloseEvent(_connections.begin()->second);
}


