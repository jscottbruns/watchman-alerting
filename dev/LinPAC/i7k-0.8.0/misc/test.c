/*
 *  test.c  this is a old porgram from dos, and here for testing the
 * source codes of microsoft environment wrapping compatibility of msw.h
 *  there are some things a little hard to warp codes to unix due to the
 * different architecture and programming environment between unix and
 * microsoft, so, this program maybe has not as full function as when it
 * running under microsoft dos.  if you are looking for a program which
 * can do the primary test of i7000 action, try i7kon. 
 */  
#include   "msw.h"
	FILE * stream;

#define    TIMEOUT  5L
int iComPort, iChksum, iDelay1;
long int lBaudRate;
float ok_count[256], err_count[256];

	/*
	 * ---- main
	 * ------------------------------------------------------------- 
	 */ 
	main() 
{
	char cChar;
	load_learning();
	iComPort = 2;
	iChksum = 0;
	lBaudRate = 9600L;
	OPEN_COM(iComPort, lBaudRate);	/* default */
	for (;;)
		 {
		printf("\n************** Learning Demo Program ****************");
		show_status();
		printf("\n*       0 : init                   (for all module) *");
		printf("\n*       1 : search (1200 to 115200)(for all module) *");
		printf("\n*       2 : send command           (for all module) *");
		printf("\n*       3 : find learning kits     (7060/7012/7021) *");
		printf("\n*       4 : 7060       test                         *");
		printf("\n*       5 : 7012D      test                         *");
		printf("\n*       6 : 7017       test                         *");
		printf("\n*       7 : 7018       test                         *");
		printf("\n*       8 : 7017/7043  test                         *");
		printf("\n*       9 : 7012D/7021 test                         *");
		printf("\n*       H : information about I-7000 learning kits  *");
		printf("\n*       Q : quit                                    *");
		printf("\n*************** Press Keyword ***********************");
		printf("\n");
		cChar = getche();
		switch (cChar)
			 {
		case '0':
			init();
			break;
		case '1':
			search();
			break;
		case '2':
			send_command();
			break;
		case '3':
			find_learning();
			break;
		case '4':
			test_7060();
			break;
		case '5':
			test_7012();
			break;
		case '6':
			test_7017();
			break;
		case '9':
			test_7012_7021();
			break;
		case 'h':
		case 'H':
			help();
			break;
		case 'q':
		case 'Q':
			goto ret_label;
		default:
			printf(" --> Error Keyword");
			break;
			}
		}
  ret_label:CLOSE_COM(iComPort);
	printf("\n************** learning Demo Program ****************\n");
}


	/*
	 * ---- show status
	 * ------------------------------------------------------ 
	 */ 
	show_status() 
{
	printf("\n* STATUS : COM=%d,", iComPort);
	printf("Baud_Rate=%5ld,", lBaudRate);
	if (iChksum == 0)
		printf("Checksum=DISABLE   *");
	
	else
		printf("Checksum=ENABLE    *");
	printf("\n*---------------------------------------------------*");
}

help() 
{
	printf("\n*---------------------------------------------------*");
	printf("\n* Baud Rate: 9600, checksum: disable                *");
	printf("\n* Module * Addr * Type * Chan * Decription          *");
	printf("\n* I-7060 * 01   * 40   * 4+4  * 4*relay+4*DI        *");
	printf("\n* I-7012D* 02   * 08   * 1    * AD, +/- 10V range   *");
	printf("\n* I-7021 * 03   * 32   * 1    * DA, 0V-10V  range   *");
	printf("\n* I-7053 * 04   * 40   * 16   * DI, 4V-30V HI       *");
	printf("\n* I-7043 * 05   * 40   * 16   * DO, 100mA, 30V      *");
	printf("\n* I-7017 * 06   * 08   * 1    * AD, +/- 10V         *");
	printf("\n* I-7018 * 07   * 0E   * 1    * AD, J thermocouple  *");
	printf("\n*---------------------------------------------------*");
	printf("\nPress Any Key To Continue ");
	getch();
	getch();
}


	/*
	 * ---- function 0
	 * ------------------------------------------------------- 
	 */ 
	init() 
{
	int iRet, iPort;
	printf(" --> (0):init\n");
	printf("port (1/2/3/4)=");
	scanf("%d", &iPort);
	printf("baud rate(1200/2400/4800/9600/19200/38400/57600/115200)=");
	scanf("%ld", &lBaudRate);
	printf("chksum (0=DISABLE,others=ENABLE)=");
	scanf("%d", &iChksum);
	iRet = OPEN_COM(iPort, lBaudRate);
	if (iRet == 0) {
		printf("--> OK");
		iComPort = iPort;
	}
	
	else if (iRet == 1)
		printf("--> port error");
	
	else if (iRet == 2)
		printf("--> baudrate error");
}


	/*
	 * ---- function 1
	 * ------------------------------------------------------- 
	 */ 
