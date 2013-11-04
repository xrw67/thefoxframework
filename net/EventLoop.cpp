#include <functionl>
#include <net/EventLoop.h>
#include <base/MutexLock.h>
#include <net/Channel.h>
#include <net/Poller.h>
#include <net/SocketsOps.h>
#include <net/TimerQueue.h>

using namespace thefox;


EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

EventLoop::EventLoop()
  : _looping(false),
  , _quit(false),
  , _eventHandling(false),
  , _callingPendingFunctors(false),
  , _iteration(0),
  , _threadId(CurrentThread::tid()),
  , _poller(Poller::newDefaultPoller(this)),
  , _timerQueue(new TimerQueue(this)),
  , _wakeupFd(createEventfd()),
  , _wakeupChannel(new Channel(this, wakeupFd_)),
  , _currentActiveChannel(NULL)
{
  if (t_loopInThisThread)
  {
    LOG_FATAL << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << threadId_;
  }
  else
  {
    t_loopInThisThread = this;
  }
  wakeupChannel_->setReadCallback(
      boost::bind(&EventLoop::handleRead, this));
  // we are always reading the wakeupfd
  wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
  LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
            << " destructs in thread " << CurrentThread::tid();
  ::close(wakeupFd_);
  t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!_looping);
    assertInLoopThread();
    _looping = true;
    _quit = false;  // FIXME: what if someone calls quit() before loop() ?

    while (!_quit)
    {
        _activeChannels.clear();
        _pollReturnTime = _poller->poll(kPollTimeMs, &_activeChannels);
        ++_iteration;
        // TODO sort channel by priority
        _eventHandling = true;
        for (ChannelList::iterator it = _activeChannels.begin();it != _activeChannels.end(); ++it)
        {
            _currentActiveChannel = *it;
            _currentActiveChannel->handleEvent(_pollReturnTime);
        }
        _currentActiveChannel = NULL;
        _eventHandling = false;
        doPendingFunctors();
    }

    _looping = false;
}

void EventLoop::quit()
{
    quit_ = true;
    // There is a chance that loop() just executes while(!quit_) and exists,
    // then EventLoop destructs, then we are accessing an invalid object.
    // Can be fixed using mutex_ in both places.
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return _timerQueue->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId)
{
    return _timerQueue->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    _poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (_eventHandling)
    {
        assert(_currentActiveChannel == channel ||
               std::find(_activeChannels.begin(), _activeChannels.end(), channel) == _activeChannels.end());
    }
    _poller->removeChannel(channel);
}


void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(_wakeupFd, &one, sizeof(one));
    if (n != sizeof one)
    {
        //LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        //LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    _callingPendingFunctors = true;

    {
        MutexLockGuard lock(_mutex);
        functors.swap(_pendingFunctors);
    }

    for (size_t i = 0; i < functors.size(); ++i)
    {
        functors[i]();
    }
    _callingPendingFunctors_ = false;
}

