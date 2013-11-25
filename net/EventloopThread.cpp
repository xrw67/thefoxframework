#include <net/EventloopThread.h>
#include <net/Eventloop.h>

using namespace thefox;

EventloopThread::EventloopThread()
	: _loop(NULL)
	, _exiting(false)
{
}

EventloopThread::~EventloopThread()
{
	_exiting = true;
	if (NULL != _loop)
		_loop->quit();
}

void EventloopThread::startLoop(IoCompletionPort * const iocpPtr)
{
	::CreateThread(NULL, 0, threadProc, iocpPtr, 0, 0);
}

DWORD WINAPI EventloopThread::threadProc(LPVOID iocpPtr)
{
	Eventloop loop(repreinter_cast<IoCompletionPort *>(iocpPtr));
	_loop = &loop;
	
	loop.loop();
	_loop = NULL;	
}