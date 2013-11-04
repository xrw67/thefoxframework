#include <net/Acceptor.h>

//#include <base/Logging.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <net/SocketsOps.h>

#include <errno.h>
#include <fcntl.h>

using namespace thefox;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
  : loop_(loop)
  , acceptSocket_(sockets::createNonblockingOrDie())
  , acceptChannel_(loop, acceptSocket_.fd())
  , listenning_(false)
  , idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(_idleFd >= 0);
  _acceptSocket.setReuseAddr(true);
  _acceptSocket.setReusePort(reuseport);
  _acceptSocket.bindAddress(listenAddr);
  _acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
	_acceptChannel.disableAll();
	_acceptChannel.remove();
	::close(_idleFd);
}

void Acceptor::listen()
{
  _loop->assertInLoopThread();
  _listenning = true;
  _acceptSocket.listen();
  _acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
	_loop->assertInLoopThread();
	InetAddress peerAddr(0);
	//FIXME loop until no more
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0)
	{
		// string hostport = peerAddr.toIpPort();
		// LOG_TRACE << "Accepts of " << hostport;
		if (newConnectionCallback_)
		{
			newConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
	else
	{
		//LOG_SYSERR << "in Acceptor::handleRead";
		// Read the section named "The special problem of
		// accept()ing when you can't" in libev's doc.
		// By Marc Lehmann, author of livev.
		if (errno == EMFILE)
		{
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}

