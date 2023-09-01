#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "hal_socket.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/tcp.h> // required for TCP keepalive

#include "hal_thread.h"

#ifndef DEBUG_SOCKET
#  define DEBUG_SOCKET 0
#endif

struct sSocket {
    int      fd;
    uint32_t connectTimeout;
};

struct sServerSocket {
    int fd;
    int backLog;
};

struct sHandleSet {
    fd_set handles;
    int    maxHandle;
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
        result->maxHandle = -1;
    }
    return result;
}

void Handleset_reset(HandleSet self)
{
    FD_ZERO(&self->handles);
    self->maxHandle = -1;
}

void Handleset_addSocket(HandleSet self, const Socket sock)
{
    if (self != NULL && sock != NULL && sock->fd != -1) {
        FD_SET(sock->fd, &self->handles);
        if (sock->fd > self->maxHandle) {
            self->maxHandle = sock->fd;
        }
    }
}

int Handleset_waitReady(HandleSet self, unsigned int timeoutMs)
{
    int result;

    if ((self != NULL) && (self->maxHandle >= 0)) {
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

static bool _prepareServerAddress(const char* address, int port, struct sockaddr_in* sockaddr)
{
    bool retVal = true;

    memset((char*)sockaddr, 0, sizeof(struct sockaddr_in));

    if (address != NULL) {
        struct addrinfo  addressHints;
        struct addrinfo* lookupResult;
        int              result;

        memset(&addressHints, 0, sizeof(struct addrinfo));
        addressHints.ai_family = AF_INET;
        result                 = getaddrinfo(address, NULL, &addressHints, &lookupResult);

        if (result != 0) {
            retVal = false;
            goto exit_function;
        }

        memcpy(sockaddr, lookupResult->ai_addr, sizeof(struct sockaddr_in));
        freeaddrinfo(lookupResult);
    }
    else
        sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);

    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port   = htons(port);

exit_function:
    return retVal;
}

static void _setSocketNonBlocking(Socket self)
{
    int flags = fcntl(self->fd, F_GETFL, 0);
    fcntl(self->fd, F_SETFL, flags | O_NONBLOCK);
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

static void activateTcpNoDelay(Socket self)
{
    /* activate TCP_NODELAY option - packets will be sent immediately */
    int flag = 1;
    setsockopt(self->fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
}


ServerSocket TcpServerSocket_create(const char* address, int port)
{
    ServerSocket serverSocket = NULL;

    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
        struct sockaddr_in serverAddress;

        if (!_prepareServerAddress(address, port, &serverAddress)) {
            close(fd);
            return NULL;
        }

        int optionReuseAddr = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&optionReuseAddr, sizeof(int));

        if (bind(fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) >= 0) {
            serverSocket          = (ServerSocket)malloc(sizeof(struct sServerSocket));
            serverSocket->fd      = fd;
            serverSocket->backLog = 0;

            _setSocketNonBlocking((Socket)serverSocket);
        }
        else {
            close(fd);
            return NULL;
        }
    }

    return serverSocket;
}


Socket UdpServerSocket_create(const char* address, int port)
{
    Socket serverSocket = NULL;

    int fd;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
        struct sockaddr_in serverAddress;
        if (!_prepareServerAddress(address, port, &serverAddress)) {
            close(fd);
            return NULL;
        }

        // int optionReuseAddr = 1;
        // setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&optionReuseAddr, sizeof(int));

        if (bind(fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) >= 0) {
            serverSocket                 = (Socket)malloc(sizeof(struct sSocket));
            serverSocket->fd             = fd;
            serverSocket->connectTimeout = 0;
            // _setSocketNonBlocking((Socket)serverSocket);
        }
        else {
            close(fd);
            return NULL;
        }
    }

    return serverSocket;
}


void ServerSocket_listen(ServerSocket self)
{
    listen(self->fd, self->backLog);
}


/* CHANGED TO MAKE NON-BLOCKING --> RETURNS NULL IF NO CONNECTION IS PENDING */
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

        activateTcpNoDelay(conSocket);
    }

    return conSocket;
}

void ServerSocket_setBacklog(ServerSocket self, int backlog)
{
    self->backLog = backlog;
}

static void closeAndShutdownSocket(int socketFd)
{
    if (socketFd != -1) {
        if (DEBUG_SOCKET)
            printf("socket_linux.c: call shutdown for %i!\n", socketFd);

        // shutdown is required to unblock read or accept in another thread!
        shutdown(socketFd, SHUT_RDWR);

        close(socketFd);
    }
}

void ServerSocket_destroy(ServerSocket self)
{
    int fd = self->fd;

    self->fd = -1;

    closeAndShutdownSocket(fd);

    Thread_sleep(10);

    free(self);
}

