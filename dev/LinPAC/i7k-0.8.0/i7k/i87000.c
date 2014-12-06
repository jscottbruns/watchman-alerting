//#include "stdafx.h"
//#include <windows.h>
#include <stdlib.h>
//#include <conio.h>
#include <string.h>
//#include <process.h>
//#include <stdio.h>

/*
#ifdef _WIN32_WCE  //Provided for WinCon
	#include "UARTCE.h"
	#include "I7000CE.h"
	#define strnset _strnset

#else  //Provided for PC (Microsoft OS)
	#ifdef __cplusplus
		#define Exp87K extern "C" __declspec(dllexport)
	#else
		#define Exp87K __declspec(dllexport)
	#endif

	#include "uart.h"
	#include "i87000.h"
#endif
*/

#include "msw.h"

// ASCII characters that receive special processing
/*
#define ASCII_BEL  0x07
#define ASCII_BS   0x08
#define ASCII_TAB  0x09
#define ASCII_LF   0x0A
#define ASCII_CR   0x0D
#define ASCII_XON  0x11
#define ASCII_XOFF 0x13
*/

static char szCmd8K[80], szRec8K[80];

extern char HexToAscii(WORD hex);
extern char AsciiToHex(char cASCII);






WORD CALLBACK AnalogIn_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,cc[80],c1,c2;
  WORD   wAddr,wRetVal,i,j,wChannel,wCheckSum,wT,wTimeOut;
  WORD   wSlot;
  DWORD  dwID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  dwID=dwBuf[2];            // module ID: 0x87013/17/18/33
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   switch (dwID)
   {

       case 0x87013 :  if( wChannel>3 )  return(AdChannelError);
		              szCmd8K[0]='#';
		          break;	
	   
       case 0x87017 :  if( wChannel>7 )  return(AdChannelError);
		              szCmd8K[0]='#';
		          break;
	   
       case 0x87018 :  if( wChannel>7 )  return(AdChannelError);//  11/06/2002 by sean
		              szCmd8K[0]='#';
		          break;     
       
       case 0x87033 :  if( wChannel>3 )  return(AdChannelError);
		              szCmd8K[0]='$';
		          break;
       default: return(ModuleIdError);
   }


   if (wChannel>7) return(AdChannelError);
   
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=wChannel+'0';
   szCmd8K[4]=0;

  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  fBuf[0]=(float)0.0;
  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '>')   return ResultStrCheckError;
  if(strlen(szRec8K)==6 || (strlen(szRec8K)==8 && wCheckSum))	//Total length is 6 bytes and 8byte checksum//2003/9/24 by sean
  {
	if( !memcmp(szRec8K,">-0000",6) )  return UnderInputRange; //-0000如果經atof轉換會變為0，使用者則無法分判是Under range  or 0
	{
		fBuf[0]=(float)atof("-9999");
        return UnderInputRange;
	}
	if( !memcmp(szRec8K,">+9999",6) )  return ExceedInputRange;
	{
		fBuf[0]=(float)atof("+9999");
        return ExceedInputRange;
	}
  }


     i=1; j=0;
     while (szRec8K[i])
     {
        cc[j++]=szRec8K[i++];
     }
     cc[j]=0;

     if( wCheckSum )  // checksum enable
        cc[j-2]=0;

     fBuf[0]=(float)atof(cc);

     return(NoError);
}



/* -------------------------------------------------------------------- */
WORD CALLBACK AnalogInHex_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,c1,c2;
  WORD   wAddr,wRetVal,wChannel,wCheckSum,wT,wTimeOut;
//  WORD   wRecStrLen;
  WORD   wSlot;
  DWORD  dwID;
  WORD   A,B,C,D;
  int iRet=0;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  dwID=dwBuf[2];            // module ID: 0x87017/0x87018
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   switch (dwID)
   {
       
       case 0x87013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   
       case 0x87017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   
       case 0x87018 :  if( wChannel>7 )  return(AdChannelError); //  11/06/2002 by sean
		          break;
       default: return(ModuleIdError);
   }


   if (wChannel>7) return(AdChannelError);
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=wChannel+'0';
   szCmd8K[4]=0;

  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  fBuf[0]=(float)0.0;
  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '>')   return ResultStrCheckError;
  if(strlen(szRec8K)==6 || (strlen(szRec8K)==8 && wCheckSum))	//Total length is 6 bytes and 8byte checksum//2003/9/24 by sean
  {
		if( !memcmp(szRec8K,">-0000",6) ) //  return UnderInputRange;
		{
			strcpy(szRec8K,">8000");       //UnderRange or the cable broke			
			iRet=UnderInputRange;
		}
    	if( !memcmp(szRec8K,">+9999",6) ) //  return ExceedInputRange;
		{
			strcpy(szRec8K,">7FFF"); //OverRange or the cable broke
			iRet=ExceedInputRange;
		}
  }

//  if(wCheckSum==0) wRecStrLen=5; else wRecStrLen=7;

//   if ( strlen(szRec8K)!=wRecStrLen ) return ResultStrCheckError;

   A=AsciiToHex(szRec8K[1]);
   B=AsciiToHex(szRec8K[2]);
   C=AsciiToHex(szRec8K[3]);
   D=AsciiToHex(szRec8K[4]);
   dwBuf[7]=D+C*16+B*256+A*4096;

   if(!iRet)
	return(NoError);
   else
	return iRet;
}



