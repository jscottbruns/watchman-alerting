/*
 * 
 * source codes
 * 
 * different architecture and programming environment between unix and
 * microsoft, so, this program maybe has not as full function as when
 * running under microsoft dos.
 * can do the primary test of i7000
 */  
#include   "msw.h"
	

#define    TIMEOUT  5L




	/*
	 * ---- main
	 * ------------------------------------------------------------- 
	 */ 
	
{
	
	
	
	iChksum = 0;
	lBaudRate = 9600L;
	
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
		
			init();
			break;
		
			search();
			break;
		
			send_command();
			break;
		
			find_learning();
			break;
		
			test_7060();
			break;
		
			test_7012();
			break;
		
			test_7017();
			break;
		
			test_7012_7021();
			break;
		
		
			help();
			break;
		
		
			goto ret_label;
		
			printf(" --> Error Keyword");
			break;
			
		
  
	



	/*
	 * ---- show status
	 * ------------------------------------------------------ 
	 */ 
	
{
	
	
	
		printf("Checksum=DISABLE   *");
	
	else
		printf("Checksum=ENABLE    *");
	



{
	
	
	
	
	
	
	
	
	
	
	
	
	
	getch();



	/*
	 * ---- function 0
	 * ------------------------------------------------------- 
	 */ 
	
{
	
	
	
	scanf("%d", &iPort);
	
	scanf("%ld", &lBaudRate);
	
	scanf("%d", &iChksum);
	
	
		printf("--> OK");
		iComPort = iPort;
	}
	
	else if (iRet == 1)
		printf("--> port error");
	
	else if (iRet == 2)
		printf("--> baudrate error");



	/*
	 * ---- function 1
	 * ------------------------------------------------------- 
	 */ 
char cCmd1[100], cCmd2[100], cCmd3[100];

{
	
	
	
	
		// printf(" Search address=00 to FF, baudrate=1200 to 115200,
		// press any key to stop\n");
		printf
		(" Search address=00 to FF, baudrate=1200 to 115200, please wait...\n");
	
	
	
		
			// for (i=0; i<256; i++) /* search from adress_00 to adress_FF 
			// 
			// 
			// */
			for (i = 0; i < 4; i++)	/* search from adress_00 to * *
									   * adress_FF */
			
			
			
										   * * BaudRate_115200 */
				
				
					
				
					BaudRate = 1200L;
					break;
				
					BaudRate = 2400L;
					break;
				
					BaudRate = 4800L;
					break;
				
					BaudRate = 9600L;
					break;
				
					BaudRate = 19200L;
					break;
				
					BaudRate = 38400L;
					break;
				
					BaudRate = 57600L;
					break;
				
					BaudRate = 115200L;
					break;
					
				
					/*
					 * OPEN_COM(iComPort,BaudRate); 
					 */ 
					
				
				
					printf
						(" Open Error, iComPort=%d, BaudRate=%ld iRet=%d",
						 iComPort, BaudRate, iRet);
				
					// SEND_CMD(iComPort,cCmd3,TIMEOUT,0); /* RS-232
					// settling time delay */
					// RECEIVE_CMD(iComPort,cCmd2,TIMEOUT,0); /* chksum
					// disable */
					
				iD2 = i % 16;
				
				
				
				
				
				
					/*
					 * ------------------- checksum DISABLE
					 * ----------------------------------- 
					 */ 
					
															   * disable 
															 */
				
																	   * disable 
																	 */
				
					
					
					
					
					
															   * disable 
															 */
					
																		   * disable 
																		 */
					
						
						
							("\nFind %c%c%c%c --> Address=%d, Chksum DISABLE, speed=%ld\n",
							 
							 BaudRate);
						
					
				
					/*
					 * ------------------- checksum ENABLE
					 * ----------------------------------- 
					 */ 
					
				cCmd1[4] = 0;	/* $AA2 --> read status */
				
				
																	   * enable 
																	 */
				
					
					
					
					
					
															   * enable */
					
																		   * enable 
																		 */
					
						
						
							("\nFind %c%c%c%c --> Address=%d, Chksum ENSABLE, speed=%ld\n",
							 
							 BaudRate);
						
					
				
					printf("[%ld,%x]", BaudRate, i);
				
					// if (kbhit()!=0) {getch(); goto ret_label;}
				}
			
  
	/*
	 * default 
	 */



	/*
	 * ----------------------------------------------------------------------- 
	 */ 
	
{
	
		return (iHex + '0');
	
	else
		return ('A' + iHex - 10);



	/*
	 * ---- function 2
	 * ------------------------------------------------------- 
	 */ 
	
{
	
	
	
	
	scanf("%s", cCmd1);
	
	
	
	
	
		// SEND_CMD(iComPort,"$022\r",TIMEOUT,iChksum);
		iRet = RECEIVE_CMD(iComPort, cCmd2, TIMEOUT, iChksum);
	
	
		printf("Receive=%s", cCmd2);
	
	else if (iRet == 1)
		printf("Receive=com value error (must 1/2/3/4)");
	
	else if (iRet == 2)
		printf("Receive=Timeout");
	
	else if (iRet == 3)
		printf("Receive=chksum error");
	
	
	



	/*
	 * ---- function 3
	 * -------------------------------------------------------- 
	 */ 
	
{
	
	
	
		
		
		
		
		
		
		
			printf("Receive=%s", cCmd);
		
		else if (iRet == 1)
			printf("Receive=com value error (must 1/2/3/4)");
		
		else if (iRet == 2)
			printf("Receive=Timeout");
		
		else if (iRet == 3)
			printf("Receive=chksum error");
		
	
	
	getch();
	getch();



	/*
	 * ---- function 4
	 * -------------------------------------------------------- 
	 */ 
	
{
	
	
	
		
		
		
		
		
			printf("\nStep1 : DO=1");
		
		else {
			printf("\ntest error");
			return;
		}
		
		
		
		
		
		
		
			getch();
			return;
		}
		
		
		
		
			printf("\nStep2 : DO=2");
		
		else {
			printf("\ntest error");
			return;
		}
		
		
		
		
		
		
		
			getch();
			return;
		}
		
		
		
		
			printf("\nStep3 : DO=4");
		
		else {
			printf("\ntest error");
			return;
		}
		
		
		
		
		
		
		
			getch();
			return;
		}
		
		
		
		
			printf("\nStep4 : DO=8");
		
		else {
			printf("\ntest error");
			return;
		}
		
		
		
		
		
		
		
		
			getch();
			return;
		}
		



	/*
	 * ---- function 5
	 * -------------------------------------------------------- 
	 */ 
	
{
	
	
	
		
		
		
		
		
		
		
			getch();
			return;
		}
		



	/*
	 * ---- function 6
	 * -------------------------------------------------------- 
	 */ 
	
{
	
	
	
		
		
		
		
		
		
		
			getch();
			return;
		}
		



	/*
	 * ---- function 9
	 * -------------------------------------------------------- 
	 */ 
	
{
	
	
	
		
		
			
			
			
			
			
			
				printf("\nDA=%c.000", j);
			
			else {
				printf("\ntest error");
				return;
			}
			
			
			
			
			
			
			
				getch();
				return;
			}
			
		
		
		
			getch();
			return;
		}
		



	/*
	 * --------------------------------------------------------------------- 
	 */ 
	
{
	
	
	
	
		
		
		
		
		
	
	else
		printf("\nOPEN LEARNING.DAT ERROR");


{
	
	
	
		
			
			
			
			
			


