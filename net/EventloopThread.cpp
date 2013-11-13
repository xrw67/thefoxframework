#include <net/EventloopThread.h>
#include <net/Eventloop.h>

using namespace thefox;

EventloopThread::EventloopThread()
	: _loop(nullptr)
	, _exiting(false)
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
	::CreateThread(NULL, 0, threadProc, NULL, 0, 0);
}

DWORD WINAPI EventloopThread::threadProc(LPVOID lpParameter)
{
	Eventloop loop;
	_loop = &loop;
	
	loop.loop();
	_loop = nullptr;	
}