/************************************************************/
/*  AnalogInAll_87K											*/
/*															*/	
/*  Command:  "#AA" + [CHK(2)] + CR(1) 						*/ 
/*  Response: ">+00.000+01.001+02.002+03.003-04.004			*/
/*				-05.005-06.006-07.007" + [CHK(2)] + CR(1)	*/
/*															*/
/*  Module: I-87013,I8017H,I-87017,I-87018					*/
/*          ADAM-5013/17/18									*/
/*															*/
/*  [Apr,15,2002] by Kevin									*/
/************************************************************/
WORD CALLBACK AnalogInAll_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{

	char   wPort,wFlag,cTemp[80],wMode;
	WORD   wAddr,wRetVal,i,j,wCheckSum,wTimeOut,wT,k;
//	WORD   wTotalChannel;
	WORD   wTotalByte;
	int	   iRet;
	DWORD  dwID;
	char *endptr;
	

	//---------------------- input -------------------------------------
	wPort=(char)dwBuf[0];			// RS-232 port number: 1/2/3/4/..../255
	wAddr=(WORD)dwBuf[1];			// module address: 0x00 to 0xFF
	dwID=dwBuf[2];					// module ID: 0x87013/87017/87018
	wCheckSum=(WORD)dwBuf[3];       // checksum: 0=disable, 1=enable
	wTimeOut=(WORD)dwBuf[4];        // TimeOut constant: normal=100
	wFlag=(char)dwBuf[6];			// flag: 0=no save, 1=save send/receive string
	wMode=(char)dwBuf[7];			// 0: Engineer 1:% FSR 2:Hex

	switch (dwID)
	{
		case 0x87013:
/*			wTotalChannel=4;
			if(wCheckSum)
				wTotalByte=31;
			else 
				wTotalByte=29;*/
			break;    
		case 0x87017:
		case 0x87018:
/*			wTotalChannel=8;
			if(wCheckSum)
				wTotalByte=59;
			else 
				wTotalByte=57;*/
			break; 
		default: return(ModuleIdError);
	}

	sprintf(szCmd8K,"#%02X",wAddr);

	wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);

	if(wFlag)
	{
		strcpy(szSend,szCmd8K);
		strcpy(szReceive,szRec8K);
	}
	if (wRetVal != NoError) return wRetVal;

/*	// Only check the first byte and total bytes.
	if((szRec8K[0]!='>') && (strlen(szRec8K)!=wTotalByte)) 
		return ResultStrCheckError;
	
	for(i=0;i<wTotalChannel;i++)
	{
		//"+03.567" one float value uses 7 bytes.
		for(j=0;j<7;j++)		
			cTemp[j]=szRec8K[1+i*7+j];	
		cTemp[7]=0;
		fBuf[i]=(float)atof(cTemp);
	}
  return(NoError);*/
  switch (dwID)
  {
     
     case 0x87013 :
	 case 0x87017 :
	 case 0x87018 :

		i=1; //szRec[0] is '>'
		k=0;
		j=0;
		if (!wCheckSum) 
	 		wTotalByte=strlen(szRec8K);
		else
			wTotalByte=strlen(szRec8K)-2;
		iRet=NoError;
		switch(wMode)
		{
			case 0: //Engineer
			case 1: //Fsr
			while(i<=wTotalByte) //57 (without '\0')
			{
				if(szRec8K[i]=='+' || szRec8K[i]=='-' || i==wTotalByte)
				{
					if(i!=1 || i==wTotalByte)
					{
						cTemp[j]='\0';
						if( !memcmp(cTemp,"-0000",5))
						{
							strcpy(cTemp,"-9999");
							iRet=UnderInputRange;
						}
						else if( !memcmp(cTemp,"+9999",5) )
							iRet=ExceedInputRange;

						fBuf[k]=(float)atof(cTemp);
						if(i==wTotalByte)
							break;
						k++;
						j=0;
					}				  
					cTemp[j++]=szRec8K[i];
				}
				else			
					cTemp[j++]=szRec8K[i];			
				i++;
			}
			break;
			case 2: //Hex
			k=8; //Start from wBuf[8]
			while(i<=wTotalByte) // (without '\0')
			{
				if(szRec8K[i]=='+' || szRec8K[i]=='-')
				{				
					for(j=0;j<5;j++)
						cTemp[j]=szRec8K[i++];
					if(!memcmp(cTemp,"-0000",5))
					{
						strcpy(cTemp,"8000"); //UnderRange or the cable broke
						iRet=UnderInputRange;
					}
					else if(!memcmp(cTemp,"+9999",5))
					{
						strcpy(cTemp,"7FFF"); //OverRange or the cable broke
						iRet=ExceedInputRange;
					}
				}
				else			
				{
					for(j=0;j<4;j++)
						cTemp[j]=szRec8K[i++];					
				}				
				cTemp[j]='\0';
//				fBuf[k]=(float)atof(cTemp);
				dwBuf[k++]=(DWORD)strtol(cTemp,&endptr,16); //16 means 0~9,A~F
			}
			break;
		}
		break;
  }
	return iRet;
}
/*WORD CALLBACK AnalogInAllHex_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
	WORD wRetVal;
/*	char   wPort,wFlag,cTemp[80];
	WORD   wAddr,wRetVal,i,j,wCheckSum,wTimeOut,wT;
	WORD   wTotalChannel,wTotalByte;
	DWORD  dwID;*
//---------------------- input -------------------------------------
	//wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
	//wAddr=dwBuf[1];          // module address: 0x00 to 0xFF
	//wID=dwBuf[2];            // module ID: 0x87017/87018/87013
	//wCheckSum=dwBuf[3];      // checksum: 0=disable, 1=enable
	//wTimeOut=dwBuf[4];       // TimeOut constant: normal=100
	//wFlag=(char)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
	//---------------------- input -------------------------------------
						// Hex mode
						// dwBuf[8] = Hex input value of channel_0
						// dwBuf[9] = Hex input value of channel_1
						//..........................................
						// dwBuf[15] = Hex input value of channel_7
	dwBuf[7]=2;			// 0: Engineer 1:% FSR 2:Hex

	wRetVal=AnalogInAll_87K(dwBuf,fBuf,szSend,szReceive);

	return wRetVal;
}
WORD CALLBACK AnalogInAllFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
	WORD wRetVal;
/*char wPort,cc[80],c1,c2;
WORD wAddr,wRetVal,i,t,j,wID,wCheckSum,wTimeOut,wT,k;
WORD wRecStrLen;
float  fBuf2[10];*

//---------------------- input -------------------------------------
//wPort=(char)wBuf[0];    // RS-232 port number: 1/2/3/4/..../255
//wAddr=wBuf[1];          // module address: 0x00 to 0xFF
//wID=wBuf[2];            // module ID: 0x87017/87018/87013
//wCheckSum=wBuf[3];      // checksum: 0=disable, 1=enable
//wTimeOut=wBuf[4];       // TimeOut constant: normal=100
//wFlag=(char)wBuf[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
						// Engineer and FSR mode
                        // fBuf[0]=analog input value of channel_0
                        // fBuf[1]=analog input value of channel_1
                        // ........................................
                        // fBuf[7]=analog input value of channel_7
	dwBuf[7]=1;		    // 0: Engineer 1:% FSR 2:Hex

	wRetVal=AnalogInAll_87K(dwBuf,fBuf,szSend,szReceive);

	return wRetVal;
}*/


