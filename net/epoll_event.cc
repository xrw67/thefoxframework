#include "epoll_event.h"
#include "tcp_connection.h"

EpollEvent::EpollEvent()
{}

EpollEvent::~EpollEvent()
{}

bool EpollEvent::init()
{}

bool EpollEvent::addEvent(IoEvent *ev)
{}

bool EpollEvent::postClose(IoEvent *ev)
{}

void EpollEvent::addConnection()
{}

void EpollEvent::delConnection(TcpConnection *conn)
{}

bool EpollEvent::processEvents(uint32_t timer)
{}

bool EpollEvent::updateRead(IoEvent *ev)
{}

bool EpollEvent::updateWrite(IoEvent *ev)
{}

void EpollEvent::handler(IoEvent *ev)
{}

