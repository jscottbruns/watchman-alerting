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
#else  //Provided for PC (Microsoft OS)
	#ifdef __cplusplus
		#define Exp8K extern "C" __declspec(dllexport)
	#else
		#define Exp8K __declspec(dllexport)
	#endif

	#include "uart.h"
	#include "i8000.h"
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

/************************************************************************/
/* -------------------------------------------------------------------- */
WORD CALLBACK AnalogIn_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,cc[80],c1,c2;
  WORD   wAddr,wRetVal,i,j,wChannel,wCheckSum,wT,wTimeOut;
  WORD   wRecStrLen,wSlot;
  WORD   wID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];            // module ID: 0x8017/0x8018/0x8013/0x8033
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		              szCmd8K[0]='#';
		          break;	
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		              szCmd8K[0]='#';
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>7 )  return(AdChannelError);
		              szCmd8K[0]='#';
		          break;     
       case 0x5033 :				  
       case 0x8033 :  if( wChannel>3 )  return(AdChannelError);
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
   szCmd8K[3] = 'S';
   szCmd8K[4] = wSlot+'0';     
   szCmd8K[5] = 'C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]=0;

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

  if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;  
//  if ( strlen(szRec8K)!=wRecStrLen ) return ResultStrCheckError;
  /***** 
  if( (w7000[2]!=0x7013 && w7000[2]!=0x7011 && w7000[2]!=0x7018 && w7000[2]!=0x7033 ) && (strlen(szRec8K)!=wRecStrLen) )
     return ResultStrCheckError;

    //-----  Added by Shu, 7011, 7013, 7018, 7033
    // if( w7000[2]==0x7013 || w7000[2]==0x7011 || w7000[2]==0x7018 || w7000[2]==0x7033 )
    {
         if( !memcmp(szRec8K,">-0000",6) )
             return UnderInputRange;
         if( !memcmp(szRec8K,">+9999",6) )
             return ExceedInputRange;
     }
   *****/
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

/************************************************************************/
/* -------------------------------------------------------------------- */
WORD CALLBACK AnalogOutReadBack_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,cc[80],c1,c2;
  WORD   wAddr,wRetVal,i,j,wChannel,wCheckSum,wT,wTimeOut,wRecStrLen;
  WORD   wSlot;
  WORD   wID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];            // module ID: 0x8024,0x8022,0x8026
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   switch (wID)
   {
		case 0x8022:
		case 0x8026:
			if( wChannel>1 )  return(AdChannelError);
				break;			
        case 0x8024 :
		case 0x5024 :
			if( wChannel>3 )  return(AdChannelError);
				break;	
       default: return(ModuleIdError);
   }


   if (wChannel>7) return(AdChannelError);
   
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[0]='$';
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3] = 'S';
   szCmd8K[4] = wSlot+'0';     
   szCmd8K[5] = 'C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7] = '6';
   szCmd8K[8]=0;

  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  fBuf[0]=(float)0.0;
  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '!')   return ResultStrCheckError;


 // if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11;
//  if ( strlen(szRec8K)!=wRecStrLen ) return ResultStrCheckError;
  /***** 
  if( (w7000[2]!=0x7013 && w7000[2]!=0x7011 && w7000[2]!=0x7018 && w7000[2]!=0x7033 ) && (strlen(szRec8K)!=wRecStrLen) )
     return ResultStrCheckError;

    //-----  Added by Shu, 7011, 7013, 7018, 7033
    // if( w7000[2]==0x7013 || w7000[2]==0x7011 || w7000[2]==0x7018 || w7000[2]==0x7033 )
    {
         if( !memcmp(szRec8K,">-0000",6) )
             return UnderInputRange;
         if( !memcmp(szRec8K,">+9999",6) )
             return ExceedInputRange;
     }
   *****/
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




/************************************************************************/
/* -------------------------------------------------------------------- */
WORD CALLBACK AnalogInFsr_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,cc[80],c1,c2;
  WORD   wAddr,wRetVal,i,j,wChannel,wCheckSum,wT,wTimeOut;
  WORD   wRecStrLen,wSlot;
  WORD   wID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];            // module ID: 0x87017/0x87018
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>7 )  return(AdChannelError);//  11/06/2002 by sean
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
   szCmd8K[3] = 'S';
   szCmd8K[4] = wSlot+'0';     
   szCmd8K[5] = 'C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]=0;

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

  if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;
  //if ( strlen(szRec8K)!=wRecStrLen ) return ResultStrCheckError;
  /***** 
  if( (w7000[2]!=0x7013 && w7000[2]!=0x7011 && w7000[2]!=0x7018 && w7000[2]!=0x7033 ) && (strlen(szRec8K)!=wRecStrLen) )
     return ResultStrCheckError;

    //-----  Added by Shu, 7011, 7013, 7018, 7033
    // if( w7000[2]==0x7013 || w7000[2]==0x7011 || w7000[2]==0x7018 || w7000[2]==0x7033 )
    {
         if( !memcmp(szRec8K,">-0000",6) )
             return UnderInputRange;
         if( !memcmp(szRec8K,">+9999",6) )
             return ExceedInputRange;
     }
   *****/
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
WORD CALLBACK AnalogInHex_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,c1,c2;
  WORD   wAddr,wRetVal,wChannel,wCheckSum,wT,wTimeOut;
  WORD   wRecStrLen,wSlot;
  WORD   wID;
  WORD   A,B,C,D;
  int iRet=0;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];            // module ID: 0x87017/0x87018
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];  // channel number for multi-channel
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number

    //---------------------- output ---------------------------
                           // dwBuf[8]=analog input value

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>7 )  return(AdChannelError);//  11/06/2002 by sean
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
   szCmd8K[3] = 'S';
   szCmd8K[4] = wSlot+'0';     
   szCmd8K[5] = 'C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]=0;

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

  if(wCheckSum==0) wRecStrLen=5; else wRecStrLen=7;