Socket TcpSocket_create()
{
    Socket self = (Socket)malloc(sizeof(struct sSocket));

    self->fd             = -1;
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

    if (DEBUG_SOCKET)
        printf("Socket_connect: %s:%i\n", address, port);

    if (!_prepareServerAddress(address, port, &serverAddress))
        return false;

    self->fd = socket(AF_INET, SOCK_STREAM, 0);

    fd_set fdSet;
    FD_ZERO(&fdSet);
    FD_SET(self->fd, &fdSet);

    activateTcpNoDelay(self);

#if (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1)
    _activateKeepAlive(self->fd);
#endif

    fcntl(self->fd, F_SETFL, O_NONBLOCK);

    if (connect(self->fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        if (errno != EINPROGRESS)
            return false;
    }

    struct timeval timeout;
    timeout.tv_sec  = self->connectTimeout / 1000;
    timeout.tv_usec = (self->connectTimeout % 1000) * 1000;

    if (select(self->fd + 1, NULL, &fdSet, NULL, &timeout) == 1) {
        int       so_error;
        socklen_t len = sizeof so_error;

        getsockopt(self->fd, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0)
            return true;
    }

    close(self->fd);

    return false;
}

char* Socket_getPeerAddress(Socket self)
{
    struct sockaddr_storage addr;
    socklen_t               addrLen = sizeof(addr);

    getpeername(self->fd, (struct sockaddr*)&addr, &addrLen);

    char addrString[INET6_ADDRSTRLEN + 7];
    int  port;

    bool isIPv6;

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* ipv4Addr = (struct sockaddr_in*)&addr;
        port                         = ntohs(ipv4Addr->sin_port);
        inet_ntop(AF_INET, &(ipv4Addr->sin_addr), addrString, INET_ADDRSTRLEN);
        isIPv6 = false;
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)&addr;
        port                          = ntohs(ipv6Addr->sin6_port);
        inet_ntop(AF_INET6, &(ipv6Addr->sin6_addr), addrString, INET6_ADDRSTRLEN);
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
    socklen_t               addrLen = sizeof(addr);

    getpeername(self->fd, (struct sockaddr*)&addr, &addrLen);

    char addrString[INET6_ADDRSTRLEN + 7];
    int  port;

    bool isIPv6;

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* ipv4Addr = (struct sockaddr_in*)&addr;
        port                         = ntohs(ipv4Addr->sin_port);
        inet_ntop(AF_INET, &(ipv4Addr->sin_addr), addrString, INET_ADDRSTRLEN);
        isIPv6 = false;
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)&addr;
        port                          = ntohs(ipv6Addr->sin6_port);
        inet_ntop(AF_INET6, &(ipv6Addr->sin6_addr), addrString, INET6_ADDRSTRLEN);
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
    if (self->fd == -1)
        return -1;

    int read_bytes = recv(self->fd, buf, size, MSG_DONTWAIT);

    if (read_bytes == 0)
        return -1;

    if (read_bytes == -1) {
        int error = errno;

        switch (error) {
        case EAGAIN:
            return 0;
        case EBADF:
            return -1;

        default:
            return -1;
        }
    }

    return read_bytes;
}


int UdpSocket_read(Socket self, uint8_t* buf, int size, SocketAddr_t* from)
{
    if (self->fd == -1)
        return -1;

    (*from) = (SocketAddr_t)malloc(sizeof(struct sSocketAddr));
    memset((*from), 0, sizeof(struct sSocketAddr));
    (*from)->len = sizeof((*from)->addr);

    int read_bytes = recvfrom(self->fd, (char*)buf, size, 0, (struct sockaddr*)&(*from)->addr, &(*from)->len);

    if (read_bytes == 0)
        return -1;

    if (read_bytes == -1) {
        int error = errno;

        switch (error) {
        case EAGAIN:
            return 0;
        case EBADF:
            return -1;

        default:
            return -1;
        }
    }

    return read_bytes;
}


int Socket_write(Socket self, uint8_t* buf, int size)
{
    if (self->fd == -1)
        return -1;

    // MSG_NOSIGNAL - prevent send to signal SIGPIPE when peer unexpectedly closed the socket
    return send(self->fd, buf, size, MSG_NOSIGNAL);
}


int UdpSocket_write(Socket self, uint8_t* buf, int size, SocketAddr_t to)
{
    if (self->fd == -1)
        return -1;

    return sendto(self->fd, buf, size, 0, (struct sockaddr*)&to->addr, to->len);
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
    int fd = self->fd;

    self->fd = -1;

    closeAndShutdownSocket(fd);

    Thread_sleep(10);

    free(self);
    self = NULL;
}
