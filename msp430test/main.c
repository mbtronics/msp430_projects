/******************************************************************************
 *                  MSP-EXP430G2-LaunchPad User Experience Application
 *
 * 1. Device starts up in LPM3 + blinking LED to indicate device is alive
 *    + Upon first button press, device transitions to application mode
 * 2. Application Mode
 *    + Continuously sample ADC Temp Sensor channel, compare result against
 *      initial value
 *    + Set PWM based on measured ADC offset: Red LED for positive offset, Green
 *      LED for negative offset
 *    + Transmit temperature value via TimerA UART to PC
 *    + Button Press --> Calibrate using current temperature
 *                       Send character '°' via UART, notifying PC
 ******************************************************************************/

//#include <msp430x20x2.h>  <-taken care of by includeing io.h and setting -mmcu=msp430x2012 in cflags
#include <io.h>
#include <signal.h>

#define     LED0                  BIT0
#define     LED1                  BIT6
#define     LED_DIR               P1DIR
#define     LED_OUT               P1OUT

interrupt(TIMERA0_VECTOR) TIMERA0_ISR(void)
{
    LED_OUT ^= (LED0 + LED1); //Toggle both LEDs
}

int main(void)
{
    LED_DIR |= LED0 + LED1;
    //LED_OUT &= ~(LED0 + LED1);  //LED0 & LED1 = 0

    LED_OUT &= ~LED0;   //LED0 off
    LED_OUT |= LED1;    //LED1 on

    BCSCTL3 |= LFXT1S_2;
    TACTL |= TASSEL_1 | MC_1;
    TACCTL0 = CCIE;
    TACCR0 = 6000; //1sec

    eint();

    /* Main Application Loop */
    while(1)
    {

    }
}
