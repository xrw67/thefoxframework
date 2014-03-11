#include <net/TcpServer.h>
#include <base/Types.h>
#include <net/Iocp.h>
#include <net/Socket.h>
#include <net/EventLoop.h>
#include <net/TcpConnection.h>


using namespace thefox;
using namespace thefox::net;

TcpServer::TcpServer(const String &nameArg, InetAddress listenAddr)
    : _listenAddr(listenAddr)
    , _name(nameArg)
    , _nextConnId(1)
    , _iocp(new Iocp())
    , _listenSocket(new Socket(Socket::Create()))
{
    _iocp->associateSocket(*_listenSocket, 0);
}

TcpServer::~TcpServer(void)
{
    for (ConnectionMap::iterator it = _connections.begin();
            it != _connections.end(); ++it) {
        TcpConnection *conn = it->second;
        delete conn;
    }
    delete _iocp;
}

void TcpServer::start()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (DWORD i = 0; i < (si.dwNumberOfProcessors * 2 + 2); ++i) {
        HANDLE handle;
        handle = CreateThread(NULL, 0, WorkerThreadProc, _iocp, 0, NULL);
        CloseHandle(handle);
    }

    _acceptEvent = WSACreateEvent();
    WSAEventSelect(*_listenSocket, _acceptEvent, FD_ACCEPT);
    _listenSocket->listen(_listenAddr);
    HANDLE handle = CreateThread(NULL, 0, ListenerThreadProc, this, 0, NULL);
    CloseHandle(handle);
}

void TcpServer::newConnection(SOCKET s, InetAddress peerAddr)
{
    char buf[32] = {0};
    _snprintf(buf, sizeof(buf), ":%s#%d", _listenAddr.toIpPort().c_str(), _nextConnId);
    ++_nextConnId;
    String connName = _name + buf;
    InetAddress localAddr(_listenSocket->getLocalAddr());
    TcpConnection *conn = new TcpConnection(s, connName, localAddr, peerAddr);

    _connections[connName] = conn;
    _iocp->associateSocket(s, (ULONG_PTR)conn);

    conn->setConnectionCallback(_connectionCallback);
    conn->setCloseCallback(_closeCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);

    conn->connectEstablished();
}

DWORD TcpServer::WorkerThreadProc(LPVOID param)
{
    Iocp *iocp = reinterpret_cast<Iocp *>(param);
    EventLoop loop(iocp);
    loop.loop();
    return 0;
}

DWORD TcpServer::ListenerThreadProc(LPVOID param)
{
    TcpServer *server = reinterpret_cast<TcpServer *>(param);
    while (server->started()) {
        DWORD dwRet = WSAWaitForMultipleEvents(1,&server->_acceptEvent, FALSE, 100, FALSE);
        
        if (WSA_WAIT_TIMEOUT == dwRet)
            continue;

        WSANETWORKEVENTS events;
        int nRet = WSAEnumNetworkEvents(*server->_listenSocket, server->_acceptEvent, &events);
        if (SOCKET_ERROR == nRet) {
            // error
        }
        if (events.lNetworkEvents & FD_ACCEPT) {
            if (0 == events.iErrorCode[FD_ACCEPT_BIT]) {
                SOCKET clientSocket = INVALID_SOCKET;
                struct sockaddr_in addr;
                int len = sizeof(addr);
                clientSocket = WSAAccept(*server->_listenSocket, (sockaddr *)&addr, &len, 0, 0);
                if (SOCKET_ERROR == clientSocket) {
                    DWORD errCode = WSAGetLastError();

                } else {
                    server->newConnection(clientSocket, InetAddress(addr));
                }
            } else {
                // error
            }
        } 
    }

    return 0;
}