#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__linux__)
#  include <termios.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <sys/select.h>
#  include <sys/time.h>
#endif

#include "os_serial.h"


#if defined(_WIN32)
#  define _close(fd) CloseHandle(fd)
#elif defined(__linux__)
#  define _close(fd)           close(fd)
#  define HANDLE               int
#  define INVALID_HANDLE_VALUE (-1)
#endif


struct sSerialPort {
    HANDLE fd;
    char   dev_name[32];

    int     baud_rate;
    uint8_t data_bits;
    char    parity;
    uint8_t stop_bits;

    uint64_t sys_time; /* ms */
    int      timeout;  /* ms */
};


SerialPort serial_new(const char* dev_name,
                      int         baud_rate,
                      char        parity,
                      uint8_t     data_bits,
                      uint8_t     stop_bits)
{
    SerialPort self = (SerialPort)malloc(sizeof(struct sSerialPort));

    if (self != NULL) {
        self->fd = INVALID_HANDLE_VALUE;

        strncpy(self->dev_name, dev_name, sizeof(self->dev_name) - 1);

        self->baud_rate = baud_rate;
        self->data_bits = data_bits;
        self->parity    = parity;
        self->stop_bits = stop_bits;

        self->sys_time = 0;
        self->timeout  = 2000; /* ms */
    }

    return self;
}


void serial_del(SerialPort self)
{
    if (self != NULL) {
        serial_close(self);

        free(self);
        self = NULL;
    }
}


