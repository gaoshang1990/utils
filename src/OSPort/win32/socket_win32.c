#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#include "hal_socket.h"

#define DEBUG_SOCKET                  1
#define CONFIG_TCP_KEEPALIVE_IDLE     5
#define CONFIG_TCP_KEEPALIVE_INTERVAL 2

#ifndef __MINGW64_VERSION_MAJOR
struct tcp_keepalive {
    u_long onoff;
    u_long keepalivetime;
    u_long keepaliveinterval;
};
#endif

#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR, 4)

struct sSocket {
    SOCKET   fd;
    uint32_t connectTimeout;
};

struct sServerSocket {
    SOCKET fd;
    int    backLog;
};

struct sHandleSet {
    fd_set handles;
    SOCKET maxHandle;
};

struct sSocketAddr {
    struct sockaddr_in addr;
    socklen_t          len;
};


HandleSet Handleset_new(void)
{
    HandleSet result = (HandleSet)malloc(sizeof(struct sHandleSet));

    if (result != NULL) {
        FD_ZERO(&result->handles);
        result->maxHandle = INVALID_SOCKET;
    }
    return result;
}


void Handleset_reset(HandleSet self)
{
    FD_ZERO(&self->handles);
    self->maxHandle = INVALID_SOCKET;
}


void Handleset_addSocket(HandleSet self, const Socket sock)
{
    if (self != NULL && sock != NULL && sock->fd != INVALID_SOCKET) {
        FD_SET(sock->fd, &self->handles);

        if ((sock->fd > self->maxHandle) || (self->maxHandle == INVALID_SOCKET))
            self->maxHandle = sock->fd;
    }
}


int Handleset_waitReady(HandleSet self, unsigned int timeoutMs)
{
    int result;

    if (self != NULL && self->maxHandle >= 0) {
        struct timeval timeout;

        timeout.tv_sec  = timeoutMs / 1000;
        timeout.tv_usec = (timeoutMs % 1000) * 1000;
        result          = select(self->maxHandle + 1, &self->handles, NULL, NULL, &timeout);
    }
    else {
        result = -1;
    }

    return result;
}


void Handleset_destroy(HandleSet self)
{
    free(self);
}


static void _activateKeepAlive(SOCKET s)
{
    struct tcp_keepalive keepalive;
    DWORD                retVal = 0;

    keepalive.onoff             = 1;
    keepalive.keepalivetime     = CONFIG_TCP_KEEPALIVE_IDLE * 1000;
    keepalive.keepaliveinterval = CONFIG_TCP_KEEPALIVE_INTERVAL * 1000;

    if (WSAIoctl(s, SIO_KEEPALIVE_VALS, &keepalive, sizeof(keepalive), NULL, 0, &retVal, NULL, NULL) == SOCKET_ERROR) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: WSAIotcl(SIO_KEEPALIVE_VALS) failed: %d\n", WSAGetLastError());
    }
}


static void _setSocketNonBlocking(Socket self)
{
    unsigned long mode = 1;
    if (ioctlsocket(self->fd, FIONBIO, &mode) != 0) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: failed to set socket non-blocking!\n");
    }

    /* activate TCP_NODELAY */

    int tcpNoDelay = 1;
    setsockopt(self->fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&tcpNoDelay, sizeof(int));
}


static void _setUdpSocketNonBlocking(Socket self)
{
    unsigned long mode = 1;
    if (ioctlsocket(self->fd, FIONBIO, &mode) != 0) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: failed to set socket non-blocking!\n");
    }
}


/**
 * xu add 2022-09-01
 * 设置close后立即关闭连接
 */
int setSocketLinger(Socket self, uint16_t onoff, uint16_t linger)
{
    struct linger opt;
    opt.l_onoff  = onoff;
    opt.l_linger = linger;

    return setsockopt(self->fd, SOL_SOCKET, SO_LINGER, (const char*)&opt, sizeof(opt));
}


static bool _prepareServerAddress(const char* address, int port, struct sockaddr_in* sockaddr)
{
    memset((char*)sockaddr, 0, sizeof(struct sockaddr_in));

    if (address != NULL) {
        struct hostent* server;
        server = gethostbyname(address);

        if (server == NULL)
            return false;

        memcpy((char*)&sockaddr->sin_addr.s_addr, (char*)server->h_addr, server->h_length);
    }
    else
        sockaddr->sin_addr.s_addr = INADDR_ANY;

    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port   = htons(port);

    return true;
}


