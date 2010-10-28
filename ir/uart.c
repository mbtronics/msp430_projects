#include "uart.h"
/* Originally version from:
http://www.msp430launchpad.com/2010/08/half-duplex-software-uart-on-launchpad.html
*/

/****************************************************************/

#define TXD BIT1 // TXD on P1.1
#define RXD BIT2 // RXD on P1.2

#define FCPU 1000000
#define BAUDRATE 9600

#define BIT_TIME        (FCPU / BAUDRATE)
#define HALF_BIT_TIME   (BIT_TIME / 2)

/****************************************************************/

static volatile uint8_t bitCount; // Bit count, used when transmitting byte
static volatile unsigned int TXByte; // Value sent over UART when uart_putc() is called
static volatile unsigned int RXByte; // Value recieved once hasRecieved is set

static volatile BOOL isReceiving = FALSE; // Status for when the device is receiving
static volatile BOOL hasReceived = FALSE; // Lets the program know when a byte is received

static volatile BOOL isTransmitting = FALSE; // Status for when the device is receiving

/****************************************************************/
void uart_init(void)
{
    P1SEL |= TXD;
    P1DIR |= TXD;

    P1IES |= RXD; // RXD Hi/lo edge interrupt
    P1IFG &= ~RXD; // Clear RXD (flag) before enabling interrupt
    P1IE |= RXD; // Enable RXD interrupt
}

BOOL uart_getc(uint8_t *c)
{
    if (!hasReceived)
        return FALSE;
    *c = RXByte;
    hasReceived = FALSE;
    return TRUE;
}

void uart_putc(uint8_t c)
{
    TXByte = c;

    while(isReceiving); // Wait for RX completion

    isTransmitting = TRUE;

    CCTL0 = OUT; // TXD Idle as Mark
    TACTL = TASSEL_2 + MC_2; // SMCLK, continuous mode

    bitCount = 0xA; // Load Bit counter, 8 bits + ST/SP
    CCR0 = TAR; // Initialize compare register

    CCR0 += BIT_TIME; // Set time till first bit
    TXByte |= 0x100; // Add stop bit to TXByte (which is logical 1)
    TXByte = TXByte << 1; // Add start bit (which is logical 0)

    CCTL0 = CCIS0 + OUTMOD0 + CCIE; // Set signal, intial value, enable interrupts

    while ( CCTL0 & CCIE ); // Wait for previous TX completion

    while(isTransmitting);  // block
}

//interrupt(PORT1_VECTOR) PORT1_ISR(void)
void uart_p1_isr(void)
{
    isReceiving = TRUE;

    P1IE &= ~RXD; // Disable RXD interrupt
    P1IFG &= ~RXD; // Clear RXD IFG (interrupt flag)

    TACTL = TASSEL_2 + MC_2; // SMCLK, continuous mode
    CCR0 = TAR; // Initialize compare register
    CCR0 += HALF_BIT_TIME; // Set time till first bit
    CCTL0 = OUTMOD1 + CCIE; // Disable TX and enable interrupts

    RXByte = 0; // Initialize RXByte
    bitCount = 9; // Load Bit counter, 8 bits + start bit
}

//interrupt(TIMERA0_VECTOR) TIMERA0_ISR(void)
void uart_timer0_isr(void)
{
    if(!isReceiving)
    {
        CCR0 += BIT_TIME; // Add Offset to CCR0
        if ( bitCount == 0) // If all bits TXed
        {
            TACTL = TASSEL_2; // SMCLK, timer off (for power consumption)
            CCTL0 &= ~ CCIE ; // Disable interrupt
            isTransmitting = FALSE;
        }
        else
        {
            CCTL0 |= OUTMOD2; // Set TX bit to 0
            if (TXByte & 0x01)
                CCTL0 &= ~ OUTMOD2; // If it should be 1, set it to 1
            TXByte = TXByte >> 1;
            bitCount --;
        }
    }
    else
    {
        CCR0 += BIT_TIME; // Add Offset to CCR0
        if ( bitCount == 0)
        {
            TACTL = TASSEL_2; // SMCLK, timer off (for power consumption)
            CCTL0 &= ~ CCIE ; // Disable interrupt

            isReceiving = FALSE;

            P1IFG &= ~RXD; // clear RXD IFG (interrupt flag)
            P1IE |= RXD; // enabled RXD interrupt

            if ( (RXByte & 0x201) == 0x200) // Validate the start and stop bits are correct
            {
                RXByte = RXByte >> 1; // Remove start bit
                RXByte &= 0xFF; // Remove stop bit
                hasReceived = TRUE;
            }
        }
        else
        {
            if ( (P1IN & RXD) == RXD) // If bit is set?
                RXByte |= 0x400; // Set the value in the RXByte
            RXByte = RXByte >> 1; // Shift the bits down
            bitCount --;
        }
    }
}



