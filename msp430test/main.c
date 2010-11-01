/******************************************************************************
 *                  KISS V2.0
 ******************************************************************************/

//#include <msp430x20x2.h>  <-taken care of by includeing io.h and setting -mmcu=msp430x2012 in cflags
#include <io.h>
#include <signal.h>

#include "general.h"
#include "serial.h"

#define SAMPLES 64  // number of samples to take

#define     BUTTON      BIT3
#define     BUTTON_OUT  P1OUT     //Port 1 output register
#define     BUTTON_DIR  P1DIR     //Port 1 direction register (0=input, 1=output)
#define     BUTTON_IN   P1IN      //Port 1 input register
#define     BUTTON_IE   P1IE      //Port 1 interrupt enable register
#define     BUTTON_IES  P1IES     //Port 1 interrupt edge register (0=0>1, 1=1>0)
#define     BUTTON_IFG  P1IFG     //Port 1 interrupt flag register
#define     BUTTON_REN  P1REN     //Port 1 resistor enable register

#define     IR_BIT      BIT5

#define VOLUME_UP   0
#define VOLUME_DOWN 1

void InitializeIr(void);
void InitializeClocks(void);
void InitializeButton(void);
void ChangeVolume(void);
void TogglePause(void);
void Next(void);
void IrDecode(void);

unsigned int Volume = 0;
bool VolumeUp = true;
const unsigned int MaxVol = 5;

volatile static uint8_t times[SAMPLES];
volatile static uint8_t times_index = 0;

int main(void)
{
    unsigned int Counter=0, Overflows=0;

    StopWatchdog();
    InitializeClocks();
    InitializeButton();
    InitializeIr();
    eint();

    ConfigureTimerUart();

    SendString("KISS V2.0\n\0");

    while(1)
    {
        Counter = 0;
        Overflows = 0;

        while(~BUTTON_IN & BUTTON)
        {
            ++Counter;

            if (Counter == MAX_INT)
            {
                ++Overflows;

                if (Overflows > 1)
                {
                    ChangeVolume();
                }
            }
        }

        if (Overflows < 2)
        {
            if (Overflows == 0 && Counter > 1000 && Counter < 50000)
            {
                Next();
            }
            else if (Overflows > 0 || Counter >= 50000)
            {
                TogglePause();
            }
        }
    }
}

interrupt(PORT1_VECTOR) P1_ISR(void)
{
    ConfigureTimerUart();
    SendString("Interupt port 1\n");

    if ((P1IFG & IR_BIT) == IR_BIT)
    {
        SendString("IR!\n");
        IrDecode();
    }

    P1IFG = 0x00;   // clear interrupt flags
}

void IrDecode(void)
{
    int i;

    if (times_index >= SAMPLES)
    {
        for (i=0; i<times_index; ++i)
        {
            SendInt(times[i]);
            SendChar(',');
        }
        SendChar('\n');
        times_index = 0;
    }

    times[times_index++] = TAR/32;
    P1IES ^= IR_BIT;
    TAR = 0;
}

void InitializeIr(void)
{
    times_index = 0;

    P1IES |= IR_BIT;    // falling edge
    P1DIR &= ~IR_BIT;
    P1IE |= IR_BIT;    // interrupt enable

    TAR = 0;
}

void InitializeClocks(void)
{
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);                         // SMCLK = DCO / 8 = 1MHz
}

void InitializeButton(void)                 // Configure Push Button
{
  BUTTON_DIR &= ~BUTTON;    //input
  BUTTON_REN |= BUTTON;     //enable pull-up resistor
}

void TogglePause(void)
{
    SendString("toggle pause\n\0");
}

void Next(void)
{
    SendString("next\n\0");
}

void ChangeVolume(void)
{
    if (VolumeUp)
    {
        if (Volume < MaxVol)
        {
            ++Volume;
            SendString("volume up\n\0");
        }

        if (Volume == MaxVol)
        {
            VolumeUp = false;
        }
    }
    else
    {
        if (Volume > 0)
        {
            --Volume;
            SendString("volume down\n\0");
        }

        if (Volume == 0)
        {
            VolumeUp = true;
        }
    }
}