char cCmd1[100], cCmd2[100], cCmd3[100];
search() 
{
	int i, iD1, iD2, iRet, j;
	long BaudRate;
	char cFlag;
	
		// printf(" Search address=00 to FF, baudrate=1200 to 115200,
		// press any key to stop\n");
		printf
		(" Search address=00 to FF, baudrate=1200 to 115200, please wait...\n");
	cCmd1[0] = '$';
	strcpy(cCmd3, "Test Command\r");
	for (;;)
		
			// for (i=0; i<256; i++) /* search from adress_00 to adress_FF 
			// 
			// 
			// */
			for (i = 0; i < 4; i++)	/* search from adress_00 to * *
									   * adress_FF */
			 {
			printf("\n---------------------------------------------\n");
			for (j = 3; j <= 10; j++)	/* search from BaudRate_1200 to *
										   * * BaudRate_115200 */
				 {
				switch (j)
					 {
				case 3:
					BaudRate = 1200L;
					break;
				case 4:
					BaudRate = 2400L;
					break;
				case 5:
					BaudRate = 4800L;
					break;
				case 6:
					BaudRate = 9600L;
					break;
				case 7:
					BaudRate = 19200L;
					break;
				case 8:
					BaudRate = 38400L;
					break;
				case 9:
					BaudRate = 57600L;
					break;
				case 10:
					BaudRate = 115200L;
					break;
					}
				
					/*
					 * OPEN_COM(iComPort,BaudRate); 
					 */ 
					printf("Baudrate: %u\n", BaudRate);
				iRet = OPEN_COM(iComPort, BaudRate);
				if (iRet != 0)
					printf
						(" Open Error, iComPort=%d, BaudRate=%ld iRet=%d",
						 iComPort, BaudRate, iRet);
				
					// SEND_CMD(iComPort,cCmd3,TIMEOUT,0); /* RS-232
					// settling time delay */
					// RECEIVE_CMD(iComPort,cCmd2,TIMEOUT,0); /* chksum
					// disable */
					iD1 = i / 16;
				iD2 = i % 16;
				cCmd1[1] = get_ascii(iD1);
				cCmd1[2] = get_ascii(iD2);
				cCmd1[3] = '2';
				cCmd1[4] = 0;	/* $AA2 --> read status */
				cFlag = 0;
				
					/*
					 * ------------------- checksum DISABLE
					 * ----------------------------------- 
					 */ 
					SEND_CMD(iComPort, cCmd1, TIMEOUT, 0);	/* chksum
															   * disable 
															 */
				iRet = RECEIVE_CMD(iComPort, cCmd2, TIMEOUT, 0);	/* chksum 
																	   * disable 
																	 */
				if (iRet == 0)
					 {
					cFlag = 1;	/* find module in chksum DISABLE */
					cCmd1[3] = 'M';
					cCmd1[4] = 0;	/* $AAM --> read module name */
					SEND_CMD(iComPort, cCmd1, TIMEOUT, 0);	/* chksum
															   * disable 
															 */
					iRet = RECEIVE_CMD(iComPort, cCmd2, TIMEOUT, 0);	/* chksum 
																		   * disable 
																		 */
					if (iRet == 0)
						 {
						printf
							("\nFind %c%c%c%c --> Address=%d, Chksum DISABLE, speed=%ld\n",
							 cCmd2[3], cCmd2[4], cCmd2[5], cCmd2[6], i,
							 BaudRate);
						}
					}
				
					/*
					 * ------------------- checksum ENABLE
					 * ----------------------------------- 
					 */ 
					cCmd1[3] = '2';
				cCmd1[4] = 0;	/* $AA2 --> read status */
				SEND_CMD(iComPort, cCmd1, TIMEOUT, 1);	/* chksum enable */
				iRet = RECEIVE_CMD(iComPort, cCmd2, TIMEOUT, 1);	/* chksum 
																	   * enable 
																	 */
				if (iRet == 0)
					 {
					cFlag = 1;	/* find module in chksum ENABLE */
					cCmd1[3] = 'M';
					cCmd1[4] = 0;	/* $AAM --> read module name */
					SEND_CMD(iComPort, cCmd1, TIMEOUT, 1);	/* chksum
															   * enable */
					iRet = RECEIVE_CMD(iComPort, cCmd2, TIMEOUT, 1);	/* chksum 
																		   * enable 
																		 */
					if (iRet == 0)
						 {
						printf
							("\nFind %c%c%c%c --> Address=%d, Chksum ENSABLE, speed=%ld\n",
							 cCmd2[3], cCmd2[4], cCmd2[5], cCmd2[6], i,
							 BaudRate);
						}
					}
				if (cFlag == 0)
					printf("[%ld,%x]", BaudRate, i);
				
					// if (kbhit()!=0) {getch(); goto ret_label;}
				}
			}
  ret_label:OPEN_COM(iComPort, lBaudRate);
	/*
	 * default 
	 */
}


	/*
	 * ----------------------------------------------------------------------- 
	 */ 
	get_ascii(int iHex) 
{
	if (iHex < 10)
		return (iHex + '0');
	
	else
		return ('A' + iHex - 10);
}


	/*
	 * ---- function 2
	 * ------------------------------------------------------- 
	 */ 
	send_command() 
{
	char cCmd1[50], cCmd2[50];
	int iRet;
	printf(" --> (2):send_command\n");
	printf("cmd=");
	scanf("%s", cCmd1);
	getch();
	printf("\ncCmd1 = %s", cCmd1);
	printf(" next line\n");
	SEND_CMD(iComPort, cCmd1, TIMEOUT, iChksum);
	
		// SEND_CMD(iComPort,"$022\r",TIMEOUT,iChksum);
		iRet = RECEIVE_CMD(iComPort, cCmd2, TIMEOUT, iChksum);
	printf("Port=%d, chksum=%d, Send=%s\n", iComPort, iChksum, cCmd1);
	if (iRet == 0)
		printf("Receive=%s", cCmd2);
	
	else if (iRet == 1)
		printf("Receive=com value error (must 1/2/3/4)");
	
	else if (iRet == 2)
		printf("Receive=Timeout");
	
	else if (iRet == 3)
		printf("Receive=chksum error");
	printf("\n-----------------------------------------------------");
	printf("\nPress <Enter> to continue ");
	getch();
}


	/*
	 * ---- function 3
	 * -------------------------------------------------------- 
	 */ 
	find_learning() 
{
	char cCmd[30], cResult[30];
	int iRet, i;
	for (i = '1'; i <= '7'; i++)
		 {
		strcpy(cCmd, "$01M");
		cCmd[2] = i;
		printf("\nSend=%s, ", cCmd);
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		if (iRet == 0)
			printf("Receive=%s", cCmd);
		
		else if (iRet == 1)
			printf("Receive=com value error (must 1/2/3/4)");
		
		else if (iRet == 2)
			printf("Receive=Timeout");
		
		else if (iRet == 3)
			printf("Receive=chksum error");
		}
	printf("\n-----------------------------------------------------");
	printf("\nPress <Enter> to continue ");
	getch();
	getch();
}


	/*
	 * ---- function 4
	 * -------------------------------------------------------- 
	 */ 
	test_7060() 
{
	char cCmd[50];
	int iRet;
	for (;;)
		 {
		strcpy(cCmd, "#010001");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		if ((iRet == 0) & (cCmd[0] == '>'))
			printf("\nStep1 : DO=1");
		
		else {
			printf("\ntest error");
			return;
		}
		delay_ms(200);
		strcpy(cCmd, "$016");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		printf(" --> DI=%c", cCmd[2]);
		delay_ms(200);
		if (kbhit() != 0) {
			getch();
			return;
		}
		strcpy(cCmd, "#010002");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		if ((iRet == 0) & (cCmd[0] == '>'))
			printf("\nStep2 : DO=2");
		
		else {
			printf("\ntest error");
			return;
		}
		delay_ms(200);
		strcpy(cCmd, "$016");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		printf(" --> DI=%c", cCmd[2]);
		delay_ms(200);
		if (kbhit() != 0) {
			getch();
			return;
		}
		strcpy(cCmd, "#010004");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		if ((iRet == 0) & (cCmd[0] == '>'))
			printf("\nStep3 : DO=4");
		
		else {
			printf("\ntest error");
			return;
		}
		delay_ms(200);
		strcpy(cCmd, "$016");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		printf(" --> DI=%c", cCmd[2]);
		delay_ms(200);
		if (kbhit() != 0) {
			getch();
			return;
		}
		strcpy(cCmd, "#010008");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		if ((iRet == 0) & (cCmd[0] == '>'))
			printf("\nStep4 : DO=8");
		
		else {
			printf("\ntest error");
			return;
		}
		delay_ms(200);
		strcpy(cCmd, "$016");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		printf(" --> DI=%c", cCmd[2]);
		printf("\n---------------------------");
		delay_ms(200);
		if (kbhit() != 0) {
			getch();
			return;
		}
		}
}


	/*
	 * ---- function 5
	 * -------------------------------------------------------- 
	 */ 
	test_7012() 
{
	char cCmd[50], j;
	int iRet;
	for (;;)
		 {
		strcpy(cCmd, "#02");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		printf("\n7012 %s", cCmd);
		delay_ms(300);
		if (kbhit() != 0) {
			getch();
			return;
		}
		}
}


	/*
	 * ---- function 6
	 * -------------------------------------------------------- 
	 */ 
	test_7017() 
{
	char cCmd[50], j;
	int iRet;
	for (;;)
		 {
		strcpy(cCmd, "#02");
		SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
		iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
		printf("\n7012 %s", cCmd);
		delay_ms(300);
		if (kbhit() != 0) {
			getch();
			return;
		}
		}
}


	/*
	 * ---- function 9
	 * -------------------------------------------------------- 
	 */ 
	test_7012_7021() 
{
	char cCmd[50], j;
	int iRet;
	for (;;)
		 {
		for (j = '1'; j <= '9'; j++)
			 {
			strcpy(cCmd, "#0300.000");
			cCmd[4] = j;
			SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
			iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
			if ((iRet == 0) & (cCmd[0] == '>'))
				printf("\nDA=%c.000", j);
			
			else {
				printf("\ntest error");
				return;
			}
			delay_ms(900);
			strcpy(cCmd, "#02");
			SEND_CMD(iComPort, cCmd, TIMEOUT, 0);
			iRet = RECEIVE_CMD(iComPort, cCmd, TIMEOUT, 0);
			printf(" --> AD=%s", cCmd);
			delay_ms(100);
			if (kbhit() != 0) {
				getch();
				return;
			}
			}
		printf("\n---------------------------");
		delay_ms(1000);
		if (kbhit() != 0) {
			getch();
			return;
		}
		}
}


	/*
	 * --------------------------------------------------------------------- 
	 */ 
	load_learning() 
{
	FILE * stream;
	int wRetVal;
	stream = fopen("LEARNING.DAT", "r");
	if (stream)
		 {
		fscanf(stream, "%d", &iDelay1);
		printf("\nLOAD LEARNING.DAT OK");
		fclose(stream);
		}
	
	else
		printf("\nOPEN LEARNING.DAT ERROR");
}
delay_ms(int t) 
{
	int i, j, k;
	float p, q, r;
	for (i = 0; i < t; i++)
		for (k = 0; k < iDelay1; k++)
			 {
			p = i + t;
			q = i + k;
			r = p * q;
			}
}