//   if ( strlen(szRec8K)!=wRecStrLen ) return ResultStrCheckError;

   A=AsciiToHex(szRec8K[1]);
   B=AsciiToHex(szRec8K[2]);
   C=AsciiToHex(szRec8K[3]);
   D=AsciiToHex(szRec8K[4]);
   dwBuf[8]=D+C*16+B*256+A*4096;

   if(!iRet)
	return(NoError);
   else
	return iRet;
}







/************************************************************/
/*  AnalogInAll_8K											*/
/*															*/	
/*  Command:  "#AASi" + [CHK(2)]  + CR(1)					*/ 
/*  Response: ">+00.000+01.001+02.002+03.003-04.004			*/
/*				-05.005-06.006-07.007" + [CHK(2)] + CR(1)	*/
/*															*/
/*  Module: I-87013,I8017H,I-87017,I-87018					*/
/*          ADAM-5013/17/18									*/
/*															*/
/*  Ver. 501 [Apr,15,2002] by Kevin									*/
/************************************************************/
WORD CALLBACK AnalogInAll_8K(DWORD dwBuf[], float fBuf[], char szSend[], char szReceive[])
{
	char wPort,wFlag,cTemp[10];
	WORD wAddr,wRetVal,i,j,wCheckSum,wTimeOut,wT,wSlot;
	
	WORD wID;
	WORD wTotalChannel,wTotalByte;

	//---------------------- input -------------------------------------
	wPort=(char)dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
	wAddr=(WORD)dwBuf[1];       // module address: 0x00 to 0xFF
	wID=(WORD)dwBuf[2];         // module ID: 
								// 		0x8013 for I-87013
								//		0x8017 for I-8017H,I-87017
								//		0x8018 for I-87018
								//		0x5013/17/18 for ADAM-5013/17/18 	
	wCheckSum=(WORD)dwBuf[3];   // checksum: 0=disable, 1=enable
	wTimeOut=(WORD)dwBuf[4];    // TimeOut constant: normal=100
	wFlag=(char)dwBuf[6];		// flag: 0=no save, 1=save send/receive string
	wSlot=(WORD)dwBuf[7];       // slot number

	switch (wID)
	{
		case 0x8013:
			wTotalChannel=4;
			if(wCheckSum)
				wTotalByte=32;
			else 
				wTotalByte=30;
			break;    
		case 0x5017:
		case 0x8017:
		case 0x5018:
		case 0x8018:
			wTotalChannel=8;
			if(wCheckSum)
				wTotalByte=60;
			else 
				wTotalByte=58;
			break; 
		default: return(ModuleIdError);
	}

	sprintf(szCmd8K,"#%02XS%01X",wAddr,wSlot);
	wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
	if(wRetVal != NoError) return wRetVal;

	if(wFlag)    
	{
		strcpy(szSend,szCmd8K);
		strcpy(szReceive,szRec8K);
	}

	// Only check the first byte and total bytes.
	if((szRec8K[0]!='>') && (strlen(szRec8K)!=wTotalByte)) 
		return ResultStrCheckError;
	
	// Gets values of every channels
	for(i=0;i<wTotalChannel;i++)
	{ 
		//"+03.567" one float value uses 7 bytes.
		for(j=0;j<7;j++)		
			cTemp[j]=szRec8K[1+i*7+j];	
		cTemp[7]=0;
		fBuf[i]=(float)atof(cTemp);
	}
	return(NoError);	
}




/* -------------------------------------------------------------------- */
WORD CALLBACK SetAlarmConnect_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,c1,c2;
  WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wSlot,wChannel;
  WORD   wRecStrLen;
  WORD   wID;
  WORD   wConnectAlarm;  // 0:Low Alarm  1:High Alarm
  WORD   wConnectSlot,wConnectChannel;
 
  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];      // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];      // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];             // module ID: 0x7017/18
  wCheckSum=(WORD)dwBuf[3];  // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];   // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];   // channel
  wFlag=(char)dwBuf[6];      // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];      // slot number
  
  wConnectAlarm=(WORD)dwBuf[8]; // 0:connect Low Alarm, 1:connect high alarm
  wConnectSlot=(WORD)dwBuf[9];  // the desired connect slot, 
                                //  when 100 means to disconnect 
  wConnectChannel=(WORD)dwBuf[10];  // connect channel

  switch (wID)
  {
     case 0x5013 :	
     case 0x5017 :
     case 0x5018 :
     case 0x8013 :	
     case 0x8017 :
     case 0x8018 : break;
     default: return(ModuleIdError);
  }
  szCmd8K[0]='$';           // $aaSiCjAhCSkCn
  c2=(char)(wAddr&0x0f); 
  wAddr=wAddr/16;
  c1=(char)(wAddr&0x0f);
  szCmd8K[1] = HexToAscii(c1);
  szCmd8K[2] = HexToAscii(c2);
  szCmd8K[3]='S';
  szCmd8K[4]=wSlot+'0';
  szCmd8K[5]='C';
  szCmd8K[6]=wChannel+'0';
  szCmd8K[7]='A';
  szCmd8K[8]=(wConnectAlarm==0)?'L':'H';
  szCmd8K[9]='C';
  // szCmd8K[10]='S';
  // szCmd8K[11]=wConnectSlot+'0';
  // szCmd8K[12]='C';
  // szCmd8K[13]=wConnectChannel+'0';
  if( wConnectSlot==100 )
  {
     szCmd8K[10]='S';
     szCmd8K[11]='*';
     szCmd8K[12]='C';
     szCmd8K[13]='*';
  }
  else
  {
     szCmd8K[10]='S';
     szCmd8K[11]=wConnectSlot+'0';
     szCmd8K[12]='C';
     //szCmd8K[13]=wConnectChannel+'0';
	 c1=(char)(wConnectChannel&0x0f);
	 szCmd8K[13]=HexToAscii(c1);
  }
  szCmd8K[14]=0;

  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '!') return ResultStrCheckError+200;

  if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError+300;

  return(NoError);
}

