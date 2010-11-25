/******************************************************************************
 *                  KISS V2.0
 ******************************************************************************/

//#include <msp430x20x2.h>  <-taken care of by includeing io.h and setting -mmcu=msp430x2012 in cflags
#include <io.h>
#include <signal.h>

#include "general.h"
#include "serial.h"

void InitializeClocks(void);
void InitializePorts(bool EnablePullups);

//////Config
static const bool EnablePullups = false;
//////EndConfig

///////////Fifo Managment
#define FifoLength 20
volatile char FifoReadIndex = 0;
volatile char FifoWriteIndex = 0;
volatile int EdgeDownOnMSB_PinNumberOnLSB[FifoLength];

char IncrementFifoIndex(char Index)
{
    if(Index == FifoLength-1)
    {
       Index = 0;
    }
    else
    {
        ++Index;
    }
    return Index;
}

int GetNextDataIfAvailable()
{
    int ReturnData = 0;
    if(FifoReadIndex != FifoWriteIndex)
    {
        ReturnData = EdgeDownOnMSB_PinNumberOnLSB[(int)FifoReadIndex];
        FifoReadIndex = IncrementFifoIndex(FifoReadIndex);
    }
    return ReturnData;
}

void WriteDataToFifo(char PinNumber, bool EdgeDown)
{
    int Data = PinNumber;
    if(EdgeDown)
    {
        Data |= 0xFF00;
    }
    FifoWriteIndex = IncrementFifoIndex(FifoWriteIndex);
    EdgeDownOnMSB_PinNumberOnLSB[(int)FifoWriteIndex] = Data;
}
///////////////////////////

void ToggleOutputBit(char BitMask)
{
    bool TurnOff = (P1OUT & BitMask) > 0 ? true : false;
    if(TurnOff)
    {
        P1OUT &= ~BitMask;
    }
    else
    {
        P1OUT |= BitMask;
    }
}

void InitializeDebugPorts(bool EnablePullups)
{
    int Ports = BIT6 | BIT7;
    P1DIR |= Ports;//output
    P1REN &= ~Ports;//disable pull-up
}

int main(void)
{
    StopWatchdog();
    InitializeClocks();
    ConfigureTimerUart();
    InitializePorts(EnablePullups);

    eint();

    SendString("Starting\n\0");

    while(1)
    {
        int Data = GetNextDataIfAvailable();
        if(Data != 0)
        {

            if((Data & 0xFF00) == 0xFF00)
            {
                SendString("D");
            }
            else
            {
                SendString("U");
            }

            SendNible(Data & 0xFF);
            SendString("\n\0");
        }
    }
}




interrupt(PORT1_VECTOR) PORT1_ISR()
{
    char PinNumbers = P1IFG;

    int PinNumber;
    for(PinNumber = 0; PinNumber <= 7; ++PinNumber)
    {
        bool PinChanged = ((PinNumbers >> PinNumber) & 0x01) == 0x01 ? true : false;
        if(PinChanged)
        {
            bool EdgeDown = ((P1IES >> PinNumber) & 0x01) == 0x01 ? true : false;
            /*
            if(EdgeDown)
            {
                P1IES &= ~(0x01 << PinNumber);//Listen for EdgeUp events
            }
            else
            {
                P1IES |= (0x01 << PinNumber);//Listen for EdgeDown events
            }
            */
            WriteDataToFifo(PinNumber,EdgeDown);
        }
    }

    P1IFG &= ~PinNumbers;// clear handled pins
}

void InitializePorts(bool EnablePullups)
{
    int Ports = BIT0 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;

    P1DIR &= ~Ports;//input

    if(EnablePullups)
    {
        P1OUT |= Ports;
        P1REN |= Ports;//enable pull-up/
    }
    else
    {
        P1REN &= ~Ports;//disable pull-up
    }

	P1IE |= Ports;//Enable all the interrupts
	P1IES &= ~Ports;//Interrupt on low to high tranistions
	P1IFG &= ~Ports;//Clear interrupts
}

void InitializeClocks()
{
  BCSCTL1 = CALBC1_1MHZ;  // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);  // SMCLK = DCO / 8 = 1MHz
}