/************************************************************************/
/* -------------------------------------------------------------------- */
WORD CALLBACK AnalogInFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,cc[80],c1,c2;
  WORD   wAddr,wRetVal,i,j,wChannel,wCheckSum,wT,wTimeOut;
  WORD   wRecStrLen,wSlot;
  DWORD  dwID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  dwID=dwBuf[2];            // module ID: 0x87017/0x87018/87013
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   switch (dwID)
   {
       
       case 0x87013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   
       case 0x87017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   
       case 0x87018 :  if( wChannel>7 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }


   if (wChannel>7) return(AdChannelError);
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=wChannel+'0';
   szCmd8K[4]=0;

  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  fBuf[0]=(float)0.0;
  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '>')   return ResultStrCheckError;
  if(strlen(szRec8K)==6 || (strlen(szRec8K)==8 && wCheckSum))	//Total length is 6 bytes and 8byte checksum//2003/9/24 by sean
  {
         if( !memcmp(szRec8K,">-0000",6) )//-0000如果經atof轉換會變為0，使用者則無法分判是Under range  or 0
		 {
			 fBuf[0]=(float)atof("-9999");
             return UnderInputRange;
		 }
         if( !memcmp(szRec8K,">+9999",6) )
		 {
			 fBuf[0]=(float)atof("+9999");
             return ExceedInputRange;
		 }
  }

  if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;

     i=1; j=0;
     while (szRec8K[i])
     {
        cc[j++]=szRec8K[i++];
     }
     cc[j]=0;

     if( wCheckSum )  // checksum enable
        cc[j-2]=0;

     fBuf[0]=(float)atof(cc);

     return(NoError);
}