/*---- Read Analog Input Alarm Mode ---------*/
/*---- for 87013, 87017, 87018  -----*/
WORD CALLBACK ReadAlarmMode_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel,wHiLo;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x87013/17/18
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   wHiLo=(WORD)dwBuf[8];    // 0: Low Alarm  1: High Alarm
   //---------------------- output ------------------------------------
   //               Alarm State:
   // dwBuf(9):0       Disable
   // dwBuf(9):1       Momentary
   // dwBuf(9):2       Latch

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
       case 0x5033 :
       case 0x8033 :  if( wChannel>3 )  return(AdChannelError);
		          break;	   
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>7 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCjAh
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='A';
   szCmd8K[8]=(wHiLo==0)?'L':'H';
   szCmd8K[9]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   switch( szRec8K[3] )
   {
     case 'D' :  dwBuf[9]=0;  break; 
     case 'M' :  dwBuf[9]=1;  break; 
     case 'L' :  dwBuf[9]=2;  break; 
     default: 
	         return ResultStrCheckError;
   }
   return(NoError);
}








/*-------- 87013/87017/87018 Set Hi/Lo Alarm Limit Value ---------*/
WORD CALLBACK SetAlarmLimitValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wHiLo,wT;
   WORD   wRecStrLen,wChannel,wSlot;
   WORD   wID;
   float  fValue;
   char   Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];      // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];      // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];             // module ID: 0x87013/17/18
   wCheckSum=(WORD)dwBuf[3];  // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];   // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5];   // Channel
   wFlag=(char)dwBuf[6];      // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];      // slot
   wHiLo=(WORD)dwBuf[8];      // 0: Low Alarm  1: High Alarm
   fValue=fBuf[0];      // Alarm Limit Value
   //---------------------- output -------------------------------------


   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
       case 0x5018 :
       case 0x8018 :  if( wChannel>6 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }
  

   //sprintf(szCmd8K, "$04S3C0ALU+001.23");

   sprintf(Buffer,"%+07.2f",fValue);   

   szCmd8K[0]='$';    // $aaSiCjAhU(data), where data is in +6.2f format
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='A';
   szCmd8K[8]=(wHiLo==0)?'L':'H';
   szCmd8K[9]='U';
   szCmd8K[10]=Buffer[0];
   szCmd8K[11]=Buffer[1];
   szCmd8K[12]=Buffer[2];
   szCmd8K[13]=Buffer[3];
   szCmd8K[14]=Buffer[4];
   szCmd8K[15]=Buffer[5];
   szCmd8K[16]=Buffer[6];
   szCmd8K[17]=0;

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
/*-------- 87013/87017/87018 Read Hi/Lo Alarm Limit Value ---------*/
WORD CALLBACK ReadAlarmLimitValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   i,wAddr,wRetVal,wCheckSum,wTimeOut,wHiLo,wT;
   WORD   wRecStrLen,wChannel,wSlot;
   WORD   wID;
   //float  fValue;
   char   Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];      // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];      // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];             // module ID: 0x87013/17/18
   wCheckSum=(WORD)dwBuf[3];  // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];   // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5];   // Channel
   wFlag=(char)dwBuf[6];      // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];      // slot
   wHiLo=(WORD)dwBuf[8];      // 0: Low Alarm  1: High Alarm
   //---------------------- output -------------------------------------
   //fValue=f7000[0];      // Alarm Limit Value

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>7 )  return(AdChannelError);
		          break;
       case 0x5033 :
       case 0x8033 :  if( wChannel>3 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCjRhU
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='R';
   szCmd8K[8]=(wHiLo==0)?'L':'H';
   szCmd8K[9]='U';
   szCmd8K[10]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=10; else wRecStrLen=12;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   for(i=0; i<7; i++)
       Buffer[i]=szRec8K[i+3];
   Buffer[7]=0;
   fBuf[0]=(float) atof(Buffer);

   return(NoError);
}










/**************************************/
/*------  Clear Latch Alarm  ---------*/
/*----  for 87013, 87017, 87018  -----*/
WORD CALLBACK ClearLatchAlarm_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel,wHiLo;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x87013/17/18
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   wHiLo=(WORD)dwBuf[8];    // 0: Low Alarm  1: High Alarm
   //---------------------- output ------------------------------------
   //               void

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
	   case 0x5017 :
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>6 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCjCh
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='C';
   szCmd8K[8]=(wHiLo==0)?'L':'H';
   szCmd8K[9]=0;

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

