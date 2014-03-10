#include <net/Event.h>
#include <net/TcpConnection.h>

using namespace thefox;
using namespace thefox::net;

Event::Event()
    : _event(0)
{
}

Event::~Event()
{
}

void Event::handleEvent()
{
	if (_event & EVENT_READ)
		_conn->handleRead();
	if (_event & EVENT_READ_COMPLETE)
		_conn->handleReadComplete();
	if (_event & EVENT_WRITE)
		_conn->handleWrite();
	if (_event & EVENT_WRITE_COMPLETE)
		_conn->handleWriteComplete();
	if (_event & EVENT_ZERO_BYTE_READ)
		_conn->handleZeroByteRead();
    if (_event & EVENT_ZERO_BYTE_READ_COMPLETE)
		_conn->handleZeroByteReadComplete();
	if (_event & EVENT_CLOSE)
		_conn->handleClose();
}