int serial_open(SerialPort self)
{
#if defined(_WIN32)
    self->fd = CreateFile(
        self->dev_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (self->fd == INVALID_HANDLE_VALUE)
        return -1;

    DCB serialParams       = {0};
    serialParams.DCBlength = sizeof(DCB);
    if (false == GetCommState(self->fd, &serialParams)) {
        serial_close(self);
        return -1;
    }

    /* set baud rate */
    switch (self->baud_rate) {
    case 110:
        serialParams.BaudRate = CBR_110;
        break;
    case 300:
        serialParams.BaudRate = CBR_300;
        break;
    case 600:
        serialParams.BaudRate = CBR_600;
        break;
    case 1200:
        serialParams.BaudRate = CBR_1200;
        break;
    case 2400:
        serialParams.BaudRate = CBR_2400;
        break;
    case 4800:
        serialParams.BaudRate = CBR_4800;
        break;
    case 9600:
        serialParams.BaudRate = CBR_9600;
        break;
    case 19200:
        serialParams.BaudRate = CBR_19200;
        break;
    case 38400:
        serialParams.BaudRate = CBR_38400;
        break;
    case 57600:
        serialParams.BaudRate = CBR_57600;
        break;
    case 115200:
        serialParams.BaudRate = CBR_115200;
        break;
    default:
        serialParams.BaudRate = CBR_9600;
    }

    /* Set data bits (5/6/7/8) */
    serialParams.ByteSize = self->data_bits;

    /* Set stop bits (1/2) */
    if (self->stop_bits == 1)
        serialParams.StopBits = ONESTOPBIT;
    else /* 2 */
        serialParams.StopBits = TWOSTOPBITS;

    if (self->parity == 'N')
        serialParams.Parity = NOPARITY;
    else if (self->parity == 'E')
        serialParams.Parity = EVENPARITY;
    else /* 'O' */
        serialParams.Parity = ODDPARITY;

    if (false == SetCommState(self->fd, &serialParams)) {
        serial_close(self);
        return -1;
    }

    COMMTIMEOUTS timeouts                = {0};
    timeouts.ReadIntervalTimeout         = 100;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 100;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (false == SetCommTimeouts(self->fd, &timeouts)) {
        serial_close(self);
        return -1;
    }

    if (false == SetCommMask(self->fd, EV_RXCHAR)) {
        printf("SetCommMask failed!\n");
        serial_close(self);
        return -1;
    }
#elif defined(__linux__)
    self->fd = open(self->dev_name, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (self->fd == INVALID_HANDLE_VALUE) {
        serial_close(self);
        return -1;
    }

    struct termios tios;
    speed_t        baudrate;
    tcgetattr(self->fd, &tios);

    switch (self->baud_rate) {
    case 300:
        baudrate = B300;
        break;
    case 600:
        baudrate = B600;
        break;
    case 1200:
        baudrate = B1200;
        break;
    case 2400:
        baudrate = B2400;
        break;
    case 4800:
        baudrate = B4800;
        break;
    case 9600:
        baudrate = B9600;
        break;
    case 19200:
        baudrate = B19200;
        break;
    case 38400:
        baudrate = B38400;
        break;
    case 57600:
        baudrate = B57600;
        break;
    case 115200:
        baudrate = B115200;
        break;
    default:
        baudrate = B9600;
    }

    /* Set baud rate */
    if ((cfsetispeed(&tios, baudrate) < 0) || (cfsetospeed(&tios, baudrate) < 0)) {
        serial_close(self);
        return -1;
    }

    tios.c_cflag |= (CREAD | CLOCAL);

    /* Set data bits (5/6/7/8) */
    tios.c_cflag &= ~CSIZE;
    switch (self->data_bits) {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    /* Set stop bits (1/2) */
    if (self->stop_bits == 1)
        tios.c_cflag &= ~CSTOPB;
    else /* 2 */
        tios.c_cflag |= CSTOPB;

    if (self->parity == 'N') {
        tios.c_cflag &= ~PARENB;
    }
    else if (self->parity == 'E') {
        tios.c_cflag |= PARENB;
        tios.c_cflag &= ~PARODD;
    }
    else { /* 'O' */
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
    }

    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    if (self->parity == 'N') {
        tios.c_iflag &= ~INPCK;
    }
    else {
        tios.c_iflag |= INPCK;
    }

    tios.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
    tios.c_iflag |= IGNBRK; /* Set ignore break to allow 0xff characters */
    tios.c_iflag |= IGNPAR;
    tios.c_oflag &= ~OPOST;

    tios.c_cc[VMIN]  = 0;
    tios.c_cc[VTIME] = 0;


    tcflush(self->fd, TCIOFLUSH);
    if (tcsetattr(self->fd, TCSANOW, &tios) < 0) {
        serial_close(self);
        return -1;
    }
#endif

    return 0;
}


void serial_close(SerialPort self)
{
    if (self->fd != INVALID_HANDLE_VALUE) {
        _close(self->fd);
        self->fd = INVALID_HANDLE_VALUE;
    }
}


/* since UTC1970-01-01 00:00:00 */
uint64_t _time_ms(void)
{
    uint64_t ret = 0;

#ifdef _WIN32
    FILETIME              ft;
    uint64_t              now;
    static const uint64_t DIFF_TO_UNIXTIME = 11644473600000LL;

    GetSystemTimeAsFileTime(&ft);
    now = (uint64_t)ft.dwLowDateTime + ((uint64_t)(ft.dwHighDateTime) << 32LL);
    ret = (now / 10000LL) - DIFF_TO_UNIXTIME;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    ret = ((uint64_t)now.tv_sec * 1000LL) + (now.tv_usec / 1000);
#endif

    return ret;
}


/* since system start */
static uint64_t _cpu_ms(void)
{
    uint64_t now_ms = 0;

#if defined(_WIN32)
    long now_tick = clock();
    if (now_tick < 0)
        now_ms = _time_ms();
    else
        now_ms = (uint64_t)((now_tick * 1000) / CLOCKS_PER_SEC);
#elif defined(__linux__)
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    now_ms = ((uint64_t)tp.tv_sec) * 1000LL + (tp.tv_nsec / 1000000);
#endif

    return now_ms;
}


static void serial_reset_timeout(SerialPort self, int timeout)
{
    self->timeout  = timeout;
    self->sys_time = _cpu_ms();
}


static bool serial_check_timeout(SerialPort self)
{
    if (_cpu_ms() - self->sys_time > self->timeout)
        return true;

    return false;
}


int serial_read(SerialPort self, uint8_t* buf, int max_bytes)
{
    int read_len = 0;

#if defined(_WIN32)
    if (false == ReadFile(self->fd, buf, max_bytes, &read_len, NULL))
        read_len = -1;
#elif defined(__linux__)
    read_len = read(self->fd, (void*)buf, max_bytes);
#endif

    return read_len;
}


int serial_read_byte(SerialPort self)
{
#if defined(__linux__)
    struct timeval timeout;
    timeout.tv_sec  = self->timeout / 1000;
    timeout.tv_usec = (self->timeout % 1000) * 1000;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(self->fd, &set);
    if (select(self->fd + 1, &set, NULL, NULL, &timeout) <= 0)
        return -1;
#endif

    uint8_t buf[1] = {0};

    if (1 == serial_read(self, buf, 1))
        return (int)buf[0];

    return -1;
}

/**
 * @brief Read byte until timeout
 * @pa
 */
size_t serial_read_byte_until_timeout(SerialPort self,
                                      uint8_t*   dst,
                                      size_t     dst_size,
                                      int        resp_timeout,
                                      int        byte_timeout)
{
    int    read_byte = -1;
    bool   responsed = false;
    size_t recv_len  = 0;

    serial_reset_timeout(self, resp_timeout);
    while (!serial_check_timeout(self)) {
        read_byte = serial_read_byte(self);
        if (read_byte != -1) {
            dst[recv_len++] = (uint8_t)read_byte;
            responsed       = true;
            break;
        }
    }
    if (responsed == false)
        return 0;

    serial_reset_timeout(self, byte_timeout);
    while (!serial_check_timeout(self)) {
        read_byte = serial_read_byte(self);
        if (read_byte >= 0) {
            if (recv_len < dst_size)
                dst[recv_len++] = (uint8_t)read_byte;
            serial_reset_timeout(self, byte_timeout);
        }
    }

    return recv_len;
}


static int serial_flush(SerialPort self)
{
#if defined(_WIN32)
    if (false == FlushFileBuffers(self->fd))
        return -1;
    return 0;
#elif defined(__linux__)
    return tcdrain(self->fd);
#endif
}


int serial_write(SerialPort self, uint8_t* buf, int buf_len)
{
#if defined(_WIN32)
    DWORD written_num;
    if (false == WriteFile(self->fd, buf, buf_len, &written_num, NULL))
        return -1;
#elif defined(__linux__)
    ssize_t written_num = write(self->fd, buf, buf_len);
#endif

    if (serial_flush(self) < 0)
        printf("serial_flush() failed!\n");

    return (int)written_num;
}
