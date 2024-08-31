/*
 * cdc_printf.h
 *
 *  Created on: Aug 28, 2024
 *      Author: Ahsan
 */

#ifndef INC_CDC_PRINTF_H_
#define INC_CDC_PRINTF_H_

#include "usbd_cdc_if.h"  // Include USB CDC interface header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define CDC_BUFFER_SIZE 256  // Define a suitable buffer size

void CDC_printf(const char *format, ...);

#endif /* INC_CDC_PRINTF_H_ */