/*------   Set Alarm Mode    ---------*/
/*----  for 87013, 87017, 87018  -----*/
/*  Two Steps:                        */
/*     First:  Enable/Disable Alarm  $aaSiCjAhEs   */
/*     Second: Set Alarm Mode        $aaSiCjAhs    */
WORD CALLBACK SetAlarmMode_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wAddr2,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel,wHiLo,wAlarmMode;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr2=wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x87013/17/18
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   wHiLo=(WORD)dwBuf[8];    // 0: Low Alarm  1: High Alarm
   wAlarmMode=(WORD)dwBuf[9]; // 0: Disable  1:Momentary  2: Latch
   //---------------------- output ------------------------------------
   //               void

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
       case 0x5017 : 
       case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
       case 0x5018 :
       case 0x8018 :  if( wChannel>6 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }

  /* First Step: Enable/Disable Alarm  $aaSiCjAhEs   */
   szCmd8K[0]='$';   
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='A';
   szCmd8K[8]=(wHiLo==0)?'L':'H';
   szCmd8K[9]='E';
   szCmd8K[10]=(wAlarmMode==0)?'D':'E';
   szCmd8K[11]=0;

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

   if( wAlarmMode==0 )  // Set Alarm Mode to Disable
      return(NoError);

   Sleep(200);
   /***** if Setting Alarm Mode to Momentary/Latch  *****/
  /* Second Step: Set Alarm Mode  $aaSiCjAhs   */
   szCmd8K[0]='$';   
   c2=(char)(wAddr2&0x0f);
   wAddr2=wAddr2/16;
   c1=(char)(wAddr2&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='A';
   szCmd8K[8]=(wHiLo==0)?'L':'H';
   szCmd8K[9]=(wAlarmMode==1)?'M':'L';
   szCmd8K[10]=0;

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

/**************************************/
/*------  Read Alarm Status  ---------*/
/*----  for 87013, 87017, 87018  -----*/
WORD CALLBACK ReadAlarmStatus_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x87013/17/18
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   //---------------------- output ------------------------------------
   // dwBuf[8]  1: High Alarm Occur    0: Don't Occur              
   // dwBuf[9]  1: Low Alarm Occur    0: Don't Occur              

   switch (wID)
   {
       case 0x5013 :
       case 0x8013 :  if( wChannel>3 )  return(AdChannelError);
		          break;	
       case 0x5017 :
	   case 0x8017 :  if( wChannel>7 )  return(AdChannelError);
		          break;
	   case 0x5018 :
       case 0x8018 :  if( wChannel>6 )  return(AdChannelError);
		          break;
       case 0x5033 :
       case 0x8033 :  if( wChannel>3 )  return(AdChannelError);
		          break;
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCjS
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='S';
   szCmd8K[8]=0;  

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=5; else wRecStrLen=7;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   dwBuf[8]=szRec8K[3]-'0';
   dwBuf[9]=szRec8K[4]-'0';   

   return(NoError);
}

WORD CALLBACK AnalogOut_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,wCheckSum,wTimeOut,wT;
  float   fVal;
  WORD    wRecStrLen,wSlot,wChannel;
  char    OutCh[10];
  WORD    wID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];           // module ID: 0x8022,0x8024,0x8026
  wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];      // Not used if module ID is 7016/7021
                             // Channel No.(0 to 1) if module ID is 7022
                             // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];    // slot
  fVal=fBuf[0];          // analog output value
  //---------------------- output ------------------------------------
                          // void

	switch (wID)
    {
		case 0x8022:
		case 0x8026:
			if( wChannel>1 )  return(AdChannelError);
				break;
        case 0x8024 :
		case 0x5024 :
			if( wChannel>3 )  return(AdChannelError);
				break;	
       default: return(ModuleIdError);
	}

   if (wChannel>7) return(AdChannelError);

 /* if( wID==0x8024 || wID==0x5024 )     // #aaSiCj(data)
  {
     if( wChannel<0 || wChannel>3 )
     {
         return(AdChannelError);
     }*/
     sprintf(OutCh,"%+07.3f",fBuf[0]);
     //sprintf(OutCh,"%+07.3f",f7000[0]);
     //sprintf(OutCh,"%+07.3f",f7000[0]);
     szCmd8K[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd8K[1] = HexToAscii(c1);
     szCmd8K[2] = HexToAscii(c2);
     szCmd8K[3]='S';
     szCmd8K[4]=wSlot+'0';
     szCmd8K[5]='C';
     szCmd8K[6]=wChannel+'0';      // channel No.
     szCmd8K[7]= OutCh[0];
     szCmd8K[8]= OutCh[1];
     szCmd8K[9]= OutCh[2];
     szCmd8K[10]=OutCh[3];
     szCmd8K[11]=OutCh[4];
     szCmd8K[12]=OutCh[5];
	 szCmd8K[13]=OutCh[6];
     szCmd8K[14]=0;

  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '>')   return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}
/* -------------------------------------------------------------------- */