WORD CALLBACK AnalogOut_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wCheckSum,wTimeOut,wT;
  int 	  wChannel;
  float   fVal;
  WORD    wRecStrLen;
  char    OutCh[10];
  DWORD   dwID;


  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
  dwID=dwBuf[2];           // module ID: 0x87016/21/22/24
  wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];      // Not used if module ID is 7016/7021
                             // Channel No.(0 to 1) if module ID is 7022
                             // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
  fVal=fBuf[0];          // analog output value
  //---------------------- output ------------------------------------
                          // void
  if( dwID==0x87016 )
  {
     sprintf(OutCh,"%+07.3f",fBuf[0]);

     szCmd8K[0]='$';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd8K[1]=HexToAscii(c1);
     szCmd8K[2]=HexToAscii(c2);
     szCmd8K[3]='7';

     szCmd8K[4]=OutCh[0];
     szCmd8K[5]=OutCh[1];
     szCmd8K[6]=OutCh[2];
     szCmd8K[7]=OutCh[3];
     szCmd8K[8]=OutCh[4];
     szCmd8K[9]=OutCh[5];
     szCmd8K[10]=OutCh[6];
     szCmd8K[11]=0;
  }
  else if(dwID==0x87021)
  {
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);

   szCmd8K[5]='.';
   szCmd8K[9]=0;


   i=(WORD)(fVal/10.0);
   szCmd8K[3]=i+0x30; fVal-=i*10;

   i=(int)fVal;
   szCmd8K[4]=i+0x30; fVal-=i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[6]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[7]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[8]=i+0x30;
  }
  else if( dwID==0x87022 )
  {
     if( wChannel<0 || wChannel>2 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%06.3f",fBuf[0]);

     szCmd8K[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd8K[1] = HexToAscii(c1);
     szCmd8K[2] = HexToAscii(c2);
     szCmd8K[3]=(char)(wChannel+0x30);  // channel No.

     szCmd8K[4]=OutCh[0];
     szCmd8K[5]=OutCh[1];
     szCmd8K[6]=OutCh[2];
     szCmd8K[7]=OutCh[3];
     szCmd8K[8]=OutCh[4];
     szCmd8K[9]=OutCh[5];
     szCmd8K[10]=0;
   }
  else if( dwID==0x87024 )
  {
     if( wChannel<0 || wChannel>3 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%+07.3f",fBuf[0]);
         // sprintf(OutCh,"%+07.1f",fValue);


     szCmd8K[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd8K[1] = HexToAscii(c1);
     szCmd8K[2] = HexToAscii(c2);
     szCmd8K[3]=(char)(wChannel+0x30);  // channel No.

     szCmd8K[4]=OutCh[0];
     szCmd8K[5]=OutCh[1];
     szCmd8K[6]=OutCh[2];
     szCmd8K[7]=OutCh[3];
     szCmd8K[8]=OutCh[4];
     szCmd8K[9]=OutCh[5];
     szCmd8K[10]=OutCh[6];
     szCmd8K[11]=0;
  }


  else if( dwID==0x87026 )
  {
     if( wChannel<0 || wChannel>2 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%06.3f",fBuf[0]);

     szCmd8K[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd8K[1] = HexToAscii(c1);
     szCmd8K[2] = HexToAscii(c2);
     szCmd8K[3]=(char)(wChannel+0x30);  // channel No.

     szCmd8K[4]=OutCh[0];
     szCmd8K[5]=OutCh[1];
     szCmd8K[6]=OutCh[2];
     szCmd8K[7]=OutCh[3];
     szCmd8K[8]=OutCh[4];
     szCmd8K[9]=OutCh[5];
     szCmd8K[10]=0;
   }



  else
     return(ModuleIdError);


   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

  if( dwID==0x87016 )
  {
     if(szRec8K[0] != '!')
     {
        return ResultStrCheckError;
     }
     if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;
  }
  else    // 7021, 7024
  {
     if(szRec8K[0] != '>')
     {
        return ResultStrCheckError;
     }
     if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  }

  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}

WORD CALLBACK AnalogOutHex_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]) 
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,wCheckSum,wTimeOut,wT,wChannel;
  WORD    wRecStrLen;
  char    sHex[5];
  DWORD   dwID;
   
  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  dwID=dwBuf[2];            // module ID: 0x87022/0x87026  (0x87024 has no Hex type)
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // Channel No.(0 to 1)                              
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  
  if(dwID==0x87022)//87022 12bit
  	sprintf(sHex,"%03X",dwBuf[7]); //analog output value(Hexadeciaml Data Foramt)
  else if(dwID==0x87026)  //87026 16bit
	sprintf(sHex,"%04X",dwBuf[7]); //analog output value(Hexadeciaml Data Foramt)  	
 		
  //---------------------- output ------------------------------------
  if(dwID==0x87022)
  {
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=(char)(wChannel+0x30);  // channel No.
   szCmd8K[4]=  sHex[0];
   szCmd8K[5]=  sHex[1];	
   szCmd8K[6]=  sHex[2];
   szCmd8K[7]=0;
  }
  else if(dwID==0x87026)
  {
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=(char)(wChannel+0x30);  // channel No.
   szCmd8K[4]=  sHex[0];   
   szCmd8K[5]=  sHex[1];	
   szCmd8K[6]=  sHex[2];   
   szCmd8K[7]=  sHex[3];  
   szCmd8K[8]=  0;
   
  }
  else
     return(ModuleIdError);


   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

	if(szRec8K[0] != '>')
    {
		return ResultStrCheckError;
    }
    if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}
/* -------------------------------------------------------------------- */

 WORD CALLBACK AnalogOutFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]) 
 {
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wCheckSum,wTimeOut,wT,wChannel;
  WORD    wRecStrLen;
  float   fVal;
  DWORD   dwID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  dwID=dwBuf[2];            // module ID: 0x87022/0x87026 (0x87024 has no Fsr type)
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // Channel No.(0 to 1)
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  
  fVal=fBuf[0];             // analog output value 		
  //---------------------- output ------------------------------------
  if(dwID==0x87022 || dwID==0x87026)
  {
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=(char)(wChannel+0x30);  // channel No.
   szCmd8K[4]=  '+';

   i=(WORD)(fVal/100.0);
   szCmd8K[5]=i+0x30; fVal-=i*100;

   i=(int)(fVal/10.0);
   szCmd8K[6]=i+0x30; fVal-=i*10;
	
   i=(int)fVal;
   szCmd8K[7]=i+0x30; fVal-=i;
   szCmd8K[8]='.';
   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[9]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[10]=i+0x30; fVal-=(float)i;
	szCmd8K[11]=0;
  }
  else
     return(ModuleIdError);


   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

	if(szRec8K[0] != '>')
    {
		return ResultStrCheckError;
    }
    if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
 }
