//   Conditions for 9600/4=2400 Baud SW UART, SMCLK = 1MHz
#define     Bitime_5              0x05*4                      // ~ 0.5 bit length + small adjustment
#define     Bitime                13*4//0x0D
#define     TXD                   BIT1                      // TXD on P1.1
#define     RXD                   BIT2                      // RXD on P1.2

void ConfigureTimerUart(void);
void SendChar(unsigned int Data);
void SendString(char * String);

unsigned int TXByte;
unsigned char BitCnt;

const int MaxStringLength = 100;

void SendString(char * String)
{
    int i;

    for (i=0; i<MaxStringLength; ++i)
    {
        if (String[i] == 0)
        {
            break;
        }
        else
        {
            SendChar(String[i]);
        }
    }
}

void ConfigureTimerUart(void)
{
    CCTL0 = OUT;                               // TXD Idle as Mark
    TACTL = TASSEL_2 + MC_2 + ID_3;            // SMCLK/8, continuous mode
    P1SEL |= TXD + RXD;                        //
    P1DIR |= TXD;                              //
}

// Function Transmits Character from TXByte
void SendChar(unsigned int Data)
{
    TXByte = Data;

    asm(						// Re-implement timer capture in assembly
    "JMP 2f \n"
    "1: \n"
    "MOV &0x0170,&0x0172 \n"
    "2: \n"
    "CMP &0x0170,&0x0172\n"
    "JNZ 1b \n");
    CCR0 += Bitime;  			    // Some time till first bit
    BitCnt = 0xA;                             // Load Bit counter, 8data + ST/SP
    TXByte |= 0x100;                        // Add mark stop bit to TXByte
    TXByte = TXByte << 1;                 // Add space start bit
    CCTL0 =  CCIS0 + OUTMOD0 + CCIE;          // TXD = mark = idle
    while ( CCTL0 & CCIE )delay(5);                   // Wait for TX completion, added short delay
}

interrupt(TIMERA0_VECTOR) TIMERA0_ISR(void)
{
    CCR0 += Bitime;                           // Add Offset to CCR0
    if (CCTL0 & CCIS0)                        // TX on CCI0B?
    {
        if ( BitCnt == 0)
        {
            CCTL0 &= ~CCIE;                        // All bits TXed, disable interrupt
        }
        else
        {
            CCTL0 |= OUTMOD2;                    // TX Space
            if (TXByte & 0x01)
            {
                CCTL0 &= ~ OUTMOD2;                   // TX Mark
            }
            TXByte = TXByte >> 1;
            BitCnt --;
        }
    }
}
