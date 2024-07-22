
#include "utils_log.h"
#include "hal_socket.h"


static int udp_test()
{
    slog_info("--- udp test start ---\n");

    Socket udpSock = UdpServerSocket_create(NULL, 9001);

    SocketAddr_t addr = NULL;

    uint8_t recv[4096] = {0};

    for (;;) {
        int recvLen = UdpSocket_read(udpSock, recv, sizeof(recv), &addr);
        if (recvLen > 0) {
            printf("%s\n", recv);
            UdpSocket_write(udpSock, (uint8_t*)"hahaha", 7, addr);
        }

        delayMs_(10);
    }

    slog_info("--- udp test done ---\n");

    return 0;
}


int socket_test()
{
    slog_info("-- socket test start --\n");

    // udp_test();

    slog_info("-- socket test done --\n");

    return 0;
}
