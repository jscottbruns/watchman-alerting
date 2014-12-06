/* ----------------------- Modbus includes ----------------------------------*/

#include "mbcapi.h"

unsigned char iBuff[20];
int	iRecvLen;

/***************************************************************************************/

void printRET(void)
{
	int i;
	printf("\r\n\r\n");
	for(i=0; i<iRecvLen; i++)
		printf("%x ",iBuff[i]);
	printf("\r\n\r\n");	

}

void TCPDemo()
{
    int s1; 
    

	if((s1=cMBTCPInit("10.1.115.149",0,0,3))<0)
		exit(0); 
   		                     
	/* FC16, Write iBuff values to 1st and 2nd AO of slot 1 */
	iBuff[0] = 0x03;
	iBuff[1] = 0xE8;
	iBuff[2] = 0x07;
	iBuff[3] = 0xD0;      
	if((iRecvLen=mbTCP_W_Multi_Registers(s1,0x09,0x100,2,&iBuff[0],sizeof(iBuff)))<0)
  		dumpErrMsg(iRecvLen)  ;  
    else	printRET();     
	usleep(1000);							
	/* FC15, write value to 3nd to 7th DO of slot 2, data byte = 1 */	  	
 	iBuff[0] = 0x54;
	if((iRecvLen=mbTCP_W_Multi_Coils(s1,0x09,0x0202,5,1,&iBuff[0],sizeof(iBuff)))<0)
 		dumpErrMsg(iRecvLen)  ;
	else	printRET();
    usleep(1000);		
	/* FC 0x6c, read rang code of 3nd AI/O of slot 1 */	
	if((iRecvLen=mbTCP_R_RangeCode(s1,0x09,0x102,&iBuff[0],sizeof(iBuff)))<0)
		dumpErrMsg(iRecvLen)  ;
 	else	printRET();
    usleep(1000);		
 	/* FC 0x6c, write rang code to 3nd AI/O of slot 1*/	
	if((iRecvLen=mbTCP_W_RangeCode(s1,0x09,0x102,0x33,&iBuff[0],sizeof(iBuff)))<0)
		dumpErrMsg(iRecvLen)  ;
 	else	printRET();

	cMBTCPClose(s1);

}


void RTUDemo(void)
{
	char *LinConTTYName = "/dev/ttyS6";   
	char *LinPACTTYName = "/dev/ttyS34";
	unsigned long ulBaudRate =  115200;
    unsigned char dataBits = 0;//(unsigned char)MB_PAR_NONE;
	unsigned char stopBit = 1;
	unsigned char parity = 8;
	int netid = 0x01;
	int h1 , h2;

	if((h1=cMBRTUInit(LinConTTYName,ulBaudRate,dataBits,parity,stopBit))<0)
		exit(0);	
		
	if((h2=cMBRTUInit("/dev/ttyS0",ulBaudRate,dataBits,parity,stopBit))<0)
		exit(0);
	
	mbRTU_CRC_Status = SET_OFF;    	//default SET_ON
  
  	/* FC5, turn on 4th DO of slot 2  */					
	if((iRecvLen=mbRTU_W_Coil(h1,netid,0x0203,SET_ON,&iBuff[0],sizeof(iBuff)))<0)
 		dumpErrMsg(iRecvLen)  ; 
	else	printRET();	
	
	/* FC1, read back 1st to 7th DO value of slot 2*/	  	
	if((iRecvLen=mbRTU_R_Coils(h1,netid,0x0200,8,&iBuff[0],sizeof(iBuff)))<0)
		dumpErrMsg(iRecvLen)  ;
	else	printRET();      	
 	
 	/* FC2, read 2nd and 3rd DI value of slot 7*/
	if((iRecvLen=mbRTU_R_Discrete(h2,netid,0x701,2,&iBuff[0],sizeof(iBuff)))<0)
		dumpErrMsg(iRecvLen)  ; 
	else	printRET();
	
		if((iRecvLen=mbRTU_R_Discrete(h1,netid,0x701,2,&iBuff[0],sizeof(iBuff)))<0)
		dumpErrMsg(iRecvLen)  ; 
	else	printRET();
     
	/* FC4, read 7th and 8th AI value of slot 5 */		
 	if((iRecvLen=mbRTU_R_Registers(h1,netid,0x506,2,&iBuff[0],sizeof(iBuff)))<0)
 		dumpErrMsg(iRecvLen)  ;  
 	else	printRET();
    
	/* FC6, write 0x1388(+5V) to 3rd AO of slot 3 */	
	if((iRecvLen=mbRTU_W_Register(h1,netid,0x102,0x1388,&iBuff[0],sizeof(iBuff)))<0)
		dumpErrMsg(iRecvLen)  ;
 	else	printRET();
 	
	   			
	cRTUClose(h1);
	cRTUClose(h2);
}

int main( int argc, char *argv[] )
{
    RTUDemo();
    usleep(5000);
	//TCPDemo();             

	return 0;
} 
/***************************************************************************************/


