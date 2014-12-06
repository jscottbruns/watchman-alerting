/*
 *  eeprom.c
 *
 *  v 0.0 2004.9.1 by Moki Matsushima  
 *
 *  read/write eeprom inside lincon
 *
 *  History
 *
 *  v 0.0 2004.9.1 by Moki Matsushima
 *    create, ... ok, i keep this as simple as possible
 */


#include<stdio.h>
#include<stdlib.h>
#include "msw.h"


/* ------------------------------------------------------------------- */
int main()
{
	int i, wRetVal;
	DWORD temp;
	/* COM2, Baudrate=9600, 8 data bit, non-parity, 1 stop bit */

	wRetVal = Open_Slot(0);
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	i=2;
	
/*	Enable_EEP();
		
	wRetVal = 20;
	printf("Write EEP = %d\n", wRetVal);
	Write_EEP(0,i,(unsigned char)wRetVal);
	wRetVal = Read_EEP(0,i);
	printf("Read EEP = %d\n", wRetVal);
	
	
	wRetVal = 25;
	printf("Write EEP = %d\n", wRetVal);
	Write_EEP(1,i,(unsigned char)wRetVal);
	wRetVal = Read_EEP(1,i);
	printf("Read EEP = %d\n", wRetVal);
	
	
	wRetVal = 30;
	printf("Write EEP = %d\n", wRetVal);
  Write_EEP(2,i,(unsigned char)wRetVal);
	wRetVal = Read_EEP(2,i);
	printf("Read EEP = %d\n", wRetVal);
		
	Disable_EEP();
*/	
 Enable_EEP();
 wRetVal = 20;
 printf("Write EEP block 0 = %d\n", wRetVal);
 Write_EEP(0,i,(unsigned char)wRetVal);
 
 wRetVal = 25;
 printf("Write EEP block 1 = %d\n", wRetVal);
 Write_EEP(1,i,(unsigned char)wRetVal);
 
 wRetVal = 30;
 printf("Write EEP block 2 = %d\n", wRetVal);
 Write_EEP(2,i,(unsigned char)wRetVal);
 Disable_EEP();
 
 wRetVal = Read_EEP(0,i);
 printf("Read EEP block 0 = %d\n", wRetVal);
 wRetVal = Read_EEP(1,i);
 printf("Read EEP block 1 = %d\n", wRetVal);
 wRetVal = Read_EEP(2,i);
 printf("Read EEP block 2 = %d\n", wRetVal);

 Disable_EEP();
	
/*	for (i=0;i<11;i++) {
	wRetVal = Read_EEP(0,i);
	printf("Read EEP = %d\n", wRetVal);
	}
*/	//getch();
	Close_Slot(0);
	return 0;
}
