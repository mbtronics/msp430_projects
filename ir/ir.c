#include <io.h>
#include <signal.h>
#include "dbg.h"

// Comment out the next line to enable some basic support for my BN59-00942A Samsung TV Remote
#define DUMP_RAW_PULSES 1   // Dump pulse timings to serial port

/****************************************************/

#define SAMPLES 64  // number of samples to take

#define     IR_BIT  BIT6
#define     TXD BIT1 // TXD on P1.1
#define     RXD BIT2 // RXD on P1.2

/****************************************************/

volatile static uint8_t times[SAMPLES];
volatile static uint8_t times_index = 0;

/****************************************************/

static void cpu_init(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT

    // configure system clock
    BCSCTL1 = CALBC1_1MHZ; // Set range
    DCOCTL = CALDCO_1MHZ; // SMCLK = DCO = 1MHz
}

// Timer0 ISR
interrupt(TIMERA0_VECTOR) TIMERA0_ISR(void)
{
    uart_timer0_isr();
}

#ifndef DUMP_RAW_PULSES
enum
{
    IR_STATE_INIT,
    IR_STATE_HDR,
    IR_STATE_HDR2,
    IR_STATE_NEXT,
};

volatile static uint8_t state = IR_STATE_INIT;
#endif

static void ir_setup(void)
{
    // TimerA SMCLK in UP mode
    TACTL = TASSEL_2 | MC_1;
    // Set TACCR0, starts timer
    TACCR0 = 0xFFFF;

    times_index = 0;

    P1IES |= IR_BIT;    // falling edge
    P1DIR &= ~IR_BIT;
    P1IE |= IR_BIT;    // interrupt enable

#ifndef DUMP_RAW_PULSES
    state = IR_STATE_INIT;
#endif
    TAR = 0;
}

static void ir_decode(void)
{
    if (times_index >= SAMPLES)
        return;

#ifdef DUMP_RAW_PULSES
    times[times_index++] = TAR/32;
    P1IES ^= IR_BIT;
    TAR = 0;
#else
    switch(state)
    {
        case IR_STATE_INIT: // initial falling edge
            P1IES &= ~IR_BIT;   // on rising edge
            state = IR_STATE_HDR;
            TAR = 0;
        break;

        case IR_STATE_HDR:
            if (TAR > 4000)
            {
                P1IES |= IR_BIT;    // on falling
                state = IR_STATE_HDR2;
                TAR = 0;
            }
            else
                ir_setup();
        break;

        case IR_STATE_HDR2:
            if (TAR > 4000)
            {
                P1IES &= ~IR_BIT;    // on rising
                state = IR_STATE_NEXT;
                TAR = 0;
            }
            else
                ir_setup();
        break;

        case IR_STATE_NEXT:
            if (TAR > 2000)
                ir_setup();
            else
            {
                times[times_index++] = TAR/32;
                P1IES ^= IR_BIT;
                TAR = 0;
            }
        break;
    }
#endif
}

interrupt(PORT1_VECTOR) P1_ISR(void)
{
    if ((P1IFG & IR_BIT) == IR_BIT)
        ir_decode();

    if ((P1IFG & RXD) == RXD)
        uart_p1_isr();

    P1IFG = 0x00;   // clear interrupt flags
}


int main(void)
{
    uint8_t i;

    cpu_init();
      
    while(1)
    {
        dint();
        ir_setup();
        eint();

        while(times_index < SAMPLES)
        {
            // spin
        }

        P1IE &= ~IR_BIT;    // interrupt enable

        dint();
        uart_init();
        eint();

#ifdef DUMP_RAW_PULSES
        for (i=0;i<times_index;i++)
        {
            dbg_putdec(times[i]);
            dbg_putc(',');
        }
        dbg_newline();
#else
        for (i=0;i<times_index;i++)
        {
            dbg_putc(times[i] > 40 ? '1' : '0');
        }
        dbg_newline();
        dbg_newline();
#endif
    }
}


