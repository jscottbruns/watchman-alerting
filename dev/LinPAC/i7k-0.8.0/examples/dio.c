/*
 *  dio.c
 *
 *  v 0.0 2001.2.1 by Moki Matsushima  
 *
 *  open port, then digital IO with 8054 at slot 2/address 01
 *
 *  History
 *
 *  v 0.0 2001.2.1 by Moki Matsushima
 *    create, ... ok, i keep this as simple as possible
 */


#include<stdio.h>
#include<stdlib.h>
#include "msw.h"


char szSend[80], szReceive[80];
DWORD dwBuf[12];
float fBuf[12];

/* ------------------------------------------------------------------- */
int main()
{
	int i, wRetVal;
	DWORD temp;
	/* COM2, Baudrate=9600, 8 data bit, non-parity, 1 stop bit */



	wRetVal = Open_Com(COM1, 9600, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}
	//--- digital output ----
	dwBuf[0] = 1;				// COM Port
	dwBuf[1] = 1;				// Address
	dwBuf[2] = 0x8054;			// ID
	dwBuf[3] = 0;				// CheckSum disable
	dwBuf[4] = 10;				// TimeOut , 100 msecond
	dwBuf[5] = 0x01;			// digital output
	dwBuf[6] = 0;				// string debug
	dwBuf[7] = 2;				// slot

	for (i = 0; i < 4; i++) {
		wRetVal = DigitalOut_8K(dwBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("DigitalOut_8K() Error !, Error Code=%d\n", wRetVal);
		getch();
		dwBuf[5] = dwBuf[5] * 2;
		temp = dwBuf[5];
		wRetVal = DigitalIn_8K(dwBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("DigitalIn_8K() Error !, Error Code=%d\n", wRetVal);
		printf("The Digital Input is %d\n", dwBuf[5]);
		dwBuf[5] = temp;
	}

	for (i = 0; i < 5; i++) {
		wRetVal = DigitalOut_8K(dwBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("DigitalOut_8K() Error !, Error Code=%d\n", wRetVal);
		getch();
		dwBuf[5] = dwBuf[5] / 2;
		temp = dwBuf[5];
		wRetVal = DigitalIn_8K(dwBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("DigitalIn_8K() Error !, Error Code=%d\n", wRetVal);
		printf("The Digital Input is %d\n", dwBuf[5]);
		dwBuf[5] = temp;
	}
	Close_Com(COM1);
	return 0;
}
