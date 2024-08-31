/*
 * cdc_printf.c
 *
 *  Created on: Aug 28, 2024
 *      Author: Ahsan
 */

#include"cdc_printf.h"

void CDC_printf(const char *format, ...) {
    char buffer[CDC_BUFFER_SIZE];
    va_list args;
    va_start(args, format);

    // Format the string into the buffer
    vsnprintf(buffer, sizeof(buffer), format, args);

    // Ensure the string is null-terminated
    buffer[CDC_BUFFER_SIZE - 1] = '\0';

    va_end(args);

    // Transmit the formatted string via USB CDC
    CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
}
