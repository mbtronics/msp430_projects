/******************************************************************************
 *                  KISS V2.0
 ******************************************************************************/

//#include <msp430x20x2.h>  <-taken care of by includeing io.h and setting -mmcu=msp430x2012 in cflags
#include <io.h>
#include <signal.h>

#include "general.h"
#include "serial.h"

void InitializeClocks(void);
void InitializePort1(void);

int main(void)
{
    StopWatchdog();
    InitializeClocks();
    ConfigureTimerUart();
    InitializePort1();

    eint();

    SendString("Starting\n\0");

    while(1)
    {
        if (~P1IN & BIT5)
        {
            SendString("P1.5 = 0\n\0");
        }
        else
        {
            SendString("P1.6 = 1\n\0");
        }
    }
}

void InitializePort1(void)
{
    P1REN &= ~BIT5;  //disable pull-up/down
    P1DIR &= ~BIT5; //input
}

void InitializeClocks(void)
{
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);                         // SMCLK = DCO / 8 = 1MHz
}