/* -------------------------------------------------------------------- */
 WORD CALLBACK AnalogOutReadBackHex_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2;
   WORD  wAddr,wRetVal,wReadType,wCheckSum,wT,wTimeOut;
   int   wChannel;
   WORD  wRecStrLen;
   DWORD dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];        // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];        // module address: 0x00 to 0xFF
   dwID=dwBuf[2];               // module ID: 0x87022/0x87026   (87024 has no Hex mode) 
   wCheckSum=(WORD)dwBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];     // TimeOut constant: normal=100
   wReadType=(WORD)dwBuf[5];    // for 8022/8026
                                //     0: command read back ($AA6)
                                //     1: analog output of current path read back ($AA8)
   wFlag=(char)dwBuf[6];        // flag: 0=no save, 1=save send/receive string
   wChannel=(WORD)dwBuf[7];     //   Channel No.(0 to 1)
                                //   Not used,            else
   //---------------------- output ------------------------------------
							// dwBuf[9]=analog output value read back(Hex)

   switch(dwID)
   {
       case 0x87022:
       case 0x87026:
             if( wChannel<0 || wChannel>2 )
                 return(CmdError);
             szCmd8K[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd8K[1] = HexToAscii(c1);
             szCmd8K[2] = HexToAscii(c2);

             if (wReadType==0) szCmd8K[3]='6';
             else if (wReadType==1) szCmd8K[3]='8';
             else return(CmdError);

             szCmd8K[4]=(char)(wChannel+'0');  // channel No.
             szCmd8K[5]=0;
             break;

       default: return(ModuleIdError);
   }


   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=6; else wRecStrLen=8; //!01FFF
   if(dwID==0x87026 ) wRecStrLen+=1; //!01FFFF

//   if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError; //01/08/2003 by sean

   sscanf(szRec8K+3,"%X",&dwBuf[9]);

   return(NoError);
}
/* -------------------------------------------------------------------- */
/******************************************************/
/*------  Read Configuration Status for 87024 ---------*/
WORD CALLBACK ReadConfigurationStatus_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel;
   DWORD  dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x87024
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   //---------------------- output ------------------------------------
   // dwBuf[8]   Output Range: 0x30, 0x31, 0x32              
   // dwBuf[9]   Slew rate             

   switch (dwID)
   {
       case 0x87024 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aa2
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='2';
   szCmd8K[4]=0;  

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   dwBuf[8]=AsciiToHex(szRec8K[3])*16+AsciiToHex(szRec8K[4]);
   dwBuf[9]=(AsciiToHex(szRec8K[7])*16+AsciiToHex(szRec8K[8]))>>2;   
   
   return(NoError);
}







/******************************************************/
/*------  Setting Start-Up Value for 87024 ---------*/
WORD CALLBACK SetStartUpValue_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wChannel;
   DWORD  dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x87024
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
//   wSlot=(WORD)dwBuf[7];    // slot
      
   switch (dwID)
   {
       case 0x87024 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aa4n
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='4';
   szCmd8K[4]=wChannel+'0';
   szCmd8K[5]=0;  

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}




/******************************************************/
/*------  Reading Start-Up Value for 87024 ---------*/
WORD CALLBACK ReadStartUpValue_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2,cc[10];
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wSlot,wChannel,i;
//   WORD   wRecStrLen;
   DWORD  dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x87024
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   //---------------------- output ------------------------------------
   // fBuf[0]: the Start-Up Value   
      
   switch (dwID)
   {
       case 0x87024 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aa7n
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='7';
   szCmd8K[4]=wChannel+'0';
   szCmd8K[5]=0;  

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

//   if(wCheckSum==0) wRecStrLen=10; else wRecStrLen=12;

//   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   for(i=3; i<=8; i++)
   {
        cc[i-3]=szRec8K[i];
   }
   cc[6]=0;

   fBuf[0]=(float)atof(cc);

   return(NoError);
}








WORD CALLBACK DigitalOut_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2,A,B,C,D;
   WORD  wAddr,wRetVal,wData,wCheckSum,wTimeOut,wT;//,wAddr2
   WORD  wRecStrLen;
   DWORD dwID;
   //char  szCmd2[80];

   //---------------------- input ---------------------------
   wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x87054/55/56/57/60/63/64/65/66/68
   wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
   wData=(WORD)dwBuf[5];         // 16-bit digital data to output
   wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output -----------------------
   // void

   //@AA(Data)[CHK](cr)
   szCmd8K[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);

   switch (dwID)
   {
       case 0x87054:  //return(54);
       case 0x87055:  //return(55);
       case 0x87064:  //return(64);
       case 0x87065:  ///return(65);
       case 0x87066:  //return(66);
       case 0x87069:
       case 0x87068:  //return(68);

				   wData&=0x00FF;
				   B=wData&0x0f; wData=wData/16;
                   A=wData&0x0f;
		           szCmd8K[3]=HexToAscii(A);
		           szCmd8K[4]=HexToAscii(B);
                   szCmd8K[5] = 0;
				   break;

       case 0x87056: 	  //16 channels 
	   case 0x87057:	  //16 channels 
		           wData&=0xFFFF;
	               D=wData&0x0f; wData=wData/16;
	               C=wData&0x0f; wData=wData/16;
	               B=wData&0x0f; wData=wData/16;
	               A=wData&0x0f;
	               szCmd8K[3]=HexToAscii(A);
		           szCmd8K[4]=HexToAscii(B);
	               szCmd8K[5]=HexToAscii(C);
	               szCmd8K[6]=HexToAscii(D);
	               szCmd8K[7]=0;
		           break;

       case 0x87060:  //6 channels
				   wData&=0x003F;
				   B=wData&0x0f; wData=wData/16;
                   A=wData&0x0f;
		           szCmd8K[3]=HexToAscii(A);
		           szCmd8K[4]=HexToAscii(B);
                   szCmd8K[5] = 0;
		           break;
       case 0x87063: //4 channels          // 01/06/2003 by sean              
				   wData&=0x000F;
				   B=wData&0x0f; 
//				   wData=wData/16;
//                 A=wData&0x0f;
                   szCmd8K[3]=HexToAscii(B);
                   szCmd8K[4]=0;
/*		           szCmd8K[3]=HexToAscii(A);
		           szCmd8K[4]=HexToAscii(B);
                   szCmd8K[5] = 0;*/
		           break;        

       default: return(ModuleIdError);
   }



   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '>')   return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
   if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}