ServerSocket TcpServerSocket_create(const char* address, int port)
{
    ServerSocket serverSocket  = NULL;
    SOCKET       listen_socket = INVALID_SOCKET;

    int     ec;
    WSADATA wsa;
    if ((ec = WSAStartup(MAKEWORD(2, 0), &wsa)) != 0) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: winsock error: code %i\n", ec);
        return NULL;
    }

    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
    _activateKeepAlive(listen_socket);
#endif

    if (listen_socket == INVALID_SOCKET) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: socket failed with error: %i\n", WSAGetLastError());

        WSACleanup();
        return NULL;
    }

    int optionReuseAddr = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optionReuseAddr, sizeof(int));


    struct sockaddr_in server_addr;
    if (!_prepareServerAddress(address, port, &server_addr))
        return NULL;
    ec = bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (ec == SOCKET_ERROR) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: bind failed with error:%i\n", WSAGetLastError());

        closesocket(listen_socket);
        WSACleanup();
        return NULL;
    }

    serverSocket = (ServerSocket)malloc(sizeof(struct sServerSocket));

    serverSocket->fd      = listen_socket;
    serverSocket->backLog = 10;

    _setSocketNonBlocking((Socket)serverSocket);

    return serverSocket;
}


Socket UdpServerSocket_create(const char* address, int port)
{
    int     ec;
    WSADATA wsa;
    if ((ec = WSAStartup(MAKEWORD(2, 0), &wsa)) != 0) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: winsock error: code %i\n", ec);
        return NULL;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listen_socket == INVALID_SOCKET) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: socket failed with error: %i\n", WSAGetLastError());
        WSACleanup();
        return NULL;
    }

    struct sockaddr_in server_addr;
    if (!_prepareServerAddress(address, port, &server_addr))
        return NULL;

    if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: bind failed with error: %i\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return NULL;
    }

    Socket serverSocket          = (Socket)malloc(sizeof(struct sSocket));
    serverSocket->fd             = listen_socket;
    serverSocket->connectTimeout = 0;

    _setUdpSocketNonBlocking(serverSocket);

    return serverSocket;
}


void ServerSocket_listen(ServerSocket self)
{
    listen(self->fd, self->backLog);
}


/* xu 2022-09-01 增加accept函数的参数 */
Socket ServerSocket_accept(ServerSocket self, int* addr)
{
    int    fd        = -1;
    Socket conSocket = NULL;

    struct sockaddr_in clientAddr;
    socklen_t          clientAddrLen = sizeof(clientAddr);

    fd = accept(self->fd, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (fd >= 0) {
        conSocket     = TcpSocket_create();
        conSocket->fd = fd;
        if (addr)
            *addr = clientAddr.sin_addr.s_addr;

        _setSocketNonBlocking(conSocket);
    }

    return conSocket;
}


void ServerSocket_setBacklog(ServerSocket self, int backlog)
{
    self->backLog = backlog;
}


void ServerSocket_destroy(ServerSocket self)
{
    closesocket(self->fd);
    WSACleanup();
    free(self);
}


Socket TcpSocket_create()
{
    Socket self = (Socket)malloc(sizeof(struct sSocket));

    self->fd             = INVALID_SOCKET;
    self->connectTimeout = 5000;

    return self;
}


void Socket_setConnectTimeout(Socket self, uint32_t timeoutInMs)
{
    self->connectTimeout = timeoutInMs;
}


bool Socket_connect(Socket self, const char* address, int port)
{
    struct sockaddr_in serverAddress;
    WSADATA            wsa;
    int                ec;

    if ((ec = WSAStartup(MAKEWORD(2, 0), &wsa)) != 0) {
        if (DEBUG_SOCKET)
            printf("WIN32_SOCKET: winsock error: code %i\n", ec);
        return false;
    }

    if (!_prepareServerAddress(address, port, &serverAddress))
        return false;

    self->fd = socket(AF_INET, SOCK_STREAM, 0);

#if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
    _activateKeepAlive(self->fd);
#endif

    _setSocketNonBlocking(self);

    fd_set fdSet;
    FD_ZERO(&fdSet);
    FD_SET(self->fd, &fdSet);

    if (connect(self->fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
            return false;
    }

    struct timeval timeout;
    timeout.tv_sec  = self->connectTimeout / 1000;
    timeout.tv_usec = (self->connectTimeout % 1000) * 1000;

    if (select(self->fd + 1, NULL, &fdSet, NULL, &timeout) <= 0)
        return false;
    else
        return true;
}


char* Socket_getPeerAddress(Socket self)
{
    struct sockaddr_storage addr;
    int                     addrLen = sizeof(addr);

    getpeername(self->fd, (struct sockaddr*)&addr, &addrLen);

    char addrString[INET6_ADDRSTRLEN + 7];
    int  addrStringLen = INET6_ADDRSTRLEN + 7;
    int  port;

    bool isIPv6;

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* ipv4Addr = (struct sockaddr_in*)&addr;
        port                         = ntohs(ipv4Addr->sin_port);
        ipv4Addr->sin_port           = 0;
        WSAAddressToString(
            (LPSOCKADDR)ipv4Addr, sizeof(struct sockaddr_storage), NULL, (LPSTR)addrString, (LPDWORD)&addrStringLen);
        isIPv6 = false;
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)&addr;
        port                          = ntohs(ipv6Addr->sin6_port);
        ipv6Addr->sin6_port           = 0;
        WSAAddressToString(
            (LPSOCKADDR)ipv6Addr, sizeof(struct sockaddr_storage), NULL, (LPSTR)addrString, (LPDWORD)&addrStringLen);
        isIPv6 = true;
    }
    else
        return NULL;

    char* clientConnection = (char*)malloc(strlen(addrString) + 9);

    if (isIPv6)
        sprintf(clientConnection, "[%s]:%i", addrString, port);
    else
        sprintf(clientConnection, "%s:%i", addrString, port);

    return clientConnection;
}


