#ifndef HAL_C_
#define HAL_C_

#include "stdint.h"

/**
 * \file hal_time.h
 * \brief Abstraction layer for system time access
 */

/**
 * Get the system time in milliseconds.
 *
 * The time value returned as 64-bit unsigned integer should represent the milliseconds
 * since the UNIX epoch (1970/01/01 00:00 UTC).
 *
 * \return the system time with millisecond resolution.
 */

uint64_t Hal_getTimeInMs(void);

uint64_t Hal_getCpuRunInMs(void);

#endif /* HAL_C_ */