WORD CALLBACK DigitalBitOut_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2; //,A,B,C,D,i;
   WORD  wAddr,wRetVal,wChannel,wData,wCheckSum,wTimeOut,wT;//,wAddr2
   WORD  wRecStrLen;
   DWORD dwID;
   //char  szCmd2[80];

   //---------------------- input ---------------------------
   wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x87054/55/56/57/60/63/64/65/66/68
   wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
   //wData=(WORD)dwBuf[5];         // don't care
   wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
   wChannel=(WORD)dwBuf[7];      // which channel to output
   wData=(WORD)dwBuf[8];         // 16-bit digital data to output
   //---------------------- output -----------------------
                        // void

   if( wData>1 )
     return DataError;

   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);

   switch (dwID)
   {
       case 0x87054:  //return(54);
       case 0x87055:  //return(55);
       case 0x87064:  //return(64);
       case 0x87065:  ///return(65);
       case 0x87066:  //return(66);
       case 0x87069:
       case 0x87068:  //return(68);

                   if( wChannel>7 )  return(AdChannelError);
				   szCmd8K[3] = 'A';
                   szCmd8K[4]=wChannel+'0';
                   szCmd8K[5]='0';
                   szCmd8K[6]=wData+'0';
				   szCmd8K[7]=0;
				   break;

       case 0x87056: 
	   case 0x87057:	   
                   if( wChannel>15 )  return(AdChannelError);
                   if( wChannel>7 )
				   {
					   szCmd8K[3]='B';
					   wChannel-=8;
				   }
                   else    
				   {
                       szCmd8K[3]='A';
				   }
                   szCmd8K[4]=wChannel+'0';
                   szCmd8K[5]='0';
                   szCmd8K[6]=wData+'0';
                   szCmd8K[7]=0;

		           break;

       case 0x87060:  
                   if( wChannel>5 )  return(AdChannelError);
				   szCmd8K[3] = 'A';
                   szCmd8K[4]=wChannel+'0';
                   szCmd8K[5]='0';
                   szCmd8K[6]=wData+'0';
				   szCmd8K[7]=0;
		           break;
       case 0x87063:  
                   if( wChannel>3 )  return(AdChannelError);
				   szCmd8K[3] = 'A';
                   szCmd8K[4]=wChannel+'0';
                   szCmd8K[5]='0';
                   szCmd8K[6]=wData+'0';
				   szCmd8K[7]=0;
		           break;        

       default: return(ModuleIdError);
   }



   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '>')   return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
   if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}





WORD CALLBACK DigitalIn_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{

 char wPort,wFlag,c1,c2,A,B,C,D;
 WORD wRet,mask,wAddr,wCheckSum,wTimeOut,wT,wRecStrLen;
 DWORD  dwID;


 //---------------------- input ---------------------------
 wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
 wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
 dwID=dwBuf[2];           // module ID: 0x87054/55/56/57/60/63/64/65/66/68
 wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
 wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
 wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
 //---------------------- output -----------------------
                        // void

 szCmd8K[0]='$';
 c2=(char)(wAddr&0x0f);
 wAddr=wAddr/16;
 c1=(char)(wAddr&0x0f);
 szCmd8K[1] = HexToAscii(c1);
 szCmd8K[2] = HexToAscii(c2);
 szCmd8K[3]='6';
 szCmd8K[4]=0;

 if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
 wRet=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
 if(wFlag==1)      strcpy(szReceive,szRec8K);
 dwBuf[5]=0;
 if(wRet != NoError) return wRet;
 if(szRec8K[0] != '!') return ResultStrCheckError;
 if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;
 if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;
 A=AsciiToHex(szRec8K[1]);
 B=AsciiToHex(szRec8K[2]);
 C=AsciiToHex(szRec8K[3]);
 D=AsciiToHex(szRec8K[4]);
 switch(dwID)
   {
	case 0x87051:
	case 0x87053:
		         wT=D+C*16+B*256+A*4096;	
				 mask=0xffff;
		         break;
    case 0x87052:
    case 0x87058:
		         wT=B+A*16;
				 mask=0xff;
				 break;
    case 0x87054:
	case 0x87055:
		         wT=D+C*16;
				 mask=0xff;
				 break;

	case 0x87063:
                 wT=D+C*16;
				 mask=0xf;
				 break;
		  
    default:return(ModuleIdError);
   }

 dwBuf[5]=wT&mask;
 return(wRet);

}

