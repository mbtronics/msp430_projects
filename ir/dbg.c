#include "common.h"
#include "dbg.h"
#include "uart.h"

#define ABS(x) ( (x) < 0 ? (-(x)) : (x) )

/**************************************************************/

void dbg_putc(uint8_t c)
{
    uart_putc(c);
}

void dbg_newline(void)
{
    dbg_putc('\r');
    dbg_putc('\n');
}

void dbg_puts(const char *str)
{
    while(*str!=0)
        dbg_putc(*str++);
}

static char nibble_to_char(uint8_t nibble)
{
    if (nibble < 0xA)
        return nibble + '0';
    return nibble - 0xA + 'A';
}

void dbg_puthex8(uint8_t h)
{
    dbg_putc(nibble_to_char((h & 0xF0)>>4));
    dbg_putc(nibble_to_char(h & 0x0F));
}

void dbg_puthex32(uint32_t h)
{
    dbg_puthex8((h & 0xFF000000) >> 24);
    dbg_puthex8((h & 0x00FF0000) >> 16);
    dbg_puthex8((h & 0x0000FF00) >> 8);
    dbg_puthex8(h & 0xFF);
}

void dbg_puthex16(uint16_t h)
{
    dbg_puthex8((h & 0xFF00) >> 8);
    dbg_puthex8(h & 0xFF);
}

void dbg_put0x8(uint8_t h)
{
    dbg_putc('0');
    dbg_putc('x');
    dbg_puthex8(h);
}

void dbg_putsmem(const uint8_t *a, const uint8_t *b)
{
    while(a != b)
    {
        dbg_putc(*a);
        a++;
    }
}


void dbg_putdec(int32_t n)
{
    int32_t m;
    BOOL in_leading_zeroes = TRUE;

    if (n < 0)
        dbg_putc('-');

    n = ABS(n);

    for (m = 1000000000; m != 1; m/=10)
    {
        if ((n / m) != 0)
            in_leading_zeroes = FALSE;
        if (!in_leading_zeroes)
            dbg_putc(nibble_to_char(n / m));
        n = n % m;
    }
    dbg_putc(nibble_to_char(n));
}

void dbg_putbin(uint8_t b)
{
    dbg_putc('b');
    (b & 0x80) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x40) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x20) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x10) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x08) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x04) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x02) ? dbg_putc('1') : dbg_putc('0');
    (b & 0x01) ? dbg_putc('1') : dbg_putc('0');
}

