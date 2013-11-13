#include <net/EventloopThread.h>
#include <net/Eventloop.h>

using namespace thefox;

EventloopThread::EventloopThread(TcpServer *server)
	: _loop(nullptr)
	, _exiting(false)
	, _server(server)
{
}

EventloopThread::~EventloopThread()
{
	_exiting = true;
	if (nullptr != _loop)
	{
		_loop->quit();
	}
}

void EventloopThread::startLoop()
{
	::CreateThread(NULL, 0, threadProc, _server, 0, 0);
}

DWORD WINAPI EventloopThread::threadProc(LPVOID lpParameter)
{
	Eventloop loop(repreinter_cast<TcpServer *>(lpParameter));
	_loop = &loop;
	
	loop.loop();
	_loop = nullptr;	
}