/*WORD CALLBACK AnalogOutHex_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]) 
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wChannel;
  WORD    wRecStrLen;
  char    sHex[5];
  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];           // module ID: 0x8022/0x8026  (0x8024 has no Hex type)
  wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];      // Channel No.(0 to 1)                              
  wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];    // slot
  
  if(wID==0x8022)//8022 12bit
  	sprintf(sHex,"%03X",dwBuf[7]); //analog output value(Hexadeciaml Data Foramt)
  else if(wID==0x8026)  //8026 16bit
	sprintf(sHex,"%04X",dwBuf[7]); //analog output value(Hexadeciaml Data Foramt)  	
 		
  //---------------------- output ------------------------------------
  szCmd8K[0]='#';
  c2=(char)(wAddr&0x0f);
  wAddr=wAddr/16;
  c1=(char)(wAddr&0x0f);
  szCmd8K[1] = HexToAscii(c1);
  szCmd8K[2] = HexToAscii(c2);
  szCmd8K[3]='S';
  szCmd8K[4]=wSlot+'0';
  szCmd8K[5]='C';
  szCmd8K[6]=wChannel+'0';      // channel No.
  szCmd8K[7]= sHex[0];
  szCmd8K[8]= sHex[1];
  szCmd8K[9]= sHex[2];

  if(wID==0x8022)  
     szCmd8K[10]=0;  
  else if(wID==0x8026)
  {
     szCmd8K[10]=sHex[3];     
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
// -------------------------------------------------------------------- 

 WORD CALLBACK AnalogOutFsr_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]) 
 {
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wID,wCheckSum,wTimeOut,wT,wChannel;
  WORD    wRecStrLen;
  float   fVal;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];           // module ID: 0x8022/0x8026 (0x8024 has no Fsr type)
  wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
  wChannel=(WORD)dwBuf[5];      // Channel No.(0 to 1)
  wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
//  sprintf(sHex,"%03X",dwBuf[7]);
	fVal=fBuf[0];          // analog output value 		
  //---------------------- output ------------------------------------
  if(wID==0x8022 || wID==0x8026)
  {
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3]=  '+';

   i=(WORD)(fVal/100.0);
   szCmd8K[4]=i+0x30; fVal-=i*100;

   i=(int)(fVal/10.0);
   szCmd8K[5]=i+0x30; fVal-=i*10;
	
   i=(int)fVal;
   szCmd8K[6]=i+0x30; fVal-=i;
   szCmd8K[7]='.';
   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[8]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd8K[9]=i+0x30; fVal-=(float)i;
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

	if(szRec8K[0] != '>')
    {
		return ResultStrCheckError;
    }
    if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
 }
// -------------------------------------------------------------------- 
  WORD CALLBACK AnalogOutReadBackHex_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2;
   WORD  wAddr,wRetVal,wReadType,wID,wCheckSum,wT,wTimeOut,wChannel;
   WORD  wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];  // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];        // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];          // module ID: 0x8022/0x8026   (8024 has no Hex 
   wCheckSum=(WORD)dwBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4];     // TimeOut constant: normal=100
   wReadType=(WORD)dwBuf[5];    // for 8022/8026
                          //     0: command read back ($AA6)
                          //     1: analog output of current path read back ($AA8)
   wFlag=(char)dwBuf[6];  // flag: 0=no save, 1=save send/receive string
   wChannel=(WORD)dwBuf[7];     //   Channel No.(0 to 1)
                          //   Not used,            else
   //---------------------- output ------------------------------------
                          // fBuf[9]=analog output value read back(Hex)

   switch(wID)
   {
       case 0x8022:
       case 0x8026:
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
   if(wID==0x8026 ) wRecStrLen+=1; //!01FFFF

   sscanf(szRec8K+3,"%X",&dwBuf[9]);

   return(NoError);
}*/
/* -------------------------------------------------------------------- */
/******************************************************/
/*------  Read Configuration Status for 8024 ---------*/
WORD CALLBACK ReadConfigurationStatus_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x8024,0x8022,0x8026
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   //---------------------- output ------------------------------------
   // dwBuf[8]   Output Range: 0x30, 0x31, 0x32              
   // dwBuf[9]   Slew rate             

   switch (wID)
   {
	   case 0x8022:  //2 channel 
	   case 0x8026:
			if( wChannel>1 )  return(AdChannelError);
		          break;
       case 0x8024 : //4 channel
	   case 0x5024 :
		          if( wChannel>3 )  return(AdChannelError);
		          break;	
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCjB
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='B';
   szCmd8K[8]=0;  

   if(wFlag==1)
   {
      strcpy(szSend,szCmd8K);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
   if (wFlag==1)       strcpy(szReceive,szRec8K);

   if (wRetVal != NoError) return wRetVal;

   if(szRec8K[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;

   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   dwBuf[8]=AsciiToHex(szRec8K[3])*16+AsciiToHex(szRec8K[4]);
   dwBuf[9]=(AsciiToHex(szRec8K[5])*16+AsciiToHex(szRec8K[6]))>>2;   
   
   return(NoError);
}

/******************************************************/
/*------  Setting Start-Up Value for 8024 ---------*/
WORD CALLBACK SetStartUpValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2;
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag;
   WORD   wRecStrLen,wSlot,wChannel;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x8024,0x8022,0x8026
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
      
   switch (wID)
   {
		case 0x8022:
		case 0x8026:
			if( wChannel>1 )  return(AdChannelError);
		          break;
	   case 0x8024 :  
	   case 0x5024 :
		          if( wChannel>3 )  return(AdChannelError);
		          break;	
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCj4
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='4';
   szCmd8K[8]=0;  

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
/*------  Reading Start-Up Value for 8024 ---------*/
WORD CALLBACK ReadStartUpValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,c1,c2,cc[10];
   WORD   wAddr,wRetVal,wCheckSum,wTimeOut,wT,wFlag,wRecStrLen;
   WORD   wSlot,wChannel,i;
   WORD   wID;

   //---------------------- input -------------------------------------
   wPort=(char)dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
   wAddr=(WORD)dwBuf[1];    // module address: 0x00 to 0xFF
   wID=(WORD)dwBuf[2];           // module ID: 0x8024,0x8022,0x8026
   wCheckSum=(WORD)dwBuf[3];// checksum: 0=disable, 1=enable
   wTimeOut=(WORD)dwBuf[4]; // TimeOut constant: normal=100
   wChannel=(WORD)dwBuf[5]; // channel
   wFlag=(WORD)dwBuf[6];    // flag: 0=no save, 1=save send/receive string
   wSlot=(WORD)dwBuf[7];    // slot
   //---------------------- output ------------------------------------
   // fBuf[0]: the Start-Up Value   
      
   switch (wID)
   {
	   case 0x8022:
	   case 0x8026:
			if( wChannel>1 )  return(AdChannelError);
		          break;
       case 0x8024 :  
	   case 0x5024 :
		          if( wChannel>3 )  return(AdChannelError);
		          break;	
       default: return(ModuleIdError);
   }

   szCmd8K[0]='$';    // $aaSiCj6
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1]=HexToAscii(c1);
   szCmd8K[2]=HexToAscii(c2);
   szCmd8K[3]='S';
   szCmd8K[4]=wSlot+'0';
   szCmd8K[5]='C';
   szCmd8K[6]=wChannel+'0';
   szCmd8K[7]='6';
   szCmd8K[8]=0;  

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

//   if( strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

   for(i=3; i<=8; i++)
   {
        cc[i-3]=szRec8K[i];
   }
   cc[6]=0;

   fBuf[0]=(float)atof(cc);

   return(NoError);
}

WORD CALLBACK DigitalOut_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,c1,c2,A,B,C,D;
  WORD   wAddr,wRetVal,wData,wCheckSum,wT,wTimeOut;
  WORD   wRecStrLen,wSlot;
  WORD   wID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];       // module ID: 0x8037/41/42/54/55/56/57/60/63/64/65/66/68/77
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wData=(WORD)dwBuf[5];     // 16-bit digital data to output
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  //---------------------- output ------------------------------------
                         

   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3] = 'S';
   szCmd8K[4] = wSlot+'0'; 
   szCmd8K[5] = '0';
   szCmd8K[6] = '0';
   
   switch (wID)//send:#AASiBB(data)[CHK](cr) receive:>[chk](cr) BB=00 all channels
   {
   	   	case 0x8041: //32bit DOs
   			szCmd8K[0]='@';
   			sprintf(szCmd8K+5,"%08X",dwBuf[5]);//Send:@AASiUUUUUUUU [CHK](cr) receive:>[chk](cr)
   			break;
   			   		
   		case 0x8042: //16bit DOs
   			szCmd8K[0]='@';
   			sprintf(szCmd8K+5,"%04X",dwBuf[5]);//Send:@AASiUUUU  [CHK](cr) receive:>[chk](cr)
   		break;
       case 0x5054:  //return(54);
       case 0x5055:  //return(55);
       case 0x5064:  //return(64);
       case 0x5065:  ///return(65);
       case 0x5066:  //return(66);
       case 0x5068:  //return(68);
       case 0x8054:  //return(54);
       case 0x8055:  //return(55);
       case 0x8064:  //return(64);
       case 0x8065:  ///return(65);
       case 0x8066:  //return(66);
	   case 0x8077:                     
       case 0x8068:  //return(68); 

				   wData&=0x00FF;
				   B=wData&0x0f; wData=wData/16;
                   A=wData&0x0f;
		           szCmd8K[7]=HexToAscii(A);
		           szCmd8K[8]=HexToAscii(B);
                   szCmd8K[9] = 0;
                   break;	

       case 0x5056: 
	   case 0x5057:	 
       case 0x8056: 
	   case 0x8057:	   
	   case 0x8037:
		           wData&=0xFFFF;
	               D=wData&0x0f; wData=wData/16;
	               C=wData&0x0f; wData=wData/16;
	               B=wData&0x0f; wData=wData/16;
	               A=wData&0x0f;
	               szCmd8K[7]=HexToAscii(A);
		           szCmd8K[8]=HexToAscii(B);
	               szCmd8K[9]=HexToAscii(C);
	               szCmd8K[10]=HexToAscii(D);
	               szCmd8K[11]=0;
				   //return(ModuleIdError);
		           break;

       case 0x5060:  
       case 0x8060:  
				   wData&=0x003F;
				   B=wData&0x0f; wData=wData/16;
                   A=wData&0x0f;
		           szCmd8K[7]=HexToAscii(A);
		           szCmd8K[8]=HexToAscii(B);
                   szCmd8K[9] = 0;
		           break;

       case 0x5063:  
       case 0x8063:  
				   wData&=0x000F;
				   B=wData&0x0f; wData=wData/16;
                   A=wData&0x0f;
		           szCmd8K[7]=HexToAscii(A);
		           szCmd8K[8]=HexToAscii(B);
                   szCmd8K[9] = 0;
		           break;     

       default: return(ModuleIdError);
   }


   //if (wChannel>7) return(AdChannelError);
    


  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '>')   return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}


