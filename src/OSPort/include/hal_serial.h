#ifndef SRC_LINK_LAYER_SERIAL_PORT_H_
#define SRC_LINK_LAYER_SERIAL_PORT_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file hal_serial.h
 * \brief Abstraction layer for serial ports.
 */

/*! \addtogroup hal Platform (Hardware/OS) abstraction layer
 *
 *  @{
 */

/**
 * @defgroup HAL_SERIAL Access to serial interfaces
 *
 *  Serial interface abstraction layer. This functions have to be implemented to
 *  port application to new platforms when the serial link layers are required.
 *
 * @{
 */

typedef struct sSerialPort* SerialPort;

typedef enum {
    SERIAL_PORT_ERROR_NONE             = 0,
    SERIAL_PORT_ERROR_INVALID_ARGUMENT = 1,
    SERIAL_PORT_ERROR_INVALID_BAUDRATE = 2,
    SERIAL_PORT_ERROR_OPEN_FAILED      = 3,
    SERIAL_PORT_ERROR_UNKNOWN          = 99
} SerialPortError;

/**
 * \brief Create a new SerialPort instance
 * \param interfaceName identifier or name of the serial interface (e.g. "/dev/ttyS1" or "COM4")
 * \param baudRate the baud rate in baud (e.g. 9600)
 * \param dataBits the number of data bits (usually 8)
 * \param parity defines what kind of parity to use ('E' - even parity, 'O' - odd parity, 'N' - no parity)
 * \param stopBits the number of stop buts (usually 1)
 * \return the new SerialPort instance
 */
SerialPort SerialPort_create(const char* interfaceName, int baudRate, uint8_t dataBits, char parity, uint8_t stopBits);

/**
 * \brief Destroy the SerialPort instance and release all resources
 */
void SerialPort_destroy(SerialPort self);

bool SerialPort_modify(SerialPort self, int baudRate, uint8_t dataBits, char parity, uint8_t stopBits);

/**
 * \brief Open the serial interface
 * \return true in case of success, false otherwise (use \ref SerialPort_getLastError for a detailed error code)
 */
bool SerialPort_open(SerialPort self);

/**
 * \brief Close (release) the serial interface
 */
void SerialPort_close(SerialPort self);

/**
 * \brief Get the baudrate used by the serial interface
 * \return the baud rate in baud
 */
int SerialPort_getBaudRate(SerialPort self);

/**
 * \brief Set the timeout used for message reception
 * \param timeout the timeout value in ms.
 */
void SerialPort_setTimeout(SerialPort self, int timeout);

void SerialPort_resetTimeout(SerialPort self, int timeout, uint64_t rcv_sys_time_ms);

void SerialPort_getTimeout(SerialPort self, uint64_t* rcv_sys_time_ms);

/**
 * \brief check the timeout used for message reception
 * \return 1 when timeout happened.
 */
int SerialPort_checkTimeout(SerialPort self);

/**
 * \brief Discard all data in the input buffer of the serial interface
 */
void SerialPort_discardInBuffer(SerialPort self);

int32_t SerialPort_read(SerialPort self, uint8_t* buff, int32_t maxBytes);

/**
 * \brief Read a byte from the interface
 * \return number of read bytes of -1 in case of an error
 */
int SerialPort_readByte(SerialPort self);

/**
 * \brief Write the number of bytes from the buffer to the serial interface
 * \param buffer the buffer containing the data to write
 * \param startPos start position in the buffer of the data to write
 * \param numberOfBytes number of bytes to write
 * \return number of bytes written, or -1 in case of an error
 */
int SerialPort_write(SerialPort self, uint8_t* buffer, int startPos, int numberOfBytes);

/**
 * \brief Get the error code of the last operation
 */
SerialPortError SerialPort_getLastError(SerialPort self);


#ifdef __cplusplus
}
#endif


#endif /* SRC_LINK_LAYER_SERIAL_PORT_H_ */
