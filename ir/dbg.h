#ifndef DBG_H
#define DBG_H 1

#include "common.h"
#include "uart.h"

void dbg_putc(uint8_t c);
void dbg_newline(void);
void dbg_puts(const char *str);
void dbg_puthex8(uint8_t h);
void dbg_puthex16(uint16_t h);
void dbg_puthex32(uint32_t h);
void dbg_put0x8(uint8_t h);
void dbg_putdec(int32_t i);
void dbg_putbin(uint8_t b);
void dbg_putsmem(const uint8_t *a, const uint8_t *b);

#endif

