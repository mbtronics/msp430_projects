/***********************************************************************/
/*  This software code is base on code of Texas Instruments            */
/*  And modified by                                                    */
/*  Robert Bon                                                         */
/*  2003-12-9                                                          */
/*                                                                     */
/*                                                                     */
/***********************************************************************/
/*  flash_idle.c                                            2000-06-20 */
/*                                                                     */
/*                  Flash erase and program functions                  */
/*                                                                     */
/* Below functions using the direct Flash programming algorithm.       */
/* After starting a flash write or erase cycle, the CPU will wait      */
/* until the flash is read-accessable again, so no program must be     */
/* copied into RAM. However, during flash programming, the CPU is      */
/* in "idle" mode.                                                     */
/*                                                                     */ 
/* Note: Since all interrupt vectors are unavailable during flash      */
/* programming, all interrupts must be disabled.                       */
/*                                                                     */
/* Anton Muehlhofer                     Texas Instruments Incorporated */
/***********************************************************************/

////////////////////////////////////////////////////////////////////////////////
// HEADER FILE
//

#include "bioscnfg.h"
#include "bios.h"
#include "mspflash.h"    /* function prototypes */




////////////////////////////////////////////////////////////////////////////////
// DEFINES
//


////////////////////////////////////////////////////////////////////////////////
// TYPES
//

////////////////////////////////////////////////////////////////////////////////
// VARIABLE DECLARATION
//

////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS DECLARATION
//

void Flash_CheckOnStartUp(void);

////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS IMPLEMENTATION
//

void Flash_Init(void)
{
	
#if MICRO_CONTROLLER_FAMILY == uC_Family_MSP430
	// === Initialize system ================================================
// IFG1=0;                    /* clear interrupt flag1 */
// WDTCTL=0x5A80;             /* stop WD */
// DCOCTL=0x60;               /* DCO frequency ~750 kHz (RSEL=4) */
// BCSCTL1=0x04;

//	FCTL2=0x0A542;             /* select MCLK/3 for flash timing */

	//FCTL2=0x0A500 + 0x80 + 24;
//	FCTL2=0x0A500 + 0x40 + 2;

	FCTL2=0x0A542;             /* select MCLK/3 for flash timing */


// _DINT();                   /* disable all interrupts to protect CPU */
									 /* during programming from system crash */
//	IE1=0x0;                   /* disable all NMI-Interrupt sources */
	Flash_CheckOnStartUp();
#endif

}



/***********************************************************************/
/*                         Flash_wb                                    */
/* programs 1 byte (8 bit) into the flash memory                       */
/***********************************************************************/
void Flash_WriteByte(char *Data_ptr, char byte)
{

	unsigned int rememberIE1;

	_DINT();                   /* disable all interrupts to protect CPU */
										/* during programming from system crash */
	IFG1=0;                    /* clear interrupt flag1 */
	WDTCTL=0x5A80;             /* stop WD */
//	DCOCTL=0x60;               /* DCO frequency ~750 kHz (RSEL=4) */
//	BCSCTL1=0x04;
	rememberIE1 = IE1;
	IE1=0x0;                   /* disable all NMI-Interrupt sources */

	FCTL3 = 0x0A500;          /* Lock = 0 */
	FCTL1 = 0x0A540;          /* WRT = 1 */
	*Data_ptr=byte;           /* program Flash word */
	FCTL1 = 0x0A500;          /* WRT = 0 */
	FCTL3 = 0x0A510;          /* Lock = 1 */

	IE1  = rememberIE1;
	_EINT();                   /* Enable all interrupts again */

}

/***********************************************************************/
/* programs 1 word (16 bits) into the flash memory                     */
/***********************************************************************/
void Flash_WriteWord(int *Data_ptr, int word)
{

	unsigned int rememberIE1;

	_DINT();                   /* disable all interrupts to protect CPU */
										/* during programming from system crash */
	IFG1=0;                    /* clear interrupt flag1 */
	WDTCTL=0x5A80;             /* stop WD */
	//	DCOCTL=0x60;               /* DCO frequency ~750 kHz (RSEL=4) */
	//	BCSCTL1=0x04;
	rememberIE1 = IE1;
	IE1=0x0;                   /* disable all NMI-Interrupt sources */

	FCTL3 = 0x0A500;          /* Lock = 0 */
	FCTL1 = 0x0A540;          /* WRT = 1 */
	*Data_ptr=word;           /* program Flash word */
	FCTL1 = 0x0A500;          /* WRT = 0 */
	FCTL3 = 0x0A510;          /* Lock = 1 */

	IE1  = rememberIE1;
	_EINT();                   /* Enable all interrupts again */

}

