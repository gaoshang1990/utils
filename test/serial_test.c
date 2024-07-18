
#include "hal_serial.h"
#include "utils_log.h"

int serial_test()
{
    SLOG_INFO("--- serial test start ---\n");

    SerialPort sp = SerialPort_create("COM1", 9600, 8, 'N', 1);
    if (sp == NULL) {
        SLOG_ERROR("SerialPort_create failed");
        return -1;
    }

    uint64_t start = cpu_ms();

    SerialPort_setTimeout(sp, 2000);
    while (!SerialPort_checkTimeout(sp)) {
        SerialPort_setTimeout(sp, 100);
    }
    uint64_t end = cpu_ms();
    SLOG_INFO("SerialPort_setTimeout: %lld ms", end - start);

    start = cpu_ms();
    SerialPort_setTimeout(sp, 2000);
    while (!SerialPort_checkTimeout(sp)) {
        // SerialPort_setTimeout(sp, 100);
    }
    end = cpu_ms();
    SLOG_INFO("SerialPort_checkTimeout: %lld ms", end - start);

    serial_del(sp);

    SLOG_INFO("--- serial test done ---\n");

    return 0;
}
