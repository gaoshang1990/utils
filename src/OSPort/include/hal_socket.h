#ifndef SOCKET_HAL_H_
#define SOCKET_HAL_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \defgroup hal Platform (Hardware/OS) abstraction layer
 *
 *  @{
 */

/**
 * @defgroup HAL_SOCKET Interface to the TCP/IP stack (abstract socket layer)
 *
 * @{
 */

/** Opaque reference for a server socket instance */
typedef struct sServerSocket* ServerSocket;

/** Opaque reference for a client or connection socket instance */
typedef struct sSocket* Socket;

/** Opaque reference for a set of server and socket handles */
typedef struct sHandleSet* HandleSet;

typedef struct sSocketAddr* SocketAddr_t; /* xu add 2022-09-01 */

/**
 * \brief Create a new connection handle set (HandleSet)
 * \return new HandleSet instance
 */
HandleSet Handleset_new(void);

/**
 * \brief Reset the handle set for reuse
 */
void Handleset_reset(HandleSet self);

/**
 * \brief add a soecket to an existing handle set
 *
 * \param self the HandleSet instance
 * \param sock the socket to add
 */
void Handleset_addSocket(HandleSet self, const Socket sock);


/**
 * \brief wait for a socket to become ready
 *
 * This function is corresponding to the BSD socket select function.
 * It returns the number of sockets on which data is pending or 0 if no data is pending
 * on any of the monitored connections. The function will return after "timeout" ms if no
 * data is pending.
 * The function shall return -1 if a socket error occures.
 *
 *  \param self the HandleSet instance
 *  \param timeout in milliseconds (ms)
 */
int Handleset_waitReady(HandleSet self, unsigned int timeoutMs);

/**
 * \brief destroy the HandleSet instance
 *
 * \param self the HandleSet instance to destroy
 */
void Handleset_destroy(HandleSet self);

/**
 * \brief Create a new TcpServerSocket instance
 *
 * Implementation of this function is MANDATORY if server functionality is required.
 *
 * \param address ip address or hostname to listen on
 * \param port the TCP port to listen on
 *
 * \return the newly create TcpServerSocket instance
 */
ServerSocket TcpServerSocket_create(const char* address, int port);


void ServerSocket_listen(ServerSocket self);

/**
 * \brief accept a new incoming connection (non-blocking)
 *
 * This function shall accept a new incoming connection. It is non-blocking and has to
 * return NULL if no new connection is pending.
 *
 * Implementation of this function is MANDATORY if server functionality is required.
 *
 * NOTE: The behaviour of this function changed with version 0.8!
 *
 * \param self server socket instance
 *
 * \return handle of the new connection socket or NULL if no new connection is available
 */
Socket ServerSocket_accept(ServerSocket self, int* addr);


/**
 * \brief set the maximum number of pending connection in the queue
 *
 * Implementation of this function is OPTIONAL.
 *
 * \param self the server socket instance
 * \param backlog the number of pending connections in the queue
 *
 */
void ServerSocket_setBacklog(ServerSocket self, int backlog);

/**
 * \brief destroy a server socket instance
 *
 * Free all resources allocated by this server socket instance.
 *
 * Implementation of this function is MANDATORY if server functionality is required.
 *
 * \param self server socket instance
 */
void ServerSocket_destroy(ServerSocket self);

/**
 * \brief create a TCP client socket
 *
 * Implementation of this function is MANDATORY if client functionality is required.
 *
 * \return a new client socket instance.
 */
Socket TcpSocket_create(void);

/**
 * \brief set the timeout to establish a new connection
 *
 * \param self the client socket instance
 * \param timeoutInMs the timeout in ms
 */
void Socket_setConnectTimeout(Socket self, uint32_t timeoutInMs);

/**
 * \brief connect to a server
 *
 * Connect to a server application identified by the address and port parameter.
 *
 * The "address" parameter may either be a hostname or an IP address. The IP address
 * has to be provided as a C string (e.g. "10.0.2.1").
 *
 * \param self the client socket instance
 * \param address the IP address or hostname as C string
 * \param port the TCP port of the application to connect to
 *
 * \return true if the connection was established successfully, false otherwise
 */
bool Socket_connect(Socket self, const char* address, int port);

/**
 * \brief read from socket to local buffer (non-blocking)
 *
 * The function shall return immediately if no data is available. In this case
 * the function returns 0. If an error happens the function shall return -1.
 * \param self the client, connection or server socket instance
 * \param buf the buffer where the read bytes are copied to
 * \param size the maximum number of bytes to read (size of the provided buffer)
 *
 * \return the number of bytes read or -1 if an error occurred
 */
int Socket_read(Socket self, uint8_t* buf, int size);

/**
 * \brief send a message through the socket
 *
 * Implementation of this function is MANDATORY
 *
 * \param self client, connection or server socket instance
 *
 * \return number of bytes transmitted of -1 in case of an error
 */
int Socket_write(Socket self, uint8_t* buf, int size);

/**
 * \brief Get the address of the peer application (IP address and port number)
 *
 * \param self the client, connection or server socket instance
 *
 * \return the IP address and port number as strings separated by the ':' character.
 */
char* Socket_getPeerAddress(Socket self);

/**
 * \brief Get the address of the peer application (IP address and port number)
 *
 * The peer address has to be returned as
 *
 * Implementation of this function is MANDATORY
 *
 * \param self the client, connection or server socket instance
 * \param peerAddressString a string to store the peer address (the string should have space
 *        for at least 60 characters)
 *
 * \return the IP address and port number as strings separated by the ':' character. If the
 *         address is an IPv6 address the IP part is encapsulated in square brackets.
 */
char* Socket_getPeerAddressStatic(Socket self, char* peerAddressString);

/**
 * \brief destroy a socket (close the socket if a connection is established)
 *
 * This function shall close the connection (if one is established) and free all
 * resources allocated by the socket.
 *
 * Implementation of this function is MANDATORY
 *
 * \param self the client, connection or server socket instance
 */
void Socket_destroy(Socket self);


int setSocketLinger(Socket self, uint16_t onoff, uint16_t linger);

void Socket_destroyAddr(SocketAddr_t addr);

Socket UdpServerSocket_create(const char* address, int port);
int    UdpSocket_read(Socket self, uint8_t* buf, int size, SocketAddr_t* from);
int    UdpSocket_write(Socket self, uint8_t* buf, int size, SocketAddr_t to);


#ifdef __cplusplus
}
#endif

#endif /* SOCKET_HAL_H_ */