/***********************************************************************/
/* erases 1 Segment of flash memory                                    */
/***********************************************************************/
void Flash_Erase(int *Data_ptr)
{

	unsigned int rememberIE1;

	_DINT();                   /* disable all interrupts to protect CPU */
										/* during programming from system crash */
	IFG1=0;                    /* clear interrupt flag1 */
	WDTCTL=0x5A80;             /* stop WD */
//	DCOCTL=0x60;               /* DCO frequency ~750 kHz (RSEL=4) */
//	BCSCTL1=0x04;
	rememberIE1 = IE1;
	IE1=0x0;                   /* disable all NMI-Interrupt sources */


	FCTL3 = 0x0A500;          /* Lock = 0 */
	FCTL1 = 0x0A502;          /* ERASE = 1 */
	*Data_ptr=0;              /* erase Flash segment */
	FCTL1 = 0x0A500;          /* ERASE = 0 */
	FCTL3 = 0x0A510;          /* Lock = 1 */


	IE1  = rememberIE1;
	_EINT();                   /* Enable all interrupts again */

}



/***********************************************************************/
/*                          Flash_copy                                 */
/***********************************************************************/
/* copies segment A into B with verify and returns 0 if operation was  */
/* successful or address of flash memory, which couldn't be programmed.*/
/* This function tries to program one word up to 5 times. If the       */
/* verify still fails, the next word will be programmed.               */
/***********************************************************************/

int Flash_Copy(int *source_ptr, int *dest_ptr, int size)
{

	unsigned int counter;
	unsigned int w;
	int error=0;
	int word;

	for(w=0; w<size; w++)
	{
		counter=0;
		do                             /* tries to program word 5 times  */
		{
			word = *source_ptr;
			Flash_WriteWord(dest_ptr, word);
			counter++;
		} while((*dest_ptr != word) && (counter < 4));
		if(counter > 4)
		{
			error=(unsigned int) dest_ptr;
		}
		dest_ptr++;
		source_ptr++;
	}
	return(error);

}




int Flash_WriteInfo(int index, int word)
{
	if (index >= FLASH_SIZE_INFOPAGE_INT)
	{
		return (-1);
	}
//	index *= 2;

	Flash_Copy     ((int*)(FSEG_A              ), (int*)(FSEG_B            ), index); 
	Flash_WriteWord(                              (int*)(FSEG_B + 2*index  ),  word);
	Flash_Copy     ((int*)(FSEG_A + 2*index + 2), (int*)(FSEG_B + 2*index + 2), ((FLASH_SIZE_INFOPAGE_INT - 1) - index)); 
                  
	Flash_Erase    ((int*)FSEG_A);
				       
	Flash_Copy     ((int*)FSEG_B,  (int*)FSEG_A , FLASH_SIZE_INFOPAGE_INT);
	Flash_Erase    ((int*)FSEG_B);
	

	return (0);
}	

int Flash_IncrementInfo(int index)
{
	int i;
	i = Flash_ReadInfo(index);
	i++;
	return (Flash_WriteInfo(index, i));
}



// This function check it the msp430 was power off during a Flash_WriteInfo().
// Then it is possible that page A is empty ((erased),
// but fortunately the information was first copied to paga B.
// So if page A is empty the copy page B to page A.
void Flash_CheckOnStartUp(void)
{
	int i;
	for (i= 0; i <FLASH_SIZE_INFOPAGE_INT; i++)
	{
		if (0xFFFF != Flash_ReadInfo(i))
		{
			// erstaat iets in de page A
			// dus spring maar uit de functie.
			return;
		}
	}
	Flash_Copy     ((int*)FSEG_B,  (int*)FSEG_A , FLASH_SIZE_INFOPAGE_INT); 
	Flash_Erase    ((int*)FSEG_B);
	
}			

