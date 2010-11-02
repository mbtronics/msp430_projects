/***********************************************************************/
/*  This software code is base on code of Texas Instruments            */
/*  And modified by                                                    */
/*  Robert Bon                                                         */
/*  2003-12-9                                                          */
/*                                                                     */
/*                                                                     */
/***********************************************************************/

#ifndef _MSP_FLASH_H
#define _MSP_FLASH_H

#include "bioscnfg.h"

////////////////////////////////////////////////////////////////////////////////
// HEADER FILE
//

////////////////////////////////////////////////////////////////////////////////
// DEFINES
//

#define  FLASH_SIZE_INFOPAGE_INT        64  // 64 int = 128 BYTE

#define  FSEG_B      0x1000    /* Flash Segment B start address */
#define  FSEG_B_LAST 0x107F

#define  FSEG_A      0x1080    /* Flash Segment A start address */
#define  FSEG_A_LAST	0x10FF

#define   Flash_ReadInfo(index)        *(int  *) (FSEG_A + (index*2))
#define   Flash_ReadInfoMsb(index)     *(char *) (FSEG_A + (index*2)+1)
#define   Flash_ReadInfoLsb(index)     *(char *) (FSEG_A + (index*2))




////////////////////////////////////////////////////////////////////////////////
// TYPES
//

////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS DECLARATION
//

void Flash_Init     (void);
void Flash_WriteByte(char *Data_ptr, char byte);
void Flash_WriteWord(int *Data_ptr, int word);
void Flash_Erase    (int *Data_ptr);

int Flash_Copy       (int *source_ptr, int *dest_ptr, int size);
int Flash_CopyToEmpty(int *source_ptr_arg, int *dest_ptr_arg);

int Flash_WriteInfo    (int index, int word);
int Flash_IncrementInfo(int index);




#endif // _MSP_FLASH_H