char* Socket_getPeerAddressStatic(Socket self, char* peerAddressString)
{
    struct sockaddr_storage addr;
    int                     addrLen = sizeof(addr);

    getpeername(self->fd, (struct sockaddr*)&addr, &addrLen);

    char addrString[INET6_ADDRSTRLEN + 7];
    int  addrStringLen = INET6_ADDRSTRLEN + 7;
    int  port;

    bool isIPv6;

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* ipv4Addr = (struct sockaddr_in*)&addr;
        port                         = ntohs(ipv4Addr->sin_port);
        ipv4Addr->sin_port           = 0;
        WSAAddressToString(
            (LPSOCKADDR)ipv4Addr, sizeof(struct sockaddr_storage), NULL, (LPSTR)addrString, (LPDWORD)&addrStringLen);
        isIPv6 = false;
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)&addr;
        port                          = ntohs(ipv6Addr->sin6_port);
        ipv6Addr->sin6_port           = 0;
        WSAAddressToString(
            (LPSOCKADDR)ipv6Addr, sizeof(struct sockaddr_storage), NULL, (LPSTR)addrString, (LPDWORD)&addrStringLen);
        isIPv6 = true;
    }
    else
        return NULL;

    if (isIPv6)
        sprintf(peerAddressString, "[%s]:%i", addrString, port);
    else
        sprintf(peerAddressString, "%s:%i", addrString, port);

    return peerAddressString;
}


int Socket_read(Socket self, uint8_t* buf, int size)
{
    int bytes_read = recv(self->fd, (char*)buf, size, 0);

    if (bytes_read == 0) /* peer has closed socket */
        return -1;

    if (bytes_read == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            return 0;
        else
            return -1;
    }

    return bytes_read;
}


int UdpSocket_read(Socket self, uint8_t* buf, int size, SocketAddr_t* from)
{
    *from = (SocketAddr_t)malloc(sizeof(struct sSocketAddr));
    memset(*from, 0, sizeof(struct sSocketAddr));
    (*from)->len = sizeof((*from)->addr);

    int read_bytes = recvfrom(self->fd, (char*)buf, size, 0, (struct sockaddr*)&(*from)->addr, &(*from)->len);
    if (read_bytes == 0) /* peer has closed socket */
        return -1;

    if (read_bytes == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            return 0;
        else
            return -1;
    }

    return read_bytes;
}


int Socket_write(Socket self, uint8_t* buf, int size)
{
    int sent_bytes = send(self->fd, (char*)buf, size, 0);

    if (sent_bytes == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK)
            sent_bytes = 0;
        else
            sent_bytes = -1;
    }

    return sent_bytes;
}


int UdpSocket_write(Socket self, uint8_t* buf, int size, SocketAddr_t to)
{
    int sent_bytes = sendto(self->fd, (char*)buf, size, 0, (struct sockaddr*)&to->addr, to->len);

    if (sent_bytes == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK)
            sent_bytes = 0;
        else
            sent_bytes = -1;
    }

    return sent_bytes;
}


void Socket_destroyAddr(SocketAddr_t addr)
{
    if (addr != NULL) {
        free(addr);
        addr = NULL;
    }
}


void Socket_destroy(Socket self)
{
    if (self->fd != INVALID_SOCKET) {
        closesocket(self->fd);
    }

    free(self);
    self = NULL;
}