WORD CALLBACK DigitalBitOut_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char   wPort,wFlag,c1,c2;//,A,B,C,D;
  WORD   wAddr,wRetVal,wData,wCheckSum,wT,wTimeOut,wChannel;
  WORD   wRecStrLen,wSlot;
  WORD   wID;

  //---------------------- input -------------------------------------
  wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
  wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
  wID=(WORD)dwBuf[2];       // module ID: 0x8037/41/42/54/55/56/57/60/63/64/65/66/68/77
  wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
  wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
  wData=(WORD)dwBuf[5];     // 16-bit digital data to output
  wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
  wSlot=(WORD)dwBuf[7];     // Slot Number
  wChannel=(WORD)dwBuf[8];  // channel number for multi-channel
  //---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

   if( wData>1 )
     return DataError;
   szCmd8K[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd8K[1] = HexToAscii(c1);
   szCmd8K[2] = HexToAscii(c2);
   szCmd8K[3] = 'S';
   szCmd8K[4] = wSlot+'0'; 
    if(wID==0x8041 || wID==0x8042)//send:@AASiSCnnD[CHK](cr) receive:>[chk](cr) (nn:channels,D is either 0 or 1)
    {
    	szCmd8K[0] = '@';
    	szCmd8K[5] = 'S';
    	szCmd8K[6] = 'C'; 
   		c2 = (char)(wData&0x0f);
    	szCmd8K[7] = HexToAscii((char)((wChannel&0xf0)>>4)); 
		szCmd8K[8] = HexToAscii((char)(wChannel&0x0f));
   		szCmd8K[9] = HexToAscii(c2); //it either 0 or 1
   		szCmd8K[10]= 0;
//   		sprintf(szCmd8K,"@%02XS%01dSC%02X%01d",wAddr,wSlot,wChannel,wData);
   		
   		
	}
	else//send:#AASiBB(data)[CHK](cr) receive:>[chk](cr) (BB=1B B :channels,data is either 0 or 1)
	{
   		szCmd8K[5] = '1';
   		c1 = (char)(wChannel&0x0f);
   		c2 = (char)(wData&0x0f);
   		szCmd8K[6] = HexToAscii(c1);
   		szCmd8K[7] = '0';
   		szCmd8K[8] = HexToAscii(c2); //0 or 1
   		szCmd8K[9] = 0;
	}   		

   switch (wID)
   {
		case 0x8041:
			if( wChannel>32 )  return(AdChannelError);
				   break;
		case 0x8042:
			if( wChannel>16 )  return(AdChannelError);
				   break;				    
       case 0x8054:  //return(54);
       case 0x8055:  //return(55);
       case 0x8064:  //return(64);
       case 0x8065:  ///return(65);
       case 0x8066:  //return(66);
       case 0x8068:  //return(68);
	   case 0x8077:

                   if( wChannel>7 )  return(AdChannelError);
				   break;

       case 0x8056: 
	   case 0x8057:	   
	   case 0x8037:
                   if( wChannel>15 )  return(AdChannelError);
		           break;

       case 0x8060:  
                   if( wChannel>5 )  return(AdChannelError);
		           break;
       case 0x8063:  
                   if( wChannel>3 )  return(AdChannelError);
		           break;        

       default: return(ModuleIdError);
   }


   //if (wChannel>7) return(AdChannelError);
    


  if(wFlag==1)
  {
    strcpy(szSend,szCmd8K);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
  if (wFlag==1)       strcpy(szReceive,szRec8K);

  if (wRetVal != NoError) return wRetVal;

  if(szRec8K[0] != '>')   return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}





WORD CALLBACK DigitalIn_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])
{

 char wPort,wFlag,c1,c2;
// char A,B,C,D;
 WORD wRet,wAddr,wCheckSum,wTimeOut,wT,wRecStrLen,wSlot,wID;
 //WORD mask;


 //---------------------- input ---------------------------
 wPort=(char)dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
 wAddr=(WORD)dwBuf[1];     // module address: 0x00 to 0xFF
 wID=(WORD)dwBuf[2];       // module ID: 0x8040/42/51/52/53/54/55/58/63/77
 wCheckSum=(WORD)dwBuf[3]; // checksum: 0=disable, 1=enable
 wTimeOut=(WORD)dwBuf[4];  // TimeOut constant: normal=100
 wFlag=(char)dwBuf[6];     // flag: 0=no save, 1=save send/receive string
 wSlot=(WORD)dwBuf[7];     // Slot Number
 //---------------------- output -----------------------
                        // void

 szCmd8K[0]='$';
 c2=(char)(wAddr&0x0f);
 wAddr=wAddr/16;
 c1=(char)(wAddr&0x0f);
 szCmd8K[1] = HexToAscii(c1);
 szCmd8K[2] = HexToAscii(c2);
 szCmd8K[3] = 'S';
 szCmd8K[4] = wSlot+'0';
 szCmd8K[5] = '6';
 szCmd8K[6] = 0;

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
 if(wID==0x8040 || wID==0x8042)
		wRecStrLen=11;
	else 
		wRecStrLen=9;
		
 if(wCheckSum)
		wRecStrLen+=2;
 	
 if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;
 switch(wID)
   {
   	case 0x8040:
   		sscanf(szRec8K+3,"%08X",&dwBuf[5]);//send:$AASi6[CHK](cr) receive:!AADDDDDDDD[CHK](cr) 
   		break;
	case 0x8042:
		sscanf(szRec8K+7,"%04X",&dwBuf[5]);//send:$AASi6[CHK](cr) receive: !AAUUUUDDDD  UUUU:DO readback,DDDD:DI value
		break;
	case 0x5051: 
	case 0x5053:
	case 0x8051://16 DI    
	case 0x8053:

		sscanf(szRec8K+3,"%04X",&dwBuf[5]);//send:$AASi6[CHK](cr)  receive:!AA(data)(data)00[CHK](cr)
		break;

    case 0x5052: //8 DI
    case 0x8052:
    case 0x8058:

		 sscanf(szRec8K+3,"%02X",&dwBuf[5]);//send:$AASi6[CHK](cr)  receive:!AA(data)00[CHK](cr)
		 break;
    case 0x5054: // 8 DI
	case 0x5055:
    case 0x8054:
	case 0x8055:
	case 0x8077:
		 sscanf(szRec8K+5,"%02X",&dwBuf[5]);//send:$AASi6[CHK](cr)  receive:!AA(data1)(data2)00[CHK](cr)  data1:DO readback,data2:DI
		 break;

	case 0x5063: // 4 DI  
	case 0x8063:

		 sscanf(szRec8K+6,"%01X",&dwBuf[5]);//send:$AASi6[CHK](cr)  receive:!AA(data)00[CHK](cr)
		 break;
		  
    default:return(ModuleIdError);
   }
 return(wRet);

}