WORD CALLBACK DigitalOutReadBack_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{

 char wPort,wFlag,c1,c2,A,B,C,D;
 WORD wRet,mask,wAddr,wCheckSum,wTimeOut,wT,wRecStrLen;
 DWORD  dwID;


 //---------------------- input ---------------------------
 wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
 wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
 dwID=dwBuf[2];           // module ID: 0x87054/55/56/57/60/63/64/65/66/68
 wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
 wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
 wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
 //---------------------- output -----------------------
                        // void

 szCmd8K[0]='$';
 c2=(char)(wAddr&0x0f);
 wAddr=wAddr/16;
 c1=(char)(wAddr&0x0f);
 szCmd8K[1] = HexToAscii(c1);
 szCmd8K[2] = HexToAscii(c2);
 szCmd8K[3]='6';
 szCmd8K[4]=0;

 if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
 wRet=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
 if(wFlag==1)      strcpy(szReceive,szRec8K);
 dwBuf[5]=0;
 if(wRet != NoError) return wRet;
 if(szRec8K[0] != '!') return ResultStrCheckError;
 if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;
 if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;
 A=AsciiToHex(szRec8K[1]);
 B=AsciiToHex(szRec8K[2]);
 C=AsciiToHex(szRec8K[3]);
 D=AsciiToHex(szRec8K[4]);
 switch(dwID)
   {
	case 0x87056:
	case 0x87057:
		         wT=D+C*16+B*256+A*4096;	
				 mask=0xffff;
		         break;
    /*case 0x87052:
		         wT=B+A*16;
				 mask=0xff;
				 break;*/
    case 0x87054:
	case 0x87055:
    case 0x87064:
	case 0x87065:
    case 0x87066:
    case 0x87069:
	case 0x87068:
		         wT=B+A*16;
				 mask=0xff;
				 break;

    case 0x87060:
		         wT=B+A*16;
				 mask=0x3f;
				 break;

	case 0x87063:
                 wT=B+A*16;
				 mask=0xf;
				 break;
		  
    default:return(ModuleIdError);
   }

 dwBuf[5]=wT&mask;
 return(wRet);

}
/* -------------------------------------------------------------------- */

WORD CALLBACK AnalogOutReadBack_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,cc[80],c1,c2;
   WORD  wAddr,wRetVal,i,j,wReadType,wCheckSum,wT,wTimeOut;
   int	 wChannel;
//   WORD  wRecStrLen;
   DWORD dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];  // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];        // module address: 0x00 to 0xFF
   dwID=dwBuf[2];          // module ID: 0x87016/87021/87024/87022/87026
   wCheckSum=(WORD)dwBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];     // TimeOut constant: normal=100
   wReadType=(WORD)dwBuf[5];    // for 7016 :  Don't Care
                          // for 7021/7022/7024
                          //     0: command read back ($AA6)
                          //     1: analog output of current path read back ($AA8)
   wFlag=(char)dwBuf[6];  // flag: 0=no save, 1=save send/receive string
   wChannel=(WORD)dwBuf[7];     //   Channel No.(0 to 3), if wID is 7024
                          //   Not used,            else
   //---------------------- output ------------------------------------
                          // fBuf[0]=analog output value read back

   switch(dwID)
   {
       case 0x87016:
             szCmd8K[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd8K[1] = HexToAscii(c1);
             szCmd8K[2] = HexToAscii(c2);
             szCmd8K[3]='6';
             szCmd8K[4]=0;
             break;
       case 0x87021:
             if (wReadType==0) szCmd8K[3]='6';
             else if (wReadType==1) szCmd8K[3]='8';
             else return(CmdError);

             szCmd8K[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd8K[1] = HexToAscii(c1);
             szCmd8K[2] = HexToAscii(c2);
             szCmd8K[4]=0;
             break;
       case 0x87022:
	   case 0x87026:
             if( wChannel<0 || wChannel>2 )
                 return(CmdError);
             szCmd8K[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd8K[1] = HexToAscii(c1);
             szCmd8K[2] = HexToAscii(c2);

             if (wReadType==0) szCmd8K[3]='6';
             else if (wReadType==1) szCmd8K[3]='8';
             else return(CmdError);

             szCmd8K[4]=(char)(wChannel+'0');  // channel No.
             szCmd8K[5]=0;
             break;
       case 0x87024:
             if( wChannel<0 || wChannel>3 )
                 return(CmdError);
             szCmd8K[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd8K[1] = HexToAscii(c1);
             szCmd8K[2] = HexToAscii(c2);

             if (wReadType==0) szCmd8K[3]='6';
             else if (wReadType==1) szCmd8K[3]='8';
             else return(CmdError);

             szCmd8K[4]=(char)(wChannel+'0');  // channel N0.
             szCmd8K[5]=0;
             break;
       default: return(ModuleIdError);
   }


   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   fBuf[0]=(float)0.0;
   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

//   if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11;
//   if( dwID==0x87016 || dwID==0x87024 ) wRecStrLen+=1;
//   if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   i=3; j=0;
   while (szRec8K[i])
   {
      cc[j++]=szRec8K[i++];
   }
   cc[j]=0;

   if( wCheckSum )  // checksum enable
      cc[j-2]=0;
   fBuf[0]=(float)atof(cc);

   return(NoError);
}

/* -------------------------------------------------------------------- */
 WORD CALLBACK AnalogOutReadBackFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   WORD  wRecStrLen,wCheckSum;
   WORD wRet;
   char wFlag;
   DWORD dwID;

   //---------------------- input -------------------------------------
   //dwBuf[0];               // RS-232 port number: 1/2/3/4/..../255
   //dwBuf[1];               // module address: 0x00 to 0xFF
   dwID=dwBuf[2];            // module ID: 87022/87026
   wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
   //dwBuf[4];               // TimeOut constant: normal=100
   //dwBuf[5];               // for 7021/7022/7024
                             //     0: command read back ($AA6)
                             //     1: analog output of current path read back ($AA8)
   wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
   //dwBuf[7];               //   Channel No.(0 to 3)
   //---------------------- output ------------------------------------
                          // fBuf[0]=analog output value read back

   switch(dwID)
   {                 
       case 0x87022:
	   case 0x87026: 
             break;
       default: return(ModuleIdError);
   }

	if(!wFlag)
		wFlag=1;
	wRet=AnalogOutReadBack_87K(dwBuf,fBuf,szSend,szReceive);
	if(!wRet)
	{
		if(wCheckSum==0) wRecStrLen=10; else wRecStrLen=12;
	    if(strlen(szReceive) != wRecStrLen) return ResultStrCheckError;
		if(!dwBuf[6]) //if flag=0; set szReceive=null
			//strnset(szReceive,0x00,strlen(szReceive));
		return(NoError);
	}
	else
		return wRet;
 }
/* -------------------------------------------------------------------- */

WORD CALLBACK DigitalInCounterRead_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wCheckSum,wTimeOut,wChannel,wT; //,wCounter;
   //WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen;
   char cc[10];
   DWORD dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x7050/52/53/60/63/65/41/44
   wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5];      // Channel No.
   wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   // w7000[7]= DigitalIn Counter Value

   switch (dwID)
   {
       case 0x87063 : if( wChannel>4 )
                        return InvalidateChannelNo;
                     break;
       case 0x87052 : 
       case 0x87054 :
	   case 0x87055 :
                     if( wChannel>8 )
                        return InvalidateChannelNo;
                     break;
       case 0x87051 : 
	   case 0x87053 :
                     if( wChannel>16 )
                        return InvalidateChannelNo;
                     break;

       default: return(ModuleIdError);
   }

   //***   #AAN  read DigitalIn Channel N's Counter Value
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3] = HexToAscii(wChannel);
   szCmd8K[4] = 0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   cc[0]=szRec8K[3];  cc[1]=szRec8K[4];  cc[2]=szRec8K[5];
   cc[3]=szRec8K[6];  cc[4]=szRec8K[7];  cc[5]=0;
   dwBuf[7]=(WORD)(atoi(cc));

   return(NoError);
}



