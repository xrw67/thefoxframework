#include <net/TcpServer.h>
#include <base/Types.h>
#include <net/Iocp.h>
#include <net/Socket.h>
#include <net/EventLoop.h>
#include <net/TcpConnection.h>

using namespace thefox;
using namespace thefox::net;

TcpServer::TcpServer(const InetAddress &listenAddr,
                     const String &nameArg)
    : _hostport(listenAddr.toIpPort())
    , _name(nameArg)
    , _nextConnId(1)
    , _iocp(new Iocp())
    , _listenSocket(new Socket(Socket::Create()))
{
    _iocp->bindHandle(*_listenSocket);
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

void start()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (si.dwNumberOfProcessors * 2 + 2); ++i) {
        HANDLE handle;
        handle = CreateThread(NULL, 0, WorkerThreadProc, _iocp, 0, NULL);
        CloseHandle(handle);
    }

    _listenSocket.listen();
    HANDLE handle = CreateThread(NULL, 0, ListenerThreadProc, this, 0, NULL);
    CloseHandle(handle);
}

TcpServer::newConnection(SOCKET s, const InetAddress &peerAddr)
{
    char buf[32] = {0};
    _snprintf(buf, sizeof(buf), ":%s#%d", _hostport.c_str(), _nextConnId);
    ++_nextConnId;
    string connName = _name + buf;
    InetAddress localAddr(Socket::getLocalAddr(s));
    TcpConnection *conn = new TcpConnection(s, connName, localAddr, peerAddr);

    _connections[connName] = conn;
    _iocp.associateSocket(s, conn);

    conn->setConnectionCallback(_connectionCallback);
    conn->setCloseCallback(_closeCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompletionCallback(_writeCompletionCallback);

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
    TcpServer *server = reinterpret_cast<TcpServe *>(param);
    while (true) {
        DWORD dwRet = WSAWaitForMultipleEvent(1,&server->_acceptEvent, FALSE, 100, FALSE);
        
        if (WSA_WAIT_TIMEOUT == dwRet)
            continue;

        WSANETWORKEVENTS events;
        int nRet = WSAEnumNetworkEvents(server->_listenSocket, server->_acceptEvent, &events);
        if (SOCKET_ERROR == nRet) {
            // error
        }
        if (events.lNetworkEvents & FD_ACCEPT) {
            if (0 == events.iErrorCode[FD_ACCEPT_BIT]) {
                SOCKET clientSocket = INVALID_SOCKET;
                int len = -1;
                clientSocket = WSAAccept(server->_listenSocket, NULL, &len, 0, 0);
                if (SOCKET_ERROR == clientSocket) {
                    // error
                } else {
                    server->newConnection(accept, peer);
                }
            } else {
                // error
            }
        } 
    }

    return 0;
}