#ifndef _OS_SERIAL_H___
#define _OS_SERIAL_H___

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sSerialPort* SerialPort;


SerialPort serial_new(const char* dev_name,
                      int         baudRate,
                      char        parity,
                      uint8_t     dataBits,
                      uint8_t     stopBits);
void       serial_del(SerialPort self);
int        serial_open(SerialPort self);
void       serial_close(SerialPort self);
int        serial_write(SerialPort self, uint8_t* buf, int buf_len);
int        serial_read(SerialPort self, uint8_t* buf, int max_bytes);
int        serial_read_byte(SerialPort self);
size_t     serial_read_byte_until_timeout(SerialPort self,
                                          uint8_t*   dst,
                                          size_t     dst_size,
                                          int        resp_timeout,
                                          int        byte_timeout);


#ifdef __cplusplus
}
#endif

#endif /* _OS_SERIAL_H___ */