WORD CALLBACK DigitalOutReadBack_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @18
{

	char wPort,wFlag,c1,c2;
//	char A,B,C,D;
	WORD wRet,wAddr,wCheckSum,wTimeOut,wT,wRecStrLen,wSlot,wID;
//	WORD mask;



 //---------------------- input ---------------------------
 	wPort=(char)dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
 	wAddr=(WORD)dwBuf[1];         // module address: 0x00 to 0xFF
 	wID=(WORD)dwBuf[2];           // module ID: 0x8037/41/42/54/55/56/57/60/63/64/65/66/68
 	wCheckSum=(WORD)dwBuf[3];     // checksum: 0=disable, 1=enable
 	wTimeOut=(WORD)dwBuf[4];      // TimeOut constant: normal=100
 	wFlag=(char)dwBuf[6];   // flag: 0=no save, 1=save send/receive string
 	wSlot=(WORD)dwBuf[7];     // Slot Number
 //---------------------- output -----------------------
                        // void

	szCmd8K[0]='$';
 	c2=(char)(wAddr&0x0f);
 	wAddr=wAddr/16;
 	c1=(char)(wAddr&0x0f);
 	szCmd8K[1] = HexToAscii(c1);
 	szCmd8K[2] = HexToAscii(c2);
 	szCmd8K[3] = 'S';
 	szCmd8K[4] = wSlot+'0';
 	szCmd8K[5] = '6';
 	szCmd8K[6] = 0;

 	if(wFlag==1)
   	{
    	strcpy(szSend,szCmd8K);
      	szReceive[0]=0;
   	}
 	wRet=Send_Receive_Cmd(wPort, szCmd8K, szRec8K, wTimeOut, wCheckSum,&wT);
 	if(wFlag==1)      strcpy(szReceive,szRec8K);
 		dwBuf[5]=0;
 	if(wRet != NoError) return wRet;
 	if(szRec8K[0] != '!') 
 		return ResultStrCheckError;
 	if(wID==0x8041 || wID==0x8042)
 	 	wRecStrLen=11; 
 	else 
		wRecStrLen=9;

	 if(wCheckSum)
		wRecStrLen+=2;
		
 	if(strlen(szRec8K) != wRecStrLen) return ResultStrCheckError;
 	switch(wID)
   	{
   		case 0x8041:
   			sscanf(szRec8K+3,"%08X",&dwBuf[5]);//send:$AASi6[CHK](cr) receive:!AAUUUUUUUU 
		    break;
   		case 0x8042:
   			sscanf(szRec8K+3,"%04X",&dwBuf[5]);//send:$AASi6[CHK](cr) receive: !AAUUUUDDDD  UUUU:DO readback,DDDD:DI value
		    break;
		case 0x5056:
		case 0x5057:
		case 0x8056://16DO readback //send:$AASi6[CHK](cr)  receive:!AA(data)(data)00[CHK](cr)  (data) is 4 bytes
		case 0x8057:
		case 0x8037:
			sscanf(szRec8K+3,"%04X",&dwBuf[5]);
		    break;

 	    case 0x5054:
		case 0x5055:
		case 0x5064:
		case 0x5065:
		case 0x5066:
		case 0x5068:
    	case 0x8054: //8DO readback
		case 0x8055: //send:$AASi6[CHK](cr)  receive:!AA(data1)(data2)00[CHK](cr)  data1:DO readback,data2:DI
		case 0x8064: //send:$AASi6[CHK](cr)  receive:!AA(data)00[CHK](cr)  
		case 0x8065:
		case 0x8066:
		case 0x8068:
		case 0x8077:
			sscanf(szRec8K+3,"%02X",&dwBuf[5]);		   
			break;
    	case 0x5060: //6DO readback //send:$AASi6[CHK](cr)  receive:!AA(data)00[CHK](cr) 
    	case 0x8060:

			sscanf(szRec8K+3,"%02X",&dwBuf[5]);
			break;
    	case 0x5063: //4DO readback //send:$AASi6[CHK](cr)  receive:!AA(data)00[CHK](cr) 
		case 0x8063:
			sscanf(szRec8K+3,"%01X",&dwBuf[5]);
			break;
		  
    	default:return(ModuleIdError);
	}
 	return(wRet);

}

unsigned Crc16Table[256];

void MakeCRC16Table(void)
{ 
	int i,j,k,crc;
	static fmaketable=0;

  if(fmaketable) return;
  for(i=0;i<256;i++){
      k=i<<8;
      crc=0;
      for(j=0;j<8;j++){
	 if((crc^k)&0x8000)
	    crc=(crc<<1)^0x1021;
	 else
	    crc<<=1;
	 k<<=1;
      }
      Crc16Table[i]=(unsigned)crc;
/*
      Print("0x%04X, ",crc);
      if((i&7)==7) Print("\n\r");
*/
  }
  fmaketable=1;
}

unsigned GetCRC16(unsigned char *data,int length)
{ unsigned crc=0;
  MakeCRC16Table();
  while(length--){
     crc=(unsigned)(Crc16Table[crc>>8] ^ (crc<<8) ^ (*data++));
  }
  return crc;
}


int CALLBACK  ShiftLeft(int data, int length)
{ 
  data<<=length;
  return (data);
}

int CALLBACK ShiftRight(int data, int length)
{ 
  data>>=length;
  return (data);
}

unsigned CALLBACK  IntToUnsig(int data)
{ 
  unsigned crc=0;
  crc=(unsigned)data;  
  return (crc);
}