WORD CALLBACK ClearDigitalInCounter_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wCheckSum,wTimeOut,wT,wChannel; //,wCounter
   //WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen;
   DWORD dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x7050/52/53/60/63/65/41/44
   wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5];      // Channel No.
   wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (dwID)
   {
       case 0x87063 : if( wChannel>4 )
                        return InvalidateChannelNo;
                     break;
       case 0x87052 : 
       case 0x87054 :
	   case 0x87055 :
                     if( wChannel>8 )
                        return InvalidateChannelNo;
                     break;
       case 0x87051 : 
	   case 0x87053 :
                     if( wChannel>16 )
                        return InvalidateChannelNo;
                     break;

       default: return(ModuleIdError);
   }


   //***   $AACN   Clear DigitalIn Channel N's Counter Value
   szCmd8K[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]='C';
   szCmd8K[4]=HexToAscii(wChannel);//wChannel+'0';
   szCmd8K[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


WORD CALLBACK DigitalInLatch_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wCheckSum,wTimeOut,wT,wLatchStatus; //,wCounter
   WORD A,B,C,D; //,E,F,G,H;
   WORD wRecStrLen,mask;
   DWORD dwID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x7050/52/53/60/63/65/41/44
   wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
   wLatchStatus=(WORD)dwBuf[5];  // 0: select to latch low
                           // 1: select to latch high
   wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   //  w7000[7]: the latched data

   //***   $AALS  S:0 latch low  S:1 latch high
   szCmd8K[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]='L';
   if( wLatchStatus==0 )
      szCmd8K[4]='0';
   else
      szCmd8K[4]='1';
   szCmd8K[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;


   A=AsciiToHex(szRec8K[1]);
   B=AsciiToHex(szRec8K[2]);
   C=AsciiToHex(szRec8K[3]);
   D=AsciiToHex(szRec8K[4]);

switch(dwID)
   {
	case 0x87051:
	case 0x87053:
		         wT=D+C*16+B*256+A*4096;	
				 mask=0xffff;
		         break;
    case 0x87052:
		         wT=B+A*16;
				 mask=0xff;
				 break;
    case 0x87054:
	case 0x87055:
		         wT=D+C*16;
				 mask=0xff;
				 break;

	case 0x87063:
                 wT=D+C*16;
				 mask=0xf;
				 break;
		  
    default:return(ModuleIdError);
   }



   dwBuf[7]=wT&mask;

   return(NoError);

}



WORD CALLBACK ClearDigitalInLatch_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wCheckSum,wTimeOut,wT; //,wLatchStatus,wCounter;
   //WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen; //,mask;
   DWORD dwID;
   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
   dwID=dwBuf[2];           // module ID: 0x7050/52/53/60/63/65/41/44
   wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
   // wLatchStatus=w7000[5];  // 0: select to latch low
                              // 1: select to latch high
   wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
   
   //---   $AAC  ---
   szCmd8K[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]='C';
   szCmd8K[4]= 0 ;
   
   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec8K);

   if(wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}
