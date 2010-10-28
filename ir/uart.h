#ifndef UART_H
#define UART_H 1

#include "common.h"

void uart_init(void);
BOOL uart_getc(uint8_t *c);
void uart_putc(uint8_t c);

void uart_p1_isr(void);
void uart_timer0_isr(void);

#endif

