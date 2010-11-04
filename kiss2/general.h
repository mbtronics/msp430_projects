#include <stdbool.h>

const int MAX_INT = 65535;

static void __inline__ delay(register unsigned int n);
void StopWatchdog(void);

// Delay Routine from mspgcc help file
static void __inline__ delay(register unsigned int n)
{
    __asm__ __volatile__ (
    "1: \n"
    " dec %[n] \n"
    " jne 1b \n"
        : [n] "+r"(n));
}

void StopWatchdog(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
}
