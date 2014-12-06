//#include "stdafx.h"
//#include <windows.h>
#include <stdlib.h>
//#include <conio.h>
#include <string.h>
//#include <process.h>
#include <stdio.h>
#include "msw.h"
#include "i7000.h"

// ASCII characters that receive special processing
//#define ASCII_BEL  0x07
//#define ASCII_BS   0x08
//#define ASCII_TAB  0x09
//#define ASCII_LF   0x0A
//#define ASCII_CR   0x0D
#define ASCII_XON  0x11
#define ASCII_XOFF 0x13

static char szCmd[100], szRec[100]; // for I-7000 series, 100 bytes is enough

/* -------------------------------------------------------------------- */
#ifndef _WIN32_WCE  //The subroutine is removed to I7000CE.cpp on WinCE system
/*	int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
	{
		switch(fdwReason)
        {
			case DLL_PROCESS_ATTACH :
                break;
			case DLL_PROCESS_DETACH :
                break;
        }

		return TRUE ;
	}*/
#endif
/* -------------------------------------------------------------------- */

short CALLBACK Short_Sub_2(short nA, short nB)
{
return(nA-nB);
}

/* -------------------------------------------------------------------- */

float CALLBACK Float_Sub_2(float fA, float fB)
{
return(fA-fB);
}

DWORD CALLBACK FloatToHex(float fv)
{
 DWORD lv;
	lv = (DWORD)*(DWORD *) & fv;
	return lv;
}


/* -------------------------------------------------------------------- */

WORD CALLBACK Get_Dll_Version(void)  //.def @4
{
	return(0x0514);
}
/* -------------------------------------------------------------------- */
WORD CALLBACK Test(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  int i;

  strcpy(szSend,"Command String Send to I7000");
  strcpy(szReceive,"Command String Receive From I7000");
  for (i=0; i<4; i++)
  {
    wBuf[i]++;
    fBuf[i]++;
  }
  return(NoError);
}

/* -------------------------------------------------------------------- */

char HexToAscii(WORD hex)
{
if (hex<10)return((char)(hex+'0'));
else return((char)(hex-10+'A'));
}

/* -------------------------------------------------------------------- */

char AsciiToHex(char cASCII)
{
if (cASCII > '9') return(cASCII-'A'+10);
else return(cASCII-'0');
}

/*-------- Host is OK --------*/
 WORD CALLBACK HostIsOK(WORD wBuf[], float fBuf[],
                                       char szSend[], char szReceive[])
{
   char   wPort,wFlag;     //,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut;//,wT;
   //WORD   wRecStrLen;
   //WORD   wSafeValue,wPowerOnValue,

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4..../255
   wAddr=wBuf[1];         // not used
   wID=wBuf[2];           // not used
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5];   don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string


   szCmd[0]='~';           // ~**
   szCmd[1] = '*';
   szCmd[2] = '*';
   szCmd[3]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Cmd(wPort, szCmd,wCheckSum);
  // if(wFlag==1) strcpy(szReceive,szRec);
   
   return wRetVal;
}

/*--- Read the module reset status, $AA5 --------*/
 WORD CALLBACK ReadModuleResetStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   //WORD   wPowerOnValue,wSafeValue;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           //  all module
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5];   don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string


   szCmd[0]='$';    // $AA5
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='5';
   if ( wID == 0x7014 )
           szCmd[3] = 'R';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)  strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError ;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError ;

   wBuf[5]=szRec[3]-'0';   // !AAS, //szRec[3],reset status, 1=the module is been reset,0=it isn't been reset
   return(NoError);                     //  S=0: has not been reset
                                        //  S=1: has been reset
}

/*--- To setup host watchdog, ~AA3ETT --------*/
 WORD CALLBACK ToSetupHostWatchdog(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   WORD   wEnable,wTimeInterval;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           //  all module
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wEnable=wBuf[5];       // 0: disable   1: Enable
   if ( wTimeOut == 0 )     // if TimeOut value = 0 then disable the Host Watchdog
           wEnable = 0;
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wTimeInterval=wBuf[7]; // time interval

   szCmd[0]='~';           // ~AA3ETT
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='3';
   szCmd[4]=wEnable+'0';
   c2=(char)(wTimeInterval&0x0f);
   wTimeInterval=wTimeInterval/16;
   c1=(char)(wTimeInterval&0x0f);
   szCmd[5] = HexToAscii(c1);
   szCmd[6] = HexToAscii(c2);
   szCmd[7]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);                     //    SS=00:  module is OK
                                        //    SS=04:  host watchdog failure
}

/*--- To read host watchdog setup value, ~AA2 --------*/
 WORD CALLBACK ToReadHostWatchdog(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   WORD   wTimeInterval;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           //  all module
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //----------------------- output ----------------------------------
   //wBuf[5];  0: disable   1: enable
   //wBuf[7];  time interval

   szCmd[0]='~';    // ~AA2
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='2';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=6; else wRecStrLen=8;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';   // 0: Host Watchdog disable,  1: enable
   wTimeInterval=AsciiToHex( szRec[4] )*16 + AsciiToHex( szRec[5] );//timeout value in Hex format,each count is 0.1 second
   wBuf[7]=wTimeInterval;

   return(NoError);
}



/*--- Read the module status about the host watchdog, ~AA0 --------*/
 WORD CALLBACK ReadModuleHostWatchdogStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   //WORD   wPowerOnValue,wSafeValue;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // all module
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5];   don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string


   szCmd[0]='~';    // ~AA0
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='0';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=5; else wRecStrLen=7;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[4]-'0';   // !AASS,module status,00=host watchdog timeout status is chear,04=host watchdog timeout status is set,reset by the command ~AA1()ResetModuleHostWatchdogStatus()
   return(NoError);                     //    SS=00:  module is OK
                                        //    SS=04:  host watchdog failure
}

/*--- Reset the module status about the host watchdog, ~AA1 --------*/
 WORD CALLBACK ResetModuleHostWatchdogStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   //WORD   wPowerOnValue,wSafeValue;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // all module
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5];   don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   szCmd[0]='~';    // ~AA1
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='1';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*--- Set Safe Value for 42/43/44/50/55/60/63/65/66/67 ---*/
 WORD CALLBACK SetSafeValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2,A,B,C,D;
   WORD  wAddr,wRetVal,wData,wID,wCheckSum,wTimeOut,wT;
   WORD  wRecStrLen;
   //char  OutCh[10];
   //float fValue;

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/63/60/65/66/67/42/43/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wData=wBuf[5];         // 16-bit digital data to output
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output -----------------------
                        // void

   switch (wID)
   {
       case 0x7055 : //@AADD, 8 bit DO , Add by 20080830
       case 0x7050 : //@AADD, 8 bit DO
                     if( wData>0x00ff )
                        return DataError;
                     wID=0;
                     break;
       case 0x7060 : //@AADD, 4 bit DO
                     if( wData>0x000f )
                        return DataError;
                     wID=2;
                     break;
	     case 0x7063 : //@AADD, 3 bit DO (0~7)
                     if( wData>0x0007 )
                        return DataError;
                     wID=2;
                     break;	
       case 0x7065 : //@AADD, 5 bit DO
                     if( wData>0x001f )
                        return DataError;
                     wID=0;
                     break;
       case 0x7066 : //@AADD, 7 bit DO
                     if( wData>0x007f )
                        return DataError;
                     wID=0;
                     break;
       case 0x7067 : //@AADD, 7 bit DO
                     if( wData>0x007f )
                        return DataError;
                     wID=0;
                     break;
       case 0x7044 : //@AADD, 8 bit DO
                     if( wData>0x00ff )
                        return DataError;
                     wID=0;
                     break;
       case 0x7042 : //@AADDDD, 13 bit DO
                     if( wData>0x1fff )
                        return DataError;
                     wID=1;
                     break;
       case 0x7043 : //@AADDDD, 16 bit DO
       case 0x7045 : 
                     if( wData>0xffff )
                        return DataError;
                     wID=1;
                     break;
       default: return(ModuleIdError);
   }

   if(wID == 0) //@AADD   7050, 7065, 7066 ,7067, 7044 ,7055
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

      B=wData&0x0f; wData=wData/16;
      A=wData&0x0f;

      szCmd[3]=HexToAscii(A);
      szCmd[4]=HexToAscii(B);
      szCmd[5]=0;
   }
   else if( wID==2 )   // @AAD  7060
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

     A=wData&0x0f;

      szCmd[3]=HexToAscii(A);
      szCmd[4]=0;
   }
   else
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

      D=wData&0x0f; wData=wData/16;
      C=wData&0x0f; wData=wData/16;
      B=wData&0x0f; wData=wData/16;
      A=wData&0x0f;

      szCmd[3]=HexToAscii(A);
      szCmd[4]=HexToAscii(B);
      szCmd[5]=HexToAscii(C);
      szCmd[6]=HexToAscii(D);
      szCmd[7]=0;

   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   // Firstly, Digital Output
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);
   
   if(wRetVal != NoError) return wRetVal;

   //if( wID==0 )  // Digit I/O module
   //{
      if(szRec[0] != '>')
      {
         return ResultStrCheckError;
      }
   //}

   //if( wID==0 )  // Digit I/O module
   //{
      if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
   //}

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   /************** Set Safe Value **************/ //~AA5V v=s(set current output as safe value)
   szCmd[0]='~';
   wAddr=wBuf[1];
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='5';
   szCmd[4]='S';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   //if( wID==0 )  // Digit I/O module
   {
      if(szRec[0] != '!')
      {
         return ResultStrCheckError;
      }
   }

   //if( wID==0 )  // Digit I/O module
   {
      if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;
   }

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*--- Set PowerOn Value for 42/43/44/50/55/60/63/65/66/67 ---*/
 WORD CALLBACK SetPowerOnValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2,A,B,C,D;
   WORD  wAddr,wRetVal,wData,wID,wCheckSum,wTimeOut,wT;
   WORD  wRecStrLen;
   //char  OutCh[10];
   //float fValue;

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/60/63/65/66/67/42/43/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wData=wBuf[5];         // 16-bit digital data to output
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output -----------------------
                        // void

   switch (wID)
   {
       case 0x7055 : //@AADD, 8 bit DO , add by 20080830
       case 0x7050 : //@AADD, 8 bit DO
                     if( wData>0x00ff )
                        return DataError;
                     wID=0;
                     break;
       case 0x7060 : //@AADD, 4 bit DO
                     if( wData>0x000f )
                        return DataError;
                     wID=2;
                     break;
	     case 0x7063 : //@AADD, 3 bit DO
                     if( wData>0x0007 )
                        return DataError;
                     wID=2;
                     break;
       case 0x7065 : //@AADD, 5 bit DO
                     if( wData>0x001f )
                        return DataError;
                     wID=0;
                     break;
	     case 0x7066 : //@AADD, 7 bit DO
                     if( wData>0x007f )
                        return DataError;
                     wID=0;
                     break;
       case 0x7067 : //@AADD, 7 bit DO
                     if( wData>0x007f )
                        return DataError;
                     wID=0;
                     break;
       case 0x7044 : //@AADD, 8 bit DO
                     if( wData>0x00ff )
                        return DataError;
                     wID=0;
                     break;
       case 0x7042 : //@AADDDD, 13 bit DO
                     if( wData>0x1fff )
                        return DataError;
                     wID=1;
                     break;
       case 0x7043 : //@AADDDD, 16 bit DO
       case 0x7045 :
                     if( wData>0xffff )
                        return DataError;
                     wID=1;
                     break;
       default: return(ModuleIdError);
   }

   if(wID == 0) //@AADD   7050 7065, 7066,7067, 7044, 7055
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

      B=wData&0x0f; wData=wData/16;
      A=wData&0x0f;

      szCmd[3]=HexToAscii(A);
      szCmd[4]=HexToAscii(B);
      szCmd[5]=0;
   }
   else if( wID==2 )  // @AAD  7060
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      A=wData&0x0f;
      szCmd[3]=HexToAscii(A);
      szCmd[4]=0;

   }
   else
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

      D=wData&0x0f; wData=wData/16;
      C=wData&0x0f; wData=wData/16;
      B=wData&0x0f; wData=wData/16;
      A=wData&0x0f;

      szCmd[3]=HexToAscii(A);
      szCmd[4]=HexToAscii(B);
      szCmd[5]=HexToAscii(C);
      szCmd[6]=HexToAscii(D);
      szCmd[7]=0;

   }

  /*****
   else  // #AA00.000    7021
   {
      szCmd[0]='#';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

      szCmd[5]='.';
      szCmd[9]=0;

      fVal=fBuf[0];
      i=(WORD)(fVal/10.0);
      szCmd[3]=i+0x30; fVal-=i*10;

      i=(int)fVal;
      szCmd[4]=i+0x30; fVal-=i;

      fVal *= (float)10.0;
      i=(int)fVal;
      szCmd[6]=i+0x30; fVal-=(float)i;

      fVal *= (float)10.0;
      i=(int)fVal;
      szCmd[7]=i+0x30; fVal-=(float)i;

      fVal *= (float)10.0;
      i=(int)fVal;
      szCmd[8]=i+0x30;
   }
   else if(wID==4)  // #AAN+00.000    7024
   {
     if( wData<0 || wData>3 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%+07.3f",fBuf[0]);
         // sprintf(OutCh,"%+07.1f",fValue);


     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=OutCh[6];
     szCmd[11]=0;
   }
  ****/

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   // Firstly, Digital Output
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   //if( wID==0 )  // Digit I/O module
   {
      if(szRec[0] != '>')
      {
         return ResultStrCheckError;
      }
   }

   //if( wID==0 )  // Digit I/O module
   {
      if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
   }

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   /************** Set PowerOn Value **************///~AA5V V=p(set current output as power on value)
   szCmd[0]='~';
   wAddr=wBuf[1];
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='5';
   szCmd[4]='P';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)  strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   //if( wID==0 )  // Digit I/O module
   //{
      if(szRec[0] != '!')
      {
         return ResultStrCheckError;
      }
//   }

   //if( wID==0 )  // Digit I/O module
  // {
      if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;
   //}

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}
/*********************************************************/
/*--- Set Safe Value for 7021/22/24, ~AA5/~AA5N ---*/
 WORD CALLBACK SetSafeValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wID,wCheckSum,wTimeOut,wT,wChannel;
  float   fVal;
  WORD    wRecStrLen;
  char    OutCh[10];

  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7021/22/24
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Not used if module ID is 7021
  						  // Channel No.(0 to 1) if module ID is 7022
                          // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
  fVal=fBuf[0];          // DA output value: from 0.0 to 10.0
  //---------------------- output ------------------------------------
                          // void

  if(wID==0x7021)  //#AA(DATA)  Send:#03+050.00  Receive:>
  {
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);

   szCmd[5]='.';
   szCmd[9]=0;


   i=(WORD)(fVal/10.0);
   szCmd[3]=i+0x30; fVal-=i*10;

   i=(int)fVal;
   szCmd[4]=i+0x30; fVal-=i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[6]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[7]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[8]=i+0x30;
  }
  else if( wID==0x7022 )//#AAN(DATA)  Send:#01105.000 Receive:>
  {
//     if( wChannel<0 || wChannel>1 )
     if( wChannel>1 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%06.3f",fBuf[0]);
         // sprintf(OutCh,"%+07.1f",fValue);

     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=0;
  }
  else if( wID==0x7024 )//#AAN(DATA)  Send:#021+05.000 Receive:>
  {
//     if( wChannel<0 || wChannel>3 )
     if( wChannel>3 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%+07.3f",fBuf[0]);
         // sprintf(OutCh,"%+07.1f",fValue);

     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=OutCh[6];
     szCmd[11]=0;
  }
  else
     return(ModuleIdError);


  if(wFlag==1)
  {
    strcpy(szSend,szCmd);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
  if(wFlag==1) strcpy(szReceive,szRec);

  if(wRetVal != NoError) return wRetVal;

  if(szRec[0] != '>') return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;

  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  /*********** secondly, set the safe value ***********/
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  if( wID==0x7024 || wID==0x7022 ) //I7024/I7022 ~AA5N
  {
     szCmd[0]='~';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3] = '5';
     szCmd[4] = wChannel+'0';
     szCmd[5]=0;

  }
  else    //I-7021/21P  only ~AA5
  {
     szCmd[0]='~';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3] = '5';
     szCmd[4]=0;
  }

  if(wFlag==1)
  {
    strcpy(szSend,szCmd);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
  if(wFlag==1) strcpy(szReceive,szRec);

  if(wRetVal != NoError) return wRetVal;

  if(szRec[0] != '!') return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}

/*********************************************************/
/*--- Read Safe Value for 7021/22/24, ~AA4/~AA4N ---*/
 WORD CALLBACK ReadSafeValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wID,wCheckSum,wTimeOut,wT,wChannel;
  //float   fVal;
  WORD    wRecStrLen;
  char    OutCh[10];

  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7021/22/24
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Not used if module ID is 7021
  			  // Channel No.(0 to 1) if module ID is 7022
                          // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

  //---------------------- output ------------------------------------
  //  fVal=fBuf[0];      // DA output value

  if(wID==0x7021)
  {
    szCmd[0]='~';
    c2=(char)(wAddr&0x0f);
    wAddr=wAddr/16;
    c1=(char)(wAddr&0x0f);
    szCmd[1] = HexToAscii(c1);
    szCmd[2] = HexToAscii(c2);
    szCmd[3]='4';
    szCmd[4]=0;
  }
  else if( wID==0x7022 )
  {
//     if( wChannel<0 || wChannel>1 )
     if( wChannel>1 )
     {
         return(AdChannelError);
     }
     szCmd[0]='~';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]='4';
     szCmd[4]=(char)(wChannel+0x30);  // channel No.
     szCmd[5]=0;
  }
  else if( wID==0x7024 )
  {
     if( wChannel>3 )
     {
         return(AdChannelError);
     }
     szCmd[0]='~';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]='4';
     szCmd[4]=(char)(wChannel+0x30);  // channel No.
     szCmd[5]=0;
  }
  else
     return(ModuleIdError);

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1) strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11; //for 7021/21P/22 !0105.000(no +/-) 2/6/2003 by sean
   if( wID==0x7024 ) //for 7024 !01+00.000 (+/-)
      wRecStrLen++;

   /*if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;*/

   if( wID==0x7024 )
   {
      for(i=0; i<7; i++)
          OutCh[i]=szRec[i+3];
      OutCh[7]=0;
   }
   else
   {
      for(i=0; i<6; i++)
          OutCh[i]=szRec[i+3];
      OutCh[6]=0;
   }
   fBuf[0]=(float) atof(OutCh);

   return(NoError);
}

/*--- Set PowerOn Value for 7021/22/24, $AA4/$AA4N ---*/
 WORD CALLBACK SetPowerOnValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wID,wCheckSum,wTimeOut,wT,wChannel;
  float   fVal;
  WORD    wRecStrLen;
  char    OutCh[10];

  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7021/0x7022/0x7024
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Not used if module ID is 7021
			  // Channel No.(0 to 1) if module ID is 7022
                          // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
  fVal=fBuf[0];          // DA output value: from 0.0 to 10.0
  //---------------------- output ------------------------------------
                          // void

  if(wID==0x7021)
  {
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);

   szCmd[5]='.';
   szCmd[9]=0;


   i=(WORD)(fVal/10.0);
   szCmd[3]=i+0x30; fVal-=i*10;

   i=(int)fVal;
   szCmd[4]=i+0x30; fVal-=i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[6]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[7]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[8]=i+0x30;
  }
  else if( wID==0x7022 )
  {
//     if( wChannel<0 || wChannel>1 )
     if( wChannel>1 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%06.3f",fBuf[0]);

     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=0;
  }
  else if( wID==0x7024 )
  {
//     if( wChannel<0 || wChannel>3 )
     if( wChannel>3 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%+07.3f",fBuf[0]);
         // sprintf(OutCh,"%+07.1f",fValue);


     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=OutCh[6];
     szCmd[11]=0;
  }
  else
     return(ModuleIdError);


  if(wFlag==1)
  {
    strcpy(szSend,szCmd);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
  if(wFlag==1)      strcpy(szReceive,szRec);

  if(wRetVal != NoError) return wRetVal;

  if(szRec[0] != '>') return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;

  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  /*********** secondly, set the PowerOn value ***********/
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  if( wID==0x7021 )
  {
     szCmd[0]='$';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3] = '4';
     szCmd[4]=0;
  }
  else
  {
     szCmd[0]='$';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3] = '4';
     szCmd[4] = wChannel+'0';
     szCmd[5]=0;
  }

  if(wFlag==1)
  {
    strcpy(szSend,szCmd);
    szReceive[0]=0;
  }
  wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
  if(wFlag==1)      strcpy(szReceive,szRec);

  if(wRetVal != NoError) return wRetVal;

  if(szRec[0] != '!') return ResultStrCheckError;

  if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}

/*********************************************************/
/*--- Read PowerOn Value for 7024, $AA7N ---*/
 WORD CALLBACK ReadPowerOnValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wID,wCheckSum,wTimeOut,wT,wChannel;
  //float   fVal;
  WORD    wRecStrLen;
  char    OutCh[10];

  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7024
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

  //---------------------- output ------------------------------------
  //  fVal=fBuf[0];      // DA output value


  if( wID==0x7024 )
  {
//     if( wChannel<0 || wChannel>3 )
     if( wChannel>3 )
     {
         return(AdChannelError);
     }
     szCmd[0]='$';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]='7';
     szCmd[4]=(char)(wChannel+0x30);  // channel No.
     szCmd[5]=0;
  }
  else
     return(ModuleIdError);

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=10; else wRecStrLen=12;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   for(i=0; i<7; i++)
      OutCh[i]=szRec[i+3];
   OutCh[7]=0;
   fBuf[0]=(float) atof(OutCh);

   return(NoError);
}

/*-------- 7011/7012/7014 Set PowerOn/Safe value --------*/
 WORD CALLBACK SetPowerOnSafeValue(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   WORD   wPowerOnValue,wSafeValue;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011/12/14
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wPowerOnValue=wBuf[5]; // PowerOn Value
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wSafeValue=wBuf[7];    // Safe value

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 : break;
       default: return(ModuleIdError);
   }

   if( wPowerOnValue>3 || wSafeValue>3 )
      return DataError;

   szCmd[0]='~';    // ~AA5PPSS
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='5';
   szCmd[4]='0';
   szCmd[5]=wPowerOnValue+'0';
   szCmd[6]='0';
   szCmd[7]=wSafeValue+'0';
   szCmd[8]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*--- Read PowerOn Value for 42/43/44/50/55/60/63/65/66/67 ---*/
 WORD CALLBACK ReadPowerOnValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2; //,A,B,C,D;
   WORD  wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT; //,wData
   WORD  wRecStrLen;
   char  *stopstring;
   char  OutCh[10];
   //float fValue;

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/60/63/65/66/67/42/43/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output -----------------------
   //   wBuf[5]:   PowerOn Value

   switch (wID)
   {
       case 0x7055 : //  8 bit DO , add by 20080830
       case 0x7050 : //  8 bit DO
       case 0x7060 : //  4 bit DO
	     case 0x7063 : //  3 bit DO
       case 0x7065 : //  5 bit DO
	     case 0x7066 : //  7 bit DO
       case 0x7067 : //  7 bit DO
       case 0x7044 : //  8 bit DO
                     wID=0;
                     break;
       case 0x7042 : //  13 bit DO
       case 0x7043 : //  16 bit DO
       case 0x7045 : wID=1; break;
                     wID=1;
                     break;
       default: return(ModuleIdError);
   }

   //--- ~AA4P ---
   szCmd[0]='~';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='4';
   szCmd[4]='P';
   szCmd[5]=0;


   if  (wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   if( wID==0 )
   {
      OutCh[0]=szRec[3];   OutCh[1]=szRec[4];
      OutCh[2]=0;
   }
   else
   {
      OutCh[0]=szRec[3];   OutCh[1]=szRec[4];
      OutCh[2]=szRec[5];   OutCh[3]=szRec[6];
      OutCh[4]=0;
   }

   wBuf[5]=(WORD)(strtoul(OutCh,&stopstring,16));

   return(NoError);
}

/*--- Read Safe Value for 42/43/44/50/55/60/63/65/66/67 ---*/
 WORD CALLBACK ReadSafeValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2; //,A,B,C,D;
   WORD  wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT; //,wData
   WORD  wRecStrLen;
   char  *stopstring;
   char  OutCh[10];
   //float fValue;

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/60/63/65/66/67/42/43/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output -----------------------
   //   wBuf[5]:   Safe Value Value

   switch (wID)
   {
       case 0x7055 : //  8 bit DO
       case 0x7050 : //  8 bit DO
       case 0x7060 : //  4 bit DO
	     case 0x7063 : //  3 bit DO
       case 0x7065 : //  5 bit DO
       case 0x7066 : //  7 bit DO
       case 0x7067 : //  7 bit DO
       case 0x7044 : //  8 bit DO
                     wID=0;
                     break;
       case 0x7042 : //  13 bit DO
       case 0x7043 : //  16 bit DO
       case 0x7045 : wID=1; break;
                     wID=1;
                     break;
       default: return(ModuleIdError);
   }

   //--- ~AA4S ---
   szCmd[0]='~';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='4';
   szCmd[4]='S';
   szCmd[5]=0;


   if  (wFlag==1)
   {
    strcpy(szSend,szCmd);
    szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   if( wID==0 )
   {
      OutCh[0]=szRec[3];   OutCh[1]=szRec[4];
      OutCh[2]=0;
   }
   else
   {
      OutCh[0]=szRec[3];   OutCh[1]=szRec[4];
      OutCh[2]=szRec[5];   OutCh[3]=szRec[6];
      OutCh[4]=0;
   }

   wBuf[5]=(WORD)(strtoul(OutCh,&stopstring,16));

   return(NoError);
}


/*-------- Read Config Status ---------*/
 WORD CALLBACK ReadConfigStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;   //,wAlarmMode
   WORD   wRecStrLen;
   WORD   A,B,C,D,E,F,G,H;
   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wAlarmMode=wBuf[5]; // Don't Care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- Output -------------------------------------
   // wBuf[7]:   module address
   // wBuf[8]:   module Range Code
   // wBuf[9]:   module baudrate
   // wBuf[10]:  module data format

   szCmd[0]='$';    // $AA2
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='2';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[1]);
   B=AsciiToHex(szRec[2]);
   wBuf[7]=A*16+B;

   C=AsciiToHex(szRec[3]);
   D=AsciiToHex(szRec[4]);
   wBuf[8]=C*16+D;

   E=AsciiToHex(szRec[5]);
   F=AsciiToHex(szRec[6]);
   wBuf[9]=E*16+F;

   G=AsciiToHex(szRec[7]);
   H=AsciiToHex(szRec[8]);
   wBuf[10]=G*16+H;

   return(NoError);
}



/* -------------------------------------------------------------------- */
 WORD CALLBACK AnalogIn(WORD wBuf[], float fBuf[],  
        char szSend[], char szReceive[])  //@07
{
char wPort,wFlag,cc[80],c1,c2;
char szCmd_sub[80],szRec_sub[80],ver[10];
WORD wAddr,wRetVal,wRetVal_sub,i,j,wChannel,wID,wCheckSum,wT,wTimeOut;
WORD wRecStrLen,w7016Mode;
float dFirmwareVer;
w7016Mode=0;
//---------------------- input -------------------------------------
wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
wAddr=wBuf[1];         // module address: 0x00 to 0xFF
wID=wBuf[2];           // module ID: 0x7005/11/12/13/14/15/16/17/18/19/33
wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
wTimeOut=wBuf[4];      // TimeOut constant: normal=100
wChannel=wBuf[5];      // channel number for multi-channel
wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
w7016Mode=wBuf[8];	   // to set 7016 mode mode :0 can only read one channel 
					   // mode : 1 can auto change to another channel	 	
//---------------------- output ------------------------------------
                        // fBuf[0]=analog input value
switch (wID)
       {
       case 0x7011: case 0x7012: case 0x7013: case 0x7014: //wID=0; //#AA  7011,7012,7013,7014 only 1 channel
		    szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=0;
	   break;
       case 0x7017: case 0x7018: case 0x7019: case 0x7005: //wID=1; //#AAN 7017,7018,7019 has 8 channels
			if (wChannel>7) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;
	   break;
       case 0x7033: //wID=2; //#AAN 7033 has 3 channels
			if (wChannel>2) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;		   
	   break;

       case 0x7015: //wID=2; //#AAN 7015 has 6 channels
			if (wChannel>5) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;		   
	   break;
       case 0x7016: //wID=3; //#AA 7016 7016D 2 channel 7016P 1channel
		  //AnalogIn for 7016 use $AA3C to set channel selected then use #AAC to read AnalogIn
		    c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd_sub[0]='$';
			szCmd_sub[1]=HexToAscii(c1);
			szCmd_sub[2]=HexToAscii(c2);
			szCmd_sub[3]='F';			
			szCmd_sub[4]=0;
			wRetVal_sub=Send_Receive_Cmd(wPort, szCmd_sub, szRec_sub, wTimeOut, wCheckSum,&wT);	
			sprintf(ver,"%s",szRec_sub+4);
			dFirmwareVer=(float)atof(ver);
			fBuf[10]=dFirmwareVer;

			if(w7016Mode==1)
			{
				if(dFirmwareVer>=1.3)
				{
					szCmd_sub[0]='$';
					szCmd_sub[1]=HexToAscii(c1);
					szCmd_sub[2]=HexToAscii(c2);
					szCmd_sub[3]='3';
					szCmd_sub[4]='3';
					szCmd_sub[5]=(char)(wChannel+0x30);
					szCmd_sub[6]=0;
				}
				else
				{
					szCmd_sub[0]='$';
					szCmd_sub[1]=HexToAscii(c1);
					szCmd_sub[2]=HexToAscii(c2);
					szCmd_sub[3]='3';
					szCmd_sub[4]=(char)(wChannel+0x30);
					szCmd_sub[5]=0;
				}
			}
		    szCmd[0]='#';
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=0;
	   break;
       default: return(ModuleIdError);
       }

   if  (wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   if ((wID==0x7016) && (w7016Mode==1))
   {
  		wRetVal_sub=Send_Receive_Cmd(wPort, szCmd_sub, szRec_sub, wTimeOut, wCheckSum,&wT);	
	
		if(wRetVal_sub != NoError) return wRetVal_sub;
		if(dFirmwareVer<1.3)
		Sleep(300);
   }

   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);//read ai here

   fBuf[0]=(float)0.0;
   //if(wRetVal != NoError) return wRetVal;

   if (szRec[0] != '>') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;


    //-----  Added by Shu, 7011, 7013, 7018, 7033
    // if( wBuf[2]==0x7013 || wBuf[2]==0x7011 || wBuf[2]==0x7018 || wBuf[2]==0x7033 )
    // {
    /*	if(strlen(szRec)==6)	//Total length is 6 bytes
		{
         if( !memcmp(szRec,">-0000",6) )
             return UnderInputRange;
         if( !memcmp(szRec,">+9999",6) )
             return ExceedInputRange;
		}   */          
     //}

     i=1; j=0;
     while (szRec[i])
     {
        cc[j++]=szRec[i++];
     }
     cc[j]=0;

     if( wCheckSum )  // checksum enable
        cc[j-2]=0;
     fBuf[0]=(float)atof(cc);
	 if(strlen(szRec)==6||strlen(szRec)==8)	//Total length is 6 bytes
	{
        
		 if( !memcmp(szRec,">-0000",6) && strlen(szRec)!=8 )
			return UnderInputRange;
		 
         if( !memcmp(szRec,">+9999",6)||!memcmp(szRec,">+9999.9",8)||!memcmp(szRec,">+999.99",8) )
             return ExceedInputRange;
	}             

     return(NoError);

}

/* -------------------------------------------------------------------- */
 WORD CALLBACK AnalogInFsr(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]) //def @8
{
   char  wPort,wFlag,cc[80],c1,c2;
   char szCmd_sub[80],szRec_sub[80];
   WORD  wAddr,wRetVal,wRetVal_sub,i,j,wChannel,wID,wCheckSum,wT,wTimeOut;
   WORD  wRecStrLen,w7016Mode;

   //---------------------- input --------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7005/11/12/13/14/15/16/17/18/19/33
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wChannel=wBuf[5];      // channel number for multi-channel
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   w7016Mode=wBuf[8];	   // to set 7016 mode mode :0 can only read one channel 
							//                  mode : 1 can auto change to another channel	 	
   //---------------------- output ------------------------------------
                           // fBuf[0]=analog input value in % FSR

switch (wID)
       {
       case 0x7011: case 0x7012: case 0x7013: case 0x7014: //wID=0; //#AA  7011,7012,7013,7014 only 1 channel
		    szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=0;
	   break;
       case 0x7017: case 0x7018: case 0x7019:case 0x7005: //wID=1; //#AAN 7017,7018,7019 has 8 channels
			if (wChannel>7) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;
	   break;
       case 0x7033: //wID=2; //#AAN 7033 has 3 channels
			if (wChannel>2) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;		   
	   break;
       case 0x7015: //wID=2; //#AAN 7015 has 6 channels
			if (wChannel>5) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;		   
	   break;
       case 0x7016: //wID=3; //#AA 7016 7016D 2 channel 7016P 1channel
		  
		    c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			if(w7016Mode==1)
			{
				szCmd_sub[0]='$';
				szCmd_sub[1]=HexToAscii(c1);
				szCmd_sub[2]=HexToAscii(c2);
				szCmd_sub[3]='3';
				szCmd_sub[4]=(char)(wChannel+0x30);
				szCmd_sub[5]=0;
			}
		    szCmd[0]='#';
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=0;
	   break;
       default: return(ModuleIdError);
       }

   if  (wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
    
   if ((wID==0x7016) && (w7016Mode==1))
   {
  		wRetVal_sub=Send_Receive_Cmd(wPort, szCmd_sub, szRec_sub, wTimeOut, wCheckSum,&wT);	
	
		if(wRetVal_sub != NoError) return wRetVal_sub;	
		Sleep(300);
   }


   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);//read ai here


   fBuf[0]=(float)0.0;
   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '>') return ResultStrCheckError;

    //-----  Added by Shu, 7011, 7013, 7018, 7033
    // if( wBuf[2]==0x7013 || wBuf[2]==0x7011 || wBuf[2]==0x7018 || wBuf[2]==0x7033 )
     //{
     //}



   if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;

//   if (strlen(szRec)!=wRecStrLen ) return ResultStrCheckError; // 01/08/2003 by sean

   i=1; j=0;
   while (szRec[i])
   {
      cc[j++]=szRec[i++];
   }
   cc[j]=0;

   if( wCheckSum )  // checksum enable
        cc[j-2]=0;
   fBuf[0]=(float)atof(cc);
     	if(strlen(szRec)==6||strlen(szRec)==8)	//Total length is 6 bytes
		{
         if( !memcmp(szRec,">-0000",6) && strlen(szRec)!=8 )
             return UnderInputRange;
         if( !memcmp(szRec,">+9999",6)||!memcmp(szRec,">+999.99",8) )
             return ExceedInputRange;
		}             

   return(NoError);
}


/* -------------------------------------------------------------------- */
 WORD CALLBACK AnalogInHex(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @9
{
   char   wPort,wFlag,c1,c2;
   char szCmd_sub[80],szRec_sub[80];
   WORD   wAddr,wRetVal,wRetVal_sub,wChannel,wID,wCheckSum,wT,wTimeOut;
//   WORD   wRecStrLen;
   WORD   A,B,C,D,w7016Mode;

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7005/11/12/13/14/15/16/17/18/19/33
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wChannel=wBuf[5];      // channel number for multi-channel
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   w7016Mode=wBuf[8];	  // to set 7016 mode mode :0 can only read one channel 
						  // mode : 1 can auto change to another channel	 	
   //---------------------- output ---------------------------
                           // wBuf[7]=analog input value

switch (wID)
       {
       case 0x7011: case 0x7012: case 0x7013: case 0x7014: //wID=0; //#AA  7011,7012,7013,7014 only 1 channel
		    szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=0;
	   break;
       case 0x7017: case 0x7018: case 0x7019: case 0x7005: //wID=1; //#AAN 7017,7018,7019 has 8 channels
			if (wChannel>7) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;
	   break;
       case 0x7033: //wID=2; //#AAN 7033 has 3 channels
			if (wChannel>2) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;		   
	   break;
       case 0x7015: //wID=2; //#AAN 7015 has 6 channels
			if (wChannel>5) return(AdChannelError);
			szCmd[0]='#';
			c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=(char)(wChannel+0x30);
			szCmd[4]=0;		   
	   break;
       case 0x7016: //wID=3; //#AA 7016 7016D 2 channel 7016P 1channel
		  
		    c2=(char)(wAddr&0x0f);
			wAddr=wAddr/16;
			c1=(char)(wAddr&0x0f);
			if(w7016Mode==1)
			{
				szCmd_sub[0]='$';
				szCmd_sub[1]=HexToAscii(c1);
				szCmd_sub[2]=HexToAscii(c2);
				szCmd_sub[3]='3';
				szCmd_sub[4]=(char)(wChannel+0x30);
				szCmd_sub[5]=0;
			}
		    szCmd[0]='#';
			szCmd[1] = HexToAscii(c1);
			szCmd[2] = HexToAscii(c2);
			szCmd[3]=0;
	   break;
       default: return(ModuleIdError);
       }

   if  (wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
    
   if ((wID==0x7016) && (w7016Mode==1))
   {
  		wRetVal_sub=Send_Receive_Cmd(wPort, szCmd_sub, szRec_sub, wTimeOut, wCheckSum,&wT);	
	
		if(wRetVal_sub != NoError) return wRetVal_sub;	
		Sleep(300);
   }

   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);//read ai here

   if(wRetVal != NoError) return wRetVal;

   if (szRec[0] != '>')   return ResultStrCheckError;


//   if(wCheckSum==0) wRecStrLen=5; else wRecStrLen=7;

//   if ( strlen(szRec)!=wRecStrLen ) return ResultStrCheckError; // 01/08/2003 by sean
   	if(strlen(szRec)==6 || strlen(szRec)==5||strlen(szRec)==7 || strlen(szRec)==8)	//Total length is 6 bytes
	{	
		if( !memcmp(szRec,">-0000",6) )
		{
		A=AsciiToHex(szRec[2]);
		B=AsciiToHex(szRec[3]);
		C=AsciiToHex(szRec[4]);
		D=AsciiToHex(szRec[5]);
		wBuf[7]=D+C*16+B*256+A*4096;
		return UnderInputRange;
		}
    	if( !memcmp(szRec,">+9999",6))  
		{
		A=7;//AsciiToHex(szRec[2]);
		B=0xF;//AsciiToHex(szRec[3]);
		C=0xF;//AsciiToHex(szRec[4]);
		D=0xF;//AsciiToHex(szRec[5]);
		wBuf[7]=D+C*16+B*256+A*4096;
		return ExceedInputRange;
		}
		if (wID!=0x7018)
		{
			if(!memcmp(szRec,">7FFF",5))  //for 7018 7FFF not means broken line
			{
				A=AsciiToHex(szRec[1]);
				B=AsciiToHex(szRec[2]);
				C=AsciiToHex(szRec[3]);
				D=AsciiToHex(szRec[4]);
				wBuf[7]=D+C*16+B*256+A*4096;
				return ExceedInputRange;
			}
		}
	
		A=AsciiToHex(szRec[1]);
		B=AsciiToHex(szRec[2]);
		C=AsciiToHex(szRec[3]);
		D=AsciiToHex(szRec[4]);
		wBuf[7]=D+C*16+B*256+A*4096;
	
   }

   return(NoError);
}


/* -------------------------------------------------------------------- */

WORD CALLBACK AnalogIn8(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
	char wPort;//,cTemp[80]
	WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wDebug;//,wT,i,j;
	//WORD wRecStrLen;

//---------------------- input -------------------------------------
wPort=(char)wBuf[0];	// RS-232 port number: 1/2/3/4/..../255
wAddr=wBuf[1];			// module address: 0x00 to 0xFF
wID=wBuf[2];			// module ID: 0x7017/18/19
wCheckSum=wBuf[3];		// checksum: 0=disable, 1=enable
wTimeOut=wBuf[4];		// TimeOut constant: normal=100
wDebug=wBuf[6];		// 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                        // fBuf[0]=analog input value of channel_0
                        // fBuf[1]=analog input value of channel_1
                        // ........................................
                        // fBuf[7]=analog input value of channel_7

	switch (wID)
	{
		case 0x7017 :
		case 0x7018 :
		case 0x7019 :
    case 0x7005:
			break;
		default: return(ModuleIdError);
	}
	wRetVal=AnalogInAll(wBuf,fBuf,szSend,szReceive);
	
	/*sprintf(szCmd,"#%02X",wAddr);

	wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);

	if(wRetVal!=NoError)
		return wRetVal;

	if(wDebug)
	{
		strcpy(szSend,szCmd);
		strcpy(szReceive,szRec);
	}
	if (wCheckSum==0) wRecStrLen=57; else wRecStrLen=59;   //01/08/2003 modify by sean

	// Only check the first byte and total bytes.

	if (szRec[0] != '>') return ResultStrCheckError;  //01/08/2003 modify by sean

	if (strlen(szRec) != wRecStrLen) return ResultStrCheckError; //01/08/2002 modify by sean

	for(i=0;i<8;i++)
	{
		//"+03.567" one float value uses 7 bytes.
		for (j=0;j<7;j++) 
			cTemp[j]=szRec[1+i*7+j];
		fBuf[i]=(float)atof(cTemp);
    }

	return(NoError);*/
	return wRetVal;
}

/* -------------------------------------------------------------------- */

WORD CALLBACK AnalogInAll(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
char wPort,wFlag,cc[80],c1,c2,wMode;
WORD wRet,wAddr,wRetVal,i,t,j,wID,wCheckSum,wTimeOut,wT,k;
WORD wRecStrLen;
//int iRet;
//float  fBuf2[10];
char *endptr;
//char *pch;
//char str[100];

//---------------------- input -------------------------------------
wPort=(char)wBuf[0]; // RS-232 port number: 1/2/3/4/..../255
wAddr=wBuf[1];           // module address: 0x00 to 0xFF
wID=wBuf[2];             // module ID: 0x7005/11/12/13/14/15/16/17/18/19/33
wCheckSum=wBuf[3];       // checksum: 0=disable, 1=enable
wTimeOut=wBuf[4];        // TimeOut constant: normal=100
wFlag=(char)wBuf[6]; // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
						// Engineer and FSR mode
                        // fBuf[0]=analog input value of channel_0
                        // fBuf[1]=analog input value of channel_1
                        // ........................................
                        // fBuf[7]=analog input value of channel_7
//---------------------- output ------------------------------------
						// Hex mode
						// wBuf[8] = Hex input value of channel_0
						// wBuf[9] = Hex input value of channel_1
						//..........................................
						// wBuf[15] = Hex input value of channel_7

wMode=(char)wBuf[7]; // 0: Engineer 1:% FSR 2:Hex

switch (wID)  //0x7005/11/12/13/14/15/16/17/18/19/33
       {      
	   case 0x7005 :    // add by cidny 20081225
	   case 0x7011 :
	   case 0x7012 :
	   case 0x7013 :
	   case 0x7014 :
     case 0x7015 :
	   case 0x7016 :  // add 2004/10/26 for 7016 new version >= B1.3
     case 0x7017 :
     case 0x7018 : 
	   case 0x7019 : 
	   case 0x7033 :
	   break;
       default: return(ModuleIdError);
       }

    szCmd[0]='#';           //#AA
    c2=(char)(wAddr&0x0f);
    wAddr=wAddr/16;
    c1=(char)(wAddr&0x0f);
    szCmd[1] = HexToAscii(c1);
    szCmd[2] = HexToAscii(c2);
    szCmd[3] = 0;

    if  (wFlag==1)
    {
		  strcpy(szSend,szCmd);
		  szReceive[0]=0;
    }
	 wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

	 for (j=0; j<7; j++) fBuf[j]=(float)0.0;
   if(wRetVal != NoError) return wRetVal;

	 if (szRec[0] != '>') return ResultStrCheckError;
         

	t=1;
  switch (wID)
  {
    case 0x7005 :    // add by cidny 20081225
	  case 0x7011 :
	  case 0x7012 :
    case 0x7013 :
   	case 0x7014 :
    case 0x7033 :
	  case 0x7016 :	
/*	  t=1;

	  if (wCheckSum==0) wRecStrLen=22; else wRecStrLen=24;   //03/28/2003 modify by sean
	  if (strlen(szRec) != wRecStrLen) return ResultStrCheckError;
	  for (j=0; j<3; j++)
	  {
		wBuf[5]=j;
        wRetVal=AnalogIn(wBuf,fBuf2,szSend,szReceive);
	    fBuf[j]=fBuf2[0];
	  }  
      break;*/

	 case 0x7017 :
	 case 0x7018 :
	 case 0x7019 :
	 case 0x7015 :
//	  if (wCheckSum==0) wRecStrLen=57; else wRecStrLen=59;   //11/20/2002 modify by sean
//	  if (strlen(szRec) != wRecStrLen) return ResultStrCheckError;

	 
/*	  for (j=0; j<8; j++)
	  {
		for (i=0; i<7; i++) cc[i]=szRec[i+t];
		fBuf[j]=(float)atof(cc);
		t=t+7;
	  }  */
		i=1; //szRec[0] is '>'
		k=0;
		j=0;
		if (!wCheckSum) 
	 		wRecStrLen=strlen(szRec);
		else
			wRecStrLen=strlen(szRec)-2;
		wRet=NoError;
		switch(wMode)
		{
			case 0: //Engineer
			case 1: //Fsr
			case 3: //Ohm
			while(i<=wRecStrLen) //57 (without '\0')
			{
				if(szRec[i]=='+' || szRec[i]=='-' || i==wRecStrLen || (szRec[i]==0x20 && (i-1) % 7==0))
				{
					
					if(i!=1 || i==wRecStrLen) 
					{
						cc[j]='\0';

							if( !memcmp(cc,"-0000",5) && strlen(szRec)!=8)
							{
								strcpy(cc,"-9999");
								wRet=UnderInputRange;
							}
							else if( !memcmp(cc,"+9999",5) )
							{
								wRet=ExceedInputRange;
							}
							else if( !memcmp(cc,"+9999.9",7) )
							{
								wRet=ExceedInputRange;
							}
							else if( !memcmp(cc,"+999.99",7) )
							{
								wRet=ExceedInputRange;
							}

						
							fBuf[k]=(float)atof(cc);
							if(i==wRecStrLen)
								break;
								k++;
								j=0;
						
					}
					cc[j++]=szRec[i];
				}
				else			
					cc[j++]=szRec[i];			
				i++;
			}//end of while
			break;
			case 2: //Hex
			k=8; //Start from wBuf[8] 'k means index of wBuf
			while(i<=wRecStrLen) // (without '\0') 'i means index of szRec
			{
				
				if(wID==0x7019 || wID==0x7015) //7019 read all channel don't detect broken line
				{
					for(j=0;j<4;j++)					
						cc[j]=szRec[i++];					
				}
				else
				{
					if(szRec[i]=='+' || szRec[i]=='-')
					{				
						for(j=0;j<5;j++)
							cc[j]=szRec[i++];
				
						if(!memcmp(cc,"-0000",5))
						{
							strcpy(cc,"8000");//UnderRange or the cable broke
							wRet=UnderInputRange;
						}
						else if(!memcmp(cc,"+9999",5))
						{
							strcpy(cc,"+7FFF"); //OverRange or the cable broke
							wRet=ExceedInputRange;
						}
					
					}
				
					else			
					{
						for(j=0;j<4;j++)
							cc[j]=szRec[i++];					
					}
				}
				cc[j]='\0';
				wBuf[k++]=(WORD)strtol(cc,&endptr,16); //16 means 0~9,A~F
			}
			break;
		}
		break;
  }
	return wRet;
}

/* -------------------------------------------------------------------- */

/*WORD CALLBACK AnalogInAllHex(WORD wBuf[], float fBuf[],
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
//wID=wBuf[2];            // module ID: 0x7017/18/19/33
//wCheckSum=wBuf[3];      // checksum: 0=disable, 1=enable
//wTimeOut=wBuf[4];       // TimeOut constant: normal=100
//wFlag=(char)wBuf[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
						// Hex mode
						// wBuf[8] = Hex input value of channel_0
						// wBuf[9] = Hex input value of channel_1
						//..........................................
						// wBuf[15] = Hex input value of channel_7
	wBuf[7]=2;			// 0: Engineer 1:% FSR 2:Hex

	wRetVal=AnalogInAll(wBuf,fBuf,szSend,szReceive);

	return wRetVal;
}

WORD CALLBACK AnalogInAllFsr(WORD wBuf[], float fBuf[],
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
//wID=wBuf[2];            // module ID: 0x7017/18/19/33
//wCheckSum=wBuf[3];      // checksum: 0=disable, 1=enable
//wTimeOut=wBuf[4];       // TimeOut constant: normal=100
//wFlag=(char)wBuf[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
						// Engineer and FSR mode
                        // fBuf[0]=analog input value of channel_0
                        // fBuf[1]=analog input value of channel_1
                        // ........................................
                        // fBuf[7]=analog input value of channel_7
	wBuf[7]=1;		    // 0: Engineer 1:% FSR 2:Hex

	wRetVal=AnalogInAll(wBuf,fBuf,szSend,szReceive);

	return wRetVal;
}*/

/* -------------------------------------------------------------------- */

 WORD CALLBACK In8_7017(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]) //def @12
{
	char  wPort,wFlag,c1,c2,InHex[5];
	WORD  wAddr,wRetVal,t,j,wID,wCheckSum,wTimeOut,wT;
	float fRange;
	DWORD dwVal;
	WORD  wRecStrLen;
	char  *cp;

	//---------------------- input -------------------------------------
	wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
	wAddr=wBuf[1];         // module address: 0x00 to 0xFF
	wID=wBuf[2];           // module type: 08/09/0A/0B/0C/0D
	wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
	wTimeOut=wBuf[4];      // TimeOut constant: normal=100
	wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
	//---------------------- output ------------------------------------
		                    // fBuf[0]=analog input value of channel_0
			                // fBuf[1]=analog input value of channel_1
				            // ........................................
					        // fBuf[7]=analog input value of channel_7

    switch (wID)
    {
       case 0x08 : fRange=(float)10.0;  break;
       case 0x09 : fRange=(float)5.0;   break;
       case 0x0A : fRange=(float)1.0;   break;
       case 0x0B : fRange=(float)500.0; break;
       case 0x0C : fRange=(float)150.0; break;
       case 0x0D : fRange=(float)20.0;  break;
       default: return(ModuleIdError);
    }

    szCmd[0]='$';           //$AAA
    c2=(char)(wAddr&0x0f);
    wAddr=wAddr/16;
    c1=(char)(wAddr&0x0f);
    szCmd[1] = HexToAscii(c1);
    szCmd[2] = HexToAscii(c2);
    szCmd[3] = 'A';
    szCmd[4] = 0;

	if  (wFlag==1)
    {
		strcpy(szSend,szCmd);
		szReceive[0]=0;
    }
	wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
    if(wFlag==1)      strcpy(szReceive,szRec);

    for (j=0; j<7; j++) fBuf[j]=(float)0.0;
    if(wRetVal != NoError) return wRetVal;

    if (szRec[0] != '>')  return ResultStrCheckError;

    if (wCheckSum==0) wRecStrLen=33; else wRecStrLen=35;

    if (strlen(szRec) != wRecStrLen) return ResultStrCheckError;

    t=1;
    InHex[4]=0;
    for (j=0; j<8; j++)
    {
       InHex[0]=szRec[t++];
       InHex[1]=szRec[t++];
       InHex[2]=szRec[t++];
       InHex[3]=szRec[t++];
       dwVal=strtol(InHex,&cp,16);
       if (dwVal<=32767) 
		       fBuf[j]=(float)dwVal/(float)32767.0*fRange;
       else
           fBuf[j]=(float)(65535-dwVal)/(float)32767.0*fRange*(float)(-1.0);
    }

    return(NoError);
}

/* -------------------------------------------------------------------- */

 WORD CALLBACK AnalogOut(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]) //def @13
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,i,wID,wCheckSum,wTimeOut,wT,wChannel;
  float   fVal;
  WORD    wRecStrLen;
  char    OutCh[10];

  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7016/21/22/24
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Not used if module ID is 7016/7021
                             // Channel No.(0 to 1) if module ID is 7022
                             // Channel No.(0 to 3) if module ID is 7024
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
  fVal=fBuf[0];          // analog output value
  //---------------------- output ------------------------------------
                          // void
  if( wID==0x7016 )
  {
     sprintf(OutCh,"%+07.3f",fBuf[0]);

     szCmd[0]='$';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1]=HexToAscii(c1);
     szCmd[2]=HexToAscii(c2);
     szCmd[3]='7';

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=OutCh[6];
     szCmd[11]=0;
  }
  else if(wID==0x7021)
  {
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);

   szCmd[5]='.';
   szCmd[9]=0;


   i=(WORD)(fVal/10.0);
   szCmd[3]=i+0x30; fVal-=i*10;

   i=(int)fVal;
   szCmd[4]=i+0x30; fVal-=i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[6]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[7]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[8]=i+0x30;
  }
  else if( wID==0x7022 )
  {
//     if( wChannel<0 || wChannel>2 )
     if(wChannel>2 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%06.3f",fBuf[0]);

     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=0;
   }
  else if( wID==0x7024 )
  {
//     if( wChannel<0 || wChannel>3 )
     if( wChannel>3 )
     {
         return(AdChannelError);
     }
     sprintf(OutCh,"%+07.3f",fBuf[0]);
         // sprintf(OutCh,"%+07.1f",fValue);


     szCmd[0]='#';
     c2=(char)(wAddr&0x0f);
     wAddr=wAddr/16;
     c1=(char)(wAddr&0x0f);
     szCmd[1] = HexToAscii(c1);
     szCmd[2] = HexToAscii(c2);
     szCmd[3]=(char)(wChannel+0x30);  // channel No.

     szCmd[4]=OutCh[0];
     szCmd[5]=OutCh[1];
     szCmd[6]=OutCh[2];
     szCmd[7]=OutCh[3];
     szCmd[8]=OutCh[4];
     szCmd[9]=OutCh[5];
     szCmd[10]=OutCh[6];
     szCmd[11]=0;
  }
  else
     return(ModuleIdError);


   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

  if( wID==0x7016 )
  {
     if(szRec[0] != '!')
     {
        return ResultStrCheckError;
     }
     if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;
  }
  else    // 7021, 7024
  {
     if(szRec[0] != '>')
     {
        return ResultStrCheckError;
     }
     if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  }

  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}
/* -------------------------------------------------------------------- */

 WORD CALLBACK AnalogOutHex(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]) 
{
  char    wPort,wFlag,c1,c2;
  WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wChannel;
  WORD    wRecStrLen;
  char    sHex[5];
  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7021/22 (7024 has no Hex type)
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Not used if module ID is 7021
                             // Channel No.(0 to 1) if module ID is 7022 
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
  sprintf(sHex,"%03X",wBuf[7]); //analog output value(Hexadeciaml Data Foramt)
 		
  //---------------------- output ------------------------------------
  if(wID==0x7021)
  {
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);  
   szCmd[3]=  sHex[0];
   szCmd[4]=  sHex[1];	
   szCmd[5]=  sHex[2];
   szCmd[6]=0;
  }
  else if(wID==0x7022)
  {
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2); 
   szCmd[3]=(char)(wChannel+0x30);  // channel No.
   szCmd[4]=  sHex[0];
   szCmd[5]=  sHex[1];	
   szCmd[6]=  sHex[2];
   szCmd[7]=0;
  }
  else
     return(ModuleIdError);


   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   // 7021, 7022
	if(szRec[0] != '>')
    {
		return ResultStrCheckError;
    }
    if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
}
/* -------------------------------------------------------------------- */

 WORD CALLBACK AnalogOutFsr(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]) 
 {
  char    wPort,wFlag;
  //char    c1,c2;
  //WORD	  i;
  WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wChannel;
  WORD    wRecStrLen;
  float   fVal;

  //---------------------- input -------------------------------------
  wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
  wAddr=wBuf[1];         // module address: 0x00 to 0xFF
  wID=wBuf[2];           // module ID: 0x7021/22 (7024 has no Fsr type)
  wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
  wTimeOut=wBuf[4];      // TimeOut constant: normal=100
  wChannel=wBuf[5];      // Not used if module ID is 7021
                             // Channel No.(0 to 1) if module ID is 7022 
  wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
//  sprintf(sHex,"%03X",wBuf[7]);
	fVal=fBuf[0];          // analog output value 		
  //---------------------- output ------------------------------------
  if(wID==0x7021) //#AA(Data)
  {
	  sprintf(szCmd,"#%02X+%006.2f",wAddr,fVal); //'03/5/22 by sean
 /*  szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]=  '+';

   i=(WORD)(fVal/100.0);
   szCmd[4]=i+0x30; fVal-=i*100;

   i=(int)(fVal/10.0);
   szCmd[5]=i+0x30; fVal-=i*10;
	
   i=(int)fVal;
   szCmd[6]=i+0x30; fVal-=i;
   szCmd[7]='.';
   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[8]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[9]=i+0x30; fVal-=(float)i;
	szCmd[10]=0;*/
  }
  else if(wID==0x7022)//#AAN(Data)
  {
	  sprintf(szCmd,"#%02X%01d+%006.2f",wAddr,wChannel,fVal); //'03/5/22 by sean
/*   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]=(char)(wChannel+0x30);  // channel No.
   szCmd[4]=  '+';

   i=(WORD)(fVal/100.0);
   szCmd[5]=i+0x30; fVal-=i*100;

   i=(int)(fVal/10.0);
   szCmd[6]=i+0x30; fVal-=i*10;
	
   i=(int)fVal;
   szCmd[7]=i+0x30; fVal-=i;
   szCmd[8]='.';
   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[9]=i+0x30; fVal-=(float)i;

   fVal *= (float)10.0;
   i=(int)fVal;
   szCmd[10]=i+0x30; fVal-=(float)i;
	szCmd[11]=0;*/
  }
  else
     return(ModuleIdError);


   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   // 7021, 7022
	if(szRec[0] != '>')
    {
		return ResultStrCheckError;
    }
    if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
  
  if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

  return(NoError);
 }
/* -------------------------------------------------------------------- */

 WORD CALLBACK AnalogOutReadBack(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @14
{
   char  wPort,wFlag,cc[80],c1,c2;
   WORD  wAddr,wRetVal,i,j,wReadType,wID,wCheckSum,wT,wTimeOut,wChannel;
   WORD  wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];  // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];        // module address: 0x00 to 0xFF
   wID=wBuf[2];          // module ID: 0x7016/7021/7022/7024
   wCheckSum=wBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];     // TimeOut constant: normal=100
   wReadType=wBuf[5];    // for 7016 :  Don't Care
                          // for 7021/7022/7024
                          //     0: command read back ($AA6)
                          //     1: analog output of current path read back ($AA8)
   wFlag=(char)wBuf[6];  // flag: 0=no save, 1=save send/receive string
   wChannel=wBuf[7];     //   Channel No.(0 to 3), if wID is 7024
                          //   Not used,            else
   //---------------------- output ------------------------------------
                          // fBuf[0]=analog output value read back

   switch(wID)
   {
       case 0x7016:
             szCmd[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd[1] = HexToAscii(c1);
             szCmd[2] = HexToAscii(c2);
             szCmd[3]='6';
             szCmd[4]=0;
             break;
       case 0x7021:
             if (wReadType==0) szCmd[3]='6';
             else if (wReadType==1) szCmd[3]='8';
             else return(CmdError);

             szCmd[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd[1] = HexToAscii(c1);
             szCmd[2] = HexToAscii(c2);
             szCmd[4]=0;
             break;
       case 0x7022:
//             if( wChannel<0 || wChannel>2 )
             if( wChannel>2 )
                 return(CmdError);
             szCmd[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd[1] = HexToAscii(c1);
             szCmd[2] = HexToAscii(c2);

             if (wReadType==0) szCmd[3]='6';
             else if (wReadType==1) szCmd[3]='8';
             else return(CmdError);

             szCmd[4]=(char)(wChannel+'0');  // channel No.
             szCmd[5]=0;
             break;
       case 0x7024:
//             if( wChannel<0 || wChannel>3 )
             if(wChannel>3 )
                 return(CmdError);
             szCmd[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd[1] = HexToAscii(c1);
             szCmd[2] = HexToAscii(c2);

             if (wReadType==0) szCmd[3]='6';
             else if (wReadType==1) szCmd[3]='8';
             else return(CmdError);

             szCmd[4]=(char)(wChannel+'0');  // channel N0.
             szCmd[5]=0;
             break;
       default: return(ModuleIdError);
   }


   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   fBuf[0]=(float)0.0;
   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11;
   if( wID==0x7016 || wID==0x7024 ) wRecStrLen+=1;

//   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   i=3; j=0;
   while (szRec[i])
   {
      cc[j++]=szRec[i++];
   }
   cc[j]=0;

   if( wCheckSum )  // checksum enable
      cc[j-2]=0;
   fBuf[0]=(float)atof(cc);

   return(NoError);
}
/* -------------------------------------------------------------------- */
 WORD CALLBACK AnalogOutReadBackFsr(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   WORD  wRecStrLen,wCheckSum,wID;
   WORD wRet;
   char wFlag;
   //---------------------- input -------------------------------------
   //wBuf[0];            // RS-232 port number: 1/2/3/4/..../255
   //wBuf[1];            // module address: 0x00 to 0xFF
   wID=wBuf[2];          // module ID: 7021/7022 (7024 have no Fsr mode)
   wCheckSum=wBuf[3];    // checksum: 0=disable, 1=enable
   //wBuf[4];            // TimeOut constant: normal=100
   //wBuf[5];            // for 7021/7022
                          //     0: command read back ($AA6)
                          //     1: analog output of current path read back ($AA8)
   wFlag=(char)wBuf[6];  // flag: 0=no save, 1=save send/receive string
   //wBuf[7];            //   Channel No.(0 to 3)
   //---------------------- output ------------------------------------
                          // fBuf[0]=analog output value read back     
   switch(wID)
   {      
       case 0x7021:  
       case 0x7022:            
             break;
      
       default: return(ModuleIdError);
   }
	if(!wFlag)
		wFlag=1;
	wRet=AnalogOutReadBack(wBuf,fBuf,szSend,szReceive);
	if(!wRet)
	{
		if(wCheckSum==0) wRecStrLen=10; else wRecStrLen=12;
	    if(strlen(szReceive) != wRecStrLen) return ResultStrCheckError;
//		if(!wBuf[6]) //if flag=0; set szReceive=null
//			strnset(szReceive,0x00,strlen(szReceive));
		return(NoError);
	}
	else
		return wRet;
 }
/* -------------------------------------------------------------------- */

 WORD CALLBACK AnalogOutReadBackHex(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char  wPort,wFlag,c1,c2;
   WORD  wAddr,wRetVal,wReadType,wID,wCheckSum,wT,wTimeOut,wChannel;
   WORD  wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];  // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];        // module address: 0x00 to 0xFF
   wID=wBuf[2];          // module ID: 0x7021/21P/22
   wCheckSum=wBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];     // TimeOut constant: normal=100
   wReadType=wBuf[5];    // for 7021/7022
                          //     0: command read back ($AA6)
                          //     1: analog output of current path read back ($AA8)
   wFlag=(char)wBuf[6];  // flag: 0=no save, 1=save send/receive string
   wChannel=wBuf[7];     //   Channel No.(0 to 1),for 7022
                          //   Not used,            else
   //---------------------- output ------------------------------------
                          // wBuf[9] analog output value read back(Hex)

   switch(wID)
   {
       case 0x7021:
             if (wReadType==0) szCmd[3]='6';
             else if (wReadType==1) szCmd[3]='8';
             else return(CmdError);

             szCmd[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd[1] = HexToAscii(c1);
             szCmd[2] = HexToAscii(c2);
             szCmd[4]=0;
             break;
       case 0x7022:
//             if( wChannel<0 || wChannel>2 )
             if( wChannel>2 )
                 return(CmdError);
             szCmd[0]='$';
             c2=(char)(wAddr&0x0f);
             wAddr=wAddr/16;
             c1=(char)(wAddr&0x0f);
             szCmd[1] = HexToAscii(c1);
             szCmd[2] = HexToAscii(c2);

             if (wReadType==0) szCmd[3]='6';
             else if (wReadType==1) szCmd[3]='8';
             else return(CmdError);

             szCmd[4]=(char)(wChannel+'0');  // channel No.
             szCmd[5]=0;
             break;

       default: return(ModuleIdError);
   }


   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=6; else wRecStrLen=8;  //!01FFF
//   if( wID==0x7021 ) wRecStrLen+=1;//!01FFFF

//   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError; //01/08/2003 by sean

//   i=3; j=0;
   sscanf(szRec+3,"%X",&wBuf[9]);
/*   while (szRec[i])
   {
      cc[j++]=szRec[i++];
   }
   cc[j]=0;

   if( wCheckSum )  // checksum enable
      cc[j-2]=0;
   fBuf[0]=(float)atof(cc);*/

   return(NoError);
}
/* -------------------------------------------------------------------- */
 WORD CALLBACK DigitalOut(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])  //def @15
{
   char  wPort,wFlag,c1,c2,A,B,C,D; //,i;
   WORD  wAddr,wAddr2,wRetVal,wData,wID,wCheckSum,wTimeOut,wT;
   WORD  wModuleID;
   WORD  wRecStrLen;
   //UCHAR OutData[4];
   char szCmd2[80];

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wModuleID=wID=wBuf[2];     // module ID: 0x7050/60/63/65/66/67/42/43/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wData=wBuf[5];         // 16-bit digital data to output
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output -----------------------
                        // void

   wAddr2=wAddr;
   switch (wID)
   {
       case 0x7055 : //  @AA(data)
       case 0x7050 : //  @AA(data)
       case 0x7060 : 
       case 0x7063 :
       case 0x7065 :
       case 0x7066 :
       case 0x7067 : 
       case 0x7044 : 
                     wID=0;
                     break;
       case 0x7042 : //  @AA(data)
       case 0x7043 : 
       case 0x7045:
                     wID=1;
                     break;
       case 0x7011:     // @AADO0?   0?=00  Bit0:Off   Bit1:Off
       case 0x7012:     //           0?=01  Bit0:On    Bit1:Off
       case 0x7014:     //           0?=02  Bit0:Off   Bit1:On
       case 0x7080:     //           0?=03  Bit0:On    Bit1:On
       case 0x7005:     // 07/07/2004
                     wID=2;
                     break;
      /***
       case 0x7016:
                        //  @AADO0?  for Bit0, Bit1
                        //  @AADO1?  for Bit2, Bit3
                     wID=3;
                     break;
      ***/
       default: return(ModuleIdError);
   }

   if(wID == 0 || wID==1 ) // @AA(DATA)
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

     switch( wModuleID )
	   {
       case 0x7060:
            wData&=0x000f;
		        A=(char)wData;
            szCmd[3]=HexToAscii(A);
            szCmd[4]=0;
	    	    break;
       case 0x7063:
		        wData&=0x0007;
            A=(char)wData;
            szCmd[3]=HexToAscii(A);
            szCmd[4]=0;
		        break;
    	case 0x7044:
    	case 0x7055:   //add by 20080830
      case 0x7050:
		       wData&=0x00FF;
           B=wData&0x0f; wData=wData/16;
           A=wData&0x0f;
		       szCmd[3]=HexToAscii(A);
		       szCmd[4]=HexToAscii(B);
		       szCmd[5]=0;
		       break;
      case 0x7065:
		       wData&=0x001F;
           B=wData&0x0f; wData=wData/16;
           A=wData&0x0f;
		       szCmd[3]=HexToAscii(A);
		       szCmd[4]=HexToAscii(B);
		       szCmd[5]=0;
		       break;
     case 0x7066:
	   case 0x7067:
		      wData&=0x007F;
          B=wData&0x0f; wData=wData/16;
          A=wData&0x0f;
	   	    szCmd[3]=HexToAscii(A);
		      szCmd[4]=HexToAscii(B);
		      szCmd[5]=0;
		      break;
	   case 0x7042:
		      wData&=0x1FFF;
	        D=wData&0x0f; wData=wData/16;
	        C=wData&0x0f; wData=wData/16;
	        B=wData&0x0f; wData=wData/16;
	        A=wData&0x0f;
	        szCmd[3]=HexToAscii(A);
		      szCmd[4]=HexToAscii(B);
	        szCmd[5]=HexToAscii(C);
	        szCmd[6]=HexToAscii(D);
	        szCmd[7]=0;
		      break;
	   case 0x7043: 
	   case 0x7045:
		   wData&=0xFFFF;
	         D=wData&0x0f; wData=wData/16;
	         C=wData&0x0f; wData=wData/16;
	         B=wData&0x0f; wData=wData/16;
	         A=wData&0x0f;
	         szCmd[3]=HexToAscii(A);
	         szCmd[4]=HexToAscii(B);
	         szCmd[5]=HexToAscii(C);
	         szCmd[6]=HexToAscii(D);
	         szCmd[7]=0;
		   break;
	  } 

   }
   else if(wID==2)  //@AADO??
   {
      
	  if(wModuleID==0x7005 ) 
	  {
		 if( wData>0x3f )
		{
			return DataError;
		}

		szCmd[0]='@';
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);        
		szCmd[3]='D';
		szCmd[4]='O';
	    wData&=0x3F;
		B=wData&0x0f; wData=wData/16;
	    A=wData&0x0f;
	    szCmd[5]=HexToAscii(A);
	    szCmd[6]=HexToAscii(B);
		szCmd[7]=0;
	  }
	  else
	  {
		 if( wData>3 )
		{
			return DataError;
		}

		szCmd[0]='@';
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);

		szCmd[3]='D';
		szCmd[4]='O';
	 
		szCmd[5]='0';
		szCmd[6]=wData+'0';
		szCmd[7]=0;
	  }
	}

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   if( wID==3 )   // 7016
   {
      wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
      if(wFlag==1)      strcpy(szReceive,szRec);

      if(wRetVal != NoError) return wRetVal;

      if(szRec[0] != '!') return ResultStrCheckError;

      Sleep(10);

      wRetVal=Send_Receive_Cmd(wPort, szCmd2, szRec, wTimeOut, wCheckSum,&wT);
      if(wFlag==1)      strcpy(szReceive,szRec);

      if(wRetVal != NoError) return wRetVal;
   }
   else
   {
      wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
      if(wFlag==1)      strcpy(szReceive,szRec);

      if(wRetVal != NoError) return wRetVal;
   }

   if( wID==0 || wID==1 )  // Digit I/O module
   {
      if(szRec[0] != '>')
      {
         return ResultStrCheckError;
      }
   }
   else  // 7011, 7012, 7014, 7080 
	     //7005 07/07/2004
   {
      if(szRec[0] != '!')
      {
         return ResultStrCheckError;
      }
   }

   if( wID==0 || wID==1 )  // Digit I/O module
   {
      if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
   }
   else
   {
      if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;
   }

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*---------- Output a specific bit for a DI/DO module ------------ */
 WORD CALLBACK DigitalBitOut(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @16
{
   char  wPort,wFlag,c1,c2; //,A,B,C,D,i;
   WORD  wAddr,wRetVal,wChannel,wData,wID,wCheckSum,wTimeOut,wT;//,wAddr2
   WORD  wRecStrLen;
   //char  szCmd2[80];

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/60/66/67/42/43/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wData=wBuf[5];         // don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wChannel=wBuf[7];      // which channel to output
   wData=wBuf[8];         // data to output
   //---------------------- output -----------------------
                        // void

   if( wData>1 )
     return DataError;
   switch (wID)
   {
       case 0x7055 :  //8 channel DI/O, add by 20080830
       case 0x7050 : if( wChannel>7 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7060 : if( wChannel>3 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7063 : if( wChannel>2 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7065 : if( wChannel>4 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7066 : if( wChannel>6 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7067 : if( wChannel>6 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7044 : if( wChannel>7 )
                        return DataError;
                     wID=0;
                     break;
       case 0x7042 : if( wChannel>12 )
                        return DataError;
                     wID=1;
                     break;
       case 0x7043 : if( wChannel>15 )
                        return DataError;
                     wID=1;
                     break;
       default: return(ModuleIdError);
   }

   if(wID == 0) //#AA00DD
   {
      szCmd[0]='#';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='1';
      szCmd[4]=wChannel+'0';
      szCmd[5]='0';
      szCmd[6]=wData+'0';
      szCmd[7]=0;
   }
   else
   {
      szCmd[0]='#';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);

      if( wChannel>7 )
      {
         szCmd[3]='B';
         wChannel-=8;
      }
      else
      {
         szCmd[3]='A';
      }
      szCmd[4]=wChannel+'0';
      szCmd[5]='0';
      szCmd[6]=wData+'0';
      szCmd[7]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '>') return ResultStrCheckError;

 //  if( wID==0 )  // Digit I/O module
//   {
      if(wCheckSum==0) wRecStrLen=1; else wRecStrLen=3;
//   }

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/* -------------------------------------------------------------------- */
 WORD CALLBACK DigitalOut_7016(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @17
{
   char  wPort,wFlag,c1,c2; //,A,B,C,D;
   WORD  wAddr,wRetVal,wData,wID,wCheckSum,wTimeOut,wT;
   WORD  wRecStrLen,wWhichBits;
   //UCHAR OutData[4];

   //---------------------- input ---------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7016
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wData=wBuf[5];         // 2-bit digital data to output
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wWhichBits=wBuf[7];    // 0: first two bits,  1: last two bits
   //---------------------- output -----------------------
                        // void
   switch (wID)
   {
        case 0x7016:  //  @AADO0?  for Bit0, Bit1
                      //  @AADO1?  for Bit2, Bit3
                      break;
        default: return(ModuleIdError);
   }


   // 7016,  @AADO0?,  @AADO1?
   if( wData>3 )
      return DataError;

   if( wWhichBits==0 )  // first 2 bits,  @AADO0?
   {
      //--- Bit0, Bit1 ---
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='D';
      szCmd[4]='O';
      szCmd[5]='0';
      szCmd[6]=wData+'0';
      szCmd[7]=0;
   }
   else   // last 2 bits,  @AADO0?
   {
      //--- Bit2, Bit3 ---
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='D';
      szCmd[4]='O';
      szCmd[5]='1';
      szCmd[6]=wData+'0';
      szCmd[7]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/* -------------------------------------------------------------------- */

 WORD CALLBACK DigitalOutReadBack(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
char wPort,wFlag,c1,c2;
WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
WORD A,B,C,D;
WORD wRecStrLen;

//---------------------- input -------------------------------------
wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
wAddr=wBuf[1];         // module address: 0x00 to 0xFF
wID=wBuf[2];           // module ID: 0x7050/60/66/67/42/43/44/55
wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
wTimeOut=wBuf[4];      // TimeOut constant: normal=100
wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                        // wBuf[5]=digital input data read back

switch (wID)
       {
       case 0x7055 : wID=0; break;  // 8 Channel DI/O,Add by 20080830
       case 0x7050 : wID=0; break;
       case 0x7060 : wID=0; break;
	     case 0x7063 : wID=0; break;
       case 0x7065 : wID=0; break;
       case 0x7066 : wID=0; break;
       case 0x7067 : wID=0; break;
       case 0x7042 : wID=1; break;
       case 0x7043 : wID=1; break;
	     case 0x7045 : wID=1; break;
       case 0x7044 : wID=0; break;
       default: return(ModuleIdError);
       }

   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='6';
   szCmd[4]=0;

   if (wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   wBuf[5]=0;
   if(wRetVal != NoError) return wRetVal;

   if (szRec[0] != '!') return ResultStrCheckError;

   if (wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;

   if (strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   if (wID == 0) // !OO
   {
      A=AsciiToHex(szRec[1]);
      B=AsciiToHex(szRec[2]);
      wBuf[5]=B+A*16;
   }
   else if (wID ==1)  // !OOOO
   {
      A=AsciiToHex(szRec[1]);
      B=AsciiToHex(szRec[2]);
      C=AsciiToHex(szRec[3]);
      D=AsciiToHex(szRec[4]);
      wBuf[5]=D+C*16+B*256+A*4096;
   }

   return(NoError);
}

/* -------------------------------------------------------------------- */

 WORD CALLBACK DigitalIn(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @19
{
   char  wPort,wFlag,c1,c2;
   WORD  wAddr,wRetVal,wID,wCheckSum,wTimeOut,mask,wT;
   WORD  A,B,C,D;
   WORD  wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/52/53/58/60/63/65/41/44/11/12/14/16/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   //wBuf[5]=16-bit digital input data

   switch (wID)
   {
       case 0x7050 : mask=0x007f; wID=1; break; /* IO    */  //dummp device
	     case 0x7055 : mask=0x00ff; wID=1; break; /* IO    */
       case 0x7052 : 
	     case 0x7058 :										 //2003/09/10 by sean
		   mask=0x00ff; wID=2; break; /* DI*8  */
       case 0x7053 :case 0x7051 : mask=0xffff; wID=0; break; /* DI*16 */
       case 0x7060 : mask=0x000f; wID=1; break; /* IO    */
       case 0x7063 : mask=0x00ff; wID=1; break; /* DI*8    */
       case 0x7065 : mask=0x000f; wID=0; break; /* DI*4    */
       case 0x7041 : mask=0x3fff; wID=0; break; /* DI*16 */
       case 0x7044 : mask=0x000f; wID=0; break; /* IO    */
       case 0x7011:
       case 0x7012:
       case 0x7014:
       case 0x7016:  wID=3;  break;
       default: return(ModuleIdError);
   }

   if ((wID==0)||(wID==1)||(wID==2)) //$016
   {
      szCmd[0]='$';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='6';
      szCmd[4]=0;
   }
   else   // 7011/7012/7014/7016 read Digital Input, @AADI
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='D';
      szCmd[4]='I';
      szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   wBuf[5]=0;
   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   switch( wID )
   {
      case 0:
      case 1:
      case 2:
         if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;  break;
      case 3:  // 7011/7012/7014/7016
         if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;  break;
   }

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   switch( wID )
   {
      case 0:
      case 1:
      case 2:
         A=AsciiToHex(szRec[1]);
         B=AsciiToHex(szRec[2]);
         C=AsciiToHex(szRec[3]);
         D=AsciiToHex(szRec[4]);

         if(wID==0) /* 16*DI */
            wT=D+C*16+B*256+A*4096;
         else if (wID==1) /* IO */
            wT=D+C*16;
         else if (wID==2) /* 8*DI */
            wT=B+A*16;
         wBuf[5]=wT&mask;
         break;
      case 3:
         A=AsciiToHex(szRec[7]);
         wBuf[5]=A;
         break;
   }

   return(NoError);
}


 WORD CALLBACK DigitalInCounterRead(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])//def @20
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wChannel,wT; //,wCounter;
   //WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen,mask;
   char cc[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/52/53/58/60/63/65/41/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wChannel=wBuf[5];      // Channel No.
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   // wBuf[7]= DigitalIn Counter Value

   switch (wID)
   {
       case 0x7050 : mask=0x007f;
                     if( wChannel>6 )
                        return InvalidateChannelNo;
                     break;
	     case 0x7055 :
       case 0x7052 : 
	     case 0x7058 :   //2003/09/10 by sean
					 mask=0x00ff;
                     if( wChannel>7 )
                        return InvalidateChannelNo;
                     break;
       case 0x7053 : case 0x7051 :mask=0xffff;
                     if( wChannel>15 )
                        return InvalidateChannelNo;
                     break;
       case 0x7060 : mask=0x000f;
                     if( wChannel>3 )
                        return InvalidateChannelNo;
                     break;
       case 0x7063 : mask=0x00ff;
                     if( wChannel>7 )
                        return InvalidateChannelNo;
                     break;
       case 0x7065 : mask=0x000f;
                     if( wChannel>3 )
                        return InvalidateChannelNo;
                     break;
       case 0x7041 : mask=0x3fff;
                     if( wChannel>13 )
                        return InvalidateChannelNo;
                     break;
       case 0x7044 : mask=0x000f;
                     if( wChannel>3 )
                        return InvalidateChannelNo;
                     break;
       default: return(ModuleIdError);
   }

   //***   #AAN  read DigitalIn Channel N's Counter Value
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]=HexToAscii(wChannel);
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   cc[0]=szRec[3];  cc[1]=szRec[4];  cc[2]=szRec[5];
   cc[3]=szRec[6];  cc[4]=szRec[7];  cc[5]=0;
   wBuf[7]=(WORD)(atoi(cc));

   return(NoError);
}


 WORD CALLBACK ClearDigitalInCounter(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wChannel; //,wCounter
   //WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen,mask;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/52/53/58/60/63/65/41/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wChannel=wBuf[5];      // Channel No.
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7050 : mask=0x007f;
                     if( wChannel>6 )
                        return InvalidateChannelNo;
                     break;
       case 0x7055 :
	     case 0x7052 : 
	     case 0x7058 : //2003/09/10 by sean
					 mask=0x00ff;
                     if( wChannel>7 )
                        return InvalidateChannelNo;
                     break;
       case 0x7053 :case 0x7051 : mask=0xffff;
                     if( wChannel>15 )
                        return InvalidateChannelNo;
                     break;
       case 0x7060 : mask=0x000f;
                     if( wChannel>3 )
                        return InvalidateChannelNo;
                     break;
       case 0x7063 : mask=0x00ff;
                     if( wChannel>7 )
                        return InvalidateChannelNo;
                     break;
       case 0x7065 : mask=0x000f;
                     if( wChannel>3 )
                        return InvalidateChannelNo;
                     break;
       case 0x7041 : mask=0x3fff;
                     if( wChannel>13 )
                        return InvalidateChannelNo;
                     break;
       case 0x7044 : mask=0x000f;
                     if( wChannel>3 )
                        return InvalidateChannelNo;
                     break;
       default: return(ModuleIdError);
   }

   //***   $AACN   Clear DigitalIn Channel N's Counter Value
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='C';
   szCmd[4]=HexToAscii(wChannel);//wChannel+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

 WORD CALLBACK DigitalInLatch(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wLatchStatus; //,wCounter
   WORD A,B,C,D; //,E,F,G,H;
   WORD wRecStrLen,mask;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/52/53/58/60/63/65/41/44/55
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wLatchStatus=wBuf[5];  // 0: select to latch low
                           // 1: select to latch high
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   //  wBuf[7]: the latched data

   switch (wID)
   {
       case 0x7050 : mask=0x007f; wID=1; break; /* IO*7 */
       case 0x7052 : 
	     case 0x7055 :   /* DI*8  */
	     case 0x7058 :
		      mask=0xff;   wID=3; break; /* DI*8  */
       case 0x7053 :case 0x7051: mask=0xffff; wID=0; break; /* DI*16 */
       case 0x7060 : mask=0x000f; wID=1; break; /* IO    */
       case 0x7063 : mask=0x00ff; wID=2; break; /* DI*8    */
       case 0x7065 : mask=0x000f; wID=0; break; /* DI*4    */
       case 0x7041 : mask=0x3fff; wID=0; break; /* DI*16 */
       case 0x7044 : mask=0x000f; wID=0; break; /* IO    */
       default: return(ModuleIdError);
   }

   //***   $AALS  S:0 latch low  S:1 latch high
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='L';
   if( wLatchStatus==0 )
      szCmd[4]='0';
   else
      szCmd[4]='1';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=7; else wRecStrLen=9;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   switch( wID )
   {
      case 0:
      case 1:
      case 2:
      case 3:
         A=AsciiToHex(szRec[1]);
         B=AsciiToHex(szRec[2]);
         C=AsciiToHex(szRec[3]);
         D=AsciiToHex(szRec[4]);

         if(wID==0) /* 16*DI */
            wT=D+C*16+B*256+A*4096;
         else if (wID==1) /* IO */
            wT=D+C*16;
         else if (wID==2) /* 8*DI */
            wT=D+C*16;
         else if (wID==3)
            wT=B+A*16;
         wBuf[7]=wT&mask;
         break;
    }

   return(NoError);
}

 WORD CALLBACK ClearDigitalInLatch(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT; //,wLatchStatus,wCounter;
   //WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen; //,mask;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7050/52/53/60/63/65/41/44
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wLatchStatus=wBuf[5];  // 0: select to latch low
                              // 1: select to latch high
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   //---   $AAC  ---
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='C';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


//---------------------- input -------------------------------------
// wPort   :             // RS-232 port number: 1/2/3/4/..../255
// wTotal  :             // number of modules to access
// wTimeOut:             // TimeOut constant: normal=100
// wAddr[?]:             // module address: 0x00 to 0xFF
// wID[?]  :             // module ID: 0x7011/12/13/14/17/18/19
// wConfig[?]:           // wConfig=08/09/0A/0B/0C/0D for wID=0x7017
// wCheckSum[?]:         // checksum: 0=disable, 1=enable
//---------------------- output ------------------------------------
                         // fBuf[?]=analog values

WORD CALLBACK NetworkAnalogIn(WORD wPort, WORD wTotal, WORD wTimeOut, WORD FAR wAddr[],
     WORD FAR wID[], WORD FAR wConfig[], WORD FAR wCheckSum[], float FAR fBuf[])
{
//char wFlag;  // cc[80],c1,c2;
WORD wRetVal,i,j,wType;
WORD wBase;    // wRecStrLen,
WORD wTable[20];
float fTable[20];

for (i=0; i<wTotal; i++)
{
wBase=i*8;

switch (wID[i])
       {
       case 0x7011 :
       case 0x7012 :
       case 0x7013 :
       case 0x7014 : wType=0; break;
       case 0x7017 : wType=2; break;
       case 0x7018 : 
	   case 0x7019 : wType=1; break;
       default: return(ModuleIdError);
       }

if (wType == 0)
   {
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4/..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7011/12/13/14/17/18/19
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // channel number for multi-channel
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value
   wTable[0]=wPort;
   wTable[1]=wAddr[i];
   wTable[2]=wID[i];
   wTable[3]=wCheckSum[i];
   wTable[4]=wTimeOut;
   wTable[6]=0;
   wRetVal=AnalogIn(wTable, fTable, szCmd, szRec);

   if (wRetVal==NoError) fBuf[wBase]=fTable[0];
   else return(FunctionError);
   }
else if (wType == 1)
   {
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4/..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7017/18/19
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0..7]=analog input value

   wTable[0]=wPort;
   wTable[1]=wAddr[i];
   wTable[2]=wID[i];
   wTable[3]=wCheckSum[i];
   wTable[4]=wTimeOut;
   wTable[6]=0;
   wRetVal=AnalogIn8(wTable, fTable, szCmd, szRec);

   if (wRetVal==NoError)
      for (j=0; j<8; j++) fBuf[wBase+j]=fTable[j];
   else return(FunctionError);
   }
else if (wType == 2)
   {
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4/..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7017
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0..7]=analog input value

   wTable[0]=wPort;
   wTable[1]=wAddr[i];
   wTable[2]=wConfig[i];
   wTable[3]=wCheckSum[i];
   wTable[4]=wTimeOut;
   wTable[6]=0;
   wRetVal=In8_7017(wTable, fTable, szCmd, szRec);

   if (wRetVal==NoError)
      for (j=0; j<8; j++) fBuf[wBase+j]=fTable[j];
   else return(FunctionError);
   }

}
return(NoError);
}

/* -------------------------------------------------------------------- */

//---------------------- input -------------------------------------
// wPort   :             // RS-232 port number: 1/2/3/4/..../255
// wTotal  :             // number of modules to access
// wTimeOut:             // TimeOut constant: normal=100
// wAddr[?]:             // module address: 0x00 to 0xFF
// wID[?]  :             // module ID: 0x7050/52/53/60/41/44
// wConfig[?]:           // reserved
// wCheckSum[?]:         // checksum: 0=disable, 1=enable
//---------------------- output ------------------------------------
                         // wBuf[?]=digital input values

 WORD CALLBACK NetworkDigitalIn(WORD wPort, WORD wTotal, WORD wTimeOut, WORD FAR wAddr[],
     WORD FAR wID[], WORD FAR wConfig[], WORD FAR wCheckSum[], WORD FAR wBuf[])
{
// char  wFlag,cc[80];
WORD wRetVal,i;  //,wType;
WORD wTable[20];
float fTable[20];

for (i=0; i<wTotal; i++)
{
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4/..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/52/53/60/41/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // wBuf[5]=digital input data
   wTable[0]=wPort;
   wTable[1]=wAddr[i];
   wTable[2]=wID[i];
   wTable[3]=wCheckSum[i];
   wTable[4]=wTimeOut;
   wTable[6]=0;
   wRetVal=DigitalIn(wTable, fTable, szCmd, szRec);

   if (wRetVal==NoError) wBuf[i]=wTable[5];
   else return(FunctionError);
}
return(NoError);
}


/*-------- 7011 Thermocouple Open Detection ---------*/
 WORD CALLBACK ThermocoupleOpen_7011(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5];            // Don't Care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   // wBuf[5]   0: Close     1: Open

   if( wID!=0x7011 )
   {
      return(ModuleIdError);
   }

   szCmd[0]='$';    // $AAB
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='B';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';

   return(NoError);
}

/*-------- 7011/7012/7014/7016 Enable Alarm ---------*/
 WORD CALLBACK EnableAlarm(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wAlarmMode,wT;
   WORD   wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wAlarmMode=wBuf[5];    // 0: Momentary,  1: Latch
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 :  break;
       default: return(ModuleIdError);
   }

   szCmd[0]='@';    // @AAEAT
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='E';
   szCmd[4]='A';
   if( wAlarmMode==0 )
      szCmd[5]='M';
   else
      szCmd[5]='L';
   szCmd[6]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*-------- 7011/7012/7014/7016/7005 Disable Alarm ---------*/
 WORD CALLBACK DisableAlarm(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,channel,wFlag,c1,c2; //alarmMode:H:high alarm and L:low alarm
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen,wHiLo;
   //float  fValue;
   //char   OutCh[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo = wBuf[5];   // wHiLo:1 : H:high alarm and 0: L:low alarm
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   channel=(char)wBuf[7] & 0x0f ;
   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 :  
	   case 0x7005 :
	   break;
       default: return(ModuleIdError);
   }
   if(wID==0x7005)//07/10/2004
   {
		szCmd[0]='@';    // @AADHCi @AADLCi
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		szCmd[3]='D';
		if( wHiLo==0 )   //  Low Alarm
		{
			szCmd[4]='L';
		}
		else
		{
			szCmd[4]='H';		
		}
		szCmd[5]='C';
		szCmd[6]=HexToAscii(channel);
		szCmd[7]=0;
   }
   else
   {
		szCmd[0]='@';    // @AADA
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		szCmd[3]='D';
		szCmd[4]='A';
		szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- 7011/7012/7014/7016/7005 Clear Latch Alarm ---------*/
 WORD CALLBACK ClearLatchAlarm(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,channel,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen,wHiLo;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo = wBuf[5];   // alarmMode:H:high alarm and L:low alarm
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   channel=(char)wBuf[7] & 0x0f ;

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 :
	   case 0x7005 :
	   break;
       default: return(ModuleIdError);
   }
   if(wID==0x7005)
   {
		szCmd[0]='@';    // @AACA
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		szCmd[3]='C';
		if( wHiLo==0 )   //  Low Alarm
		{
			szCmd[4]='L';
		}
		else
		{
			szCmd[4]='H';		
		}
		szCmd[5]='C';
		szCmd[6]=HexToAscii(channel);
		szCmd[7]=0;
   }
   else
   {
		szCmd[0]='@';    // @AACA
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		szCmd[3]='C';
		szCmd[4]='A';
		szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- 7011/7012/7014/16/7005 Set Hi/Lo Alarm Limit Value ---------*/
 /* -------7005 can set alarm limit and alarm map channel of DO output-----*/
 WORD CALLBACK SetAlarmLimitValue(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2,channel,DOChannel,alarmType;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wHiLo,wT,wRangeCode;
   WORD   wRecStrLen,wPoints;
   float  fValue;
   char   OutCh[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Lo Alarm    1: High Alarm
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wRangeCode=wBuf[7];
   channel=(char)wBuf[8] & 0x0f; //for 7005 AI channel
   DOChannel=(char)wBuf[9] & 0x06;// for 7005 alarm map to DO channel
   alarmType=(char)wBuf[10];    //alarmType=1: ==>'M' for momentary alarm 2==>'L' for latch alarm
   fValue=fBuf[0];        // Alarm Limit Value

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 :  
       case 0x7005 :
	   break;
       default: return(ModuleIdError);
   }

//   sprintf(OutCh,"%+07.2f",fValue);

   if( wID==7016 )
      if( wRangeCode>6 )
         return ExceedInputRange;

   switch( wRangeCode )
   {
      case 0:
           /*if( fValue>15.00 )
              return ExceedInputRange;
           if( fValue<-15.00 )
              return UnderInputRange;
           */
		   wPoints=3;
           break;
      case 1:
           /*if( fValue>50.00 )
              return ExceedInputRange;
           if( fValue<-50.00 )
              return UnderInputRange;*/
           wPoints=3;
           break;
      case 2:
           /*if( fValue>100.00 )
              return ExceedInputRange;
           if( fValue<-100.00 )
              return UnderInputRange;*/
           wPoints=2;
           break;
      case 3:
           /*if( fValue>500.00 )
              return ExceedInputRange;
           if( fValue<-500.00 )
              return UnderInputRange;*/
           wPoints=2;
           break;
      case 4:
           /*if( fValue>1.00 )
              return ExceedInputRange;
           if( fValue<-1.00 )
              return UnderInputRange;*/
           wPoints=4;
           break;
      case 5:
           /*if( fValue>2.50 )
              return ExceedInputRange;
           if( fValue<-2.50 )
              return UnderInputRange;*/
           wPoints=4;
           break;
      case 6:
           /*if( fValue>20.00 )
              return ExceedInputRange;
           if( fValue<-20.00 )
              return UnderInputRange;*/
           wPoints=3;
           break;
      case 7:
           /*if( fValue>500.00 )
              return ExceedInputRange;
           if( fValue<-500.00 )
              return UnderInputRange;*/
           wPoints=2;
           break;
      case 8:
           /*if( fValue>10.00 )
              return ExceedInputRange;
           if( fValue<-10.00 )
              return UnderInputRange;*/
           wPoints=3;
           break;
      case 9:
           /*if( fValue>5.00 )
              return ExceedInputRange;
           if( fValue<-5.00 )
              return UnderInputRange;*/
           wPoints=4;
           break;
      case 0x0A:
           /*if( fValue>1.00 )
              return ExceedInputRange;
           if( fValue<-1.00 )
              return UnderInputRange;*/
           wPoints=4;
           break;
      case 0x0B:
           /*if( fValue>500.00 )
              return ExceedInputRange;
           if( fValue<-500.00 )
              return UnderInputRange;*/
           wPoints=2;
           break;
      case 0x0C:
           /*if( fValue>150.00 )
              return ExceedInputRange;
           if( fValue<-150.00 )
              return UnderInputRange;*/
           wPoints=2;
           break;
      case 0x0D:
           /*if( fValue>20.00 )
              return ExceedInputRange;
           if( fValue<-20.00 )
              return UnderInputRange;*/
           wPoints=3;
           break;
      case 0x0E:
           /*if( fValue>760.00 )
              return ExceedInputRange;
           if( fValue<0.00 )
              return UnderInputRange;*/
           wPoints=3;
           break;
      case 0x0F:
           /*if( fValue>1320.00 )
              return ExceedInputRange;
           if( fValue<0.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
      case 0x10:
           /*if( fValue>400.00 )
              return ExceedInputRange;
           if( fValue<-100.00 )
              return UnderInputRange;*/
           wPoints=2;
           break;
      case 0x11:
           /*if( fValue>1000.00 )
              return ExceedInputRange;
           if( fValue<0.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
      case 0x12:
           /*if( fValue>1750.00 )
              return ExceedInputRange;
           if( fValue<500.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
      case 0x13:
           /*if( fValue>1750.00 )
              return ExceedInputRange;
           if( fValue<500.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
      case 0x14:
           /*if( fValue>1800.00 )
              return ExceedInputRange;
           if( fValue<500.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
      case 0x15:
           /*if( fValue>1300.00 )
              return ExceedInputRange;
           if( fValue<-270.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
      case 0x16:
           /*if( fValue>2320.00 )
              return ExceedInputRange;
           if( fValue<0.00 )
              return UnderInputRange;*/
           wPoints=1;
           break;
	  case 0x60: 
	  case 0x61:
	  case 0x62:
		  wPoints=2;
		  break;
       default: return(ModuleIdError);
   }

   switch( wPoints )
   {
      case 1:
         sprintf(OutCh,"%+07.1f",fValue);
         break;
      case 2:
         sprintf(OutCh,"%+07.2f",fValue);
         break;
      case 3:
         sprintf(OutCh,"%+07.3f",fValue);
         break;
      case 4:
         sprintf(OutCh,"%+07.4f",fValue);
         break;
      default: return(ModuleIdError);
   }
	
   if(wID==0x7005)
   {
		szCmd[0]='@';    // @AALO(data): set low alarm limit
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		if( wHiLo==0 )   //  Low Alarm
		{
			szCmd[3]='L';
			szCmd[4]='O';
		}
		else
		{
			szCmd[3]='H';
			szCmd[4]='I';
		}

		szCmd[5]=OutCh[0];
		szCmd[6]=OutCh[1];
		szCmd[7]=OutCh[2];
		szCmd[8]=OutCh[3];
		szCmd[9]=OutCh[4];
		szCmd[10]=OutCh[5];
		szCmd[11]=OutCh[6];
		szCmd[12]='C';
		szCmd[13]=HexToAscii(channel);

		if (alarmType==1)
			szCmd[14]='M';
		else if (alarmType==2)
			szCmd[14]='L';
		
		szCmd[15]='O';
		szCmd[16]=HexToAscii(DOChannel);
		szCmd[17]=0;
   }
   else
   {
                    // @AAHI(data): set high alarm limit
		szCmd[0]='@';    // @AALO(data): set low alarm limit
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		if( wHiLo==0 )   //  Low Alarm
		{
			szCmd[3]='L';
			szCmd[4]='O';
		}
		else
		{
			szCmd[3]='H';
			szCmd[4]='I';
		}

		szCmd[5]=OutCh[0];
		szCmd[6]=OutCh[1];
		szCmd[7]=OutCh[2];
		szCmd[8]=OutCh[3];
		szCmd[9]=OutCh[4];
		szCmd[10]=OutCh[5];
		szCmd[11]=OutCh[6];
		szCmd[12]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*-------- 7011/7012/7014/7016 Read Hi/Lo Alarm Limit Value ---------*/
 WORD CALLBACK ReadAlarmLimitValue(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2,channel;
   WORD   i,wAddr,wRetVal,wID,wCheckSum,wTimeOut,wHiLo,wT;
   WORD   wRecStrLen;
   //float  fValue;
   char   Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Lo Alarm    1: High Alarm
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   channel=(char)wBuf[7] & 0x0f;

   //---------------------- output -------------------------------------
   //fValue=fBuf[0];      // Alarm Limit Value
   //wBuf[8]              // alarm Type 0: Disable 1: momentary 2: latch
   //wBuf[9]			  // alarm to DO channel

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 : 
	   case 0x7005:
	   break;
       default: return(ModuleIdError);
   }

   if(wID==0x7005)
   {
	   // @AARH(data): read high alarm limit
		szCmd[0]='@';    // @AARL(data): read low  alarm limit
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		szCmd[3]='R';
		if( wHiLo==0 )   //  Low Alarm
		{			
			szCmd[4]='L';
		}
		else
		{		
			szCmd[4]='H';
		}
		szCmd[5]=HexToAscii(channel);
		szCmd[6]=0;
   }
   else
   {
	   // @AARH(data): read high alarm limit
		szCmd[0]='@';    // @AARL(data): read low  alarm limit
		c2=(char)(wAddr&0x0f);
		wAddr=wAddr/16;
		c1=(char)(wAddr&0x0f);
		szCmd[1] = HexToAscii(c1);
		szCmd[2] = HexToAscii(c2);
		szCmd[3]='R';
		if( wHiLo==0 )   //  Low Alarm
		{			
			szCmd[4]='L';
		}
		else
		{		
			szCmd[4]='H';
		}
		szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;
   if(wID==0x7005)
   {
		if(wCheckSum==0) wRecStrLen=13; else wRecStrLen=15;
   }
   else
   {
	   if(wCheckSum==0) wRecStrLen=10; else wRecStrLen=12;
   }

//   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   for(i=0; i<7; i++)
       Buffer[i]=szRec[i+3];
   Buffer[7]=0;
   fBuf[0]=(float) atof(Buffer);
   wBuf[8]=szRec[10]; //alarm type
   wBuf[9]=szRec[12]; //alarm to DO map

   return(NoError);
}

/*-------- 7011/7012/7014/7016 Read Event Counter --------*/
 WORD CALLBACK ReadEventCounter(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;
   int    i;
   char   szTemp[6];

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wCount=wBuf[5];     // don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ----------------------------------
   // wCount=wBuf[7];     // The Event Counter Value

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 :  break;
       default: return(ModuleIdError);
   }

   szCmd[0]='@';    // @AARE
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='R';
   szCmd[4]='E';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   for(i=0; i<5; i++)
   {
      szTemp[i]=szRec[i+3];
   }   
   szTemp[5]=0;
   wBuf[7]=(WORD)(atoi(szTemp));

   return(NoError);
}


/*-------- 7011/7012/7014/7016 Clear Event Counter --------*/
 WORD CALLBACK ClearEventCounter(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wCount=wBuf[5];     // don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 :  break;
       default: return(ModuleIdError);
   }

   szCmd[0]='@';    // @AACE
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='C';
   szCmd[4]='E';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- Set LED channel Display for 7033 --------*/
 WORD CALLBACK SetLedDisplay_7033(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wChannel;
   WORD   wRecStrLen;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7033
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wChannel=wBuf[5];      // LED channel to set (0~2 set LED to module,3 to host)
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7033 :  break;
       default: return(ModuleIdError);
   }

   if( wChannel>2 )
      return(AdChannelError);

   szCmd[0]='$';    // $AA8N
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='8';
   szCmd[4]=wChannel+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- Set LED channel Display for 7016, 7033 --------*/
 WORD CALLBACK SetLedDisplay(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wChannel;
   WORD   wRecStrLen;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7016, 0x7013,0x7033
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wChannel=wBuf[5];      // LED channel to set
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7016 :  wID=0;
                      break;
       case 0x7013 :  wID=1;         //add by cindy 20081225
                      break;
       case 0x7033 :  wID=1;
                      break;
       default: return(ModuleIdError);
   }


   if( wID==0 )  // 7016
   {
       if( wChannel>1 )
           return(AdChannelError);
        szCmd[0]='$';    // $AA3N
        c2=(char)(wAddr&0x0f);
        wAddr=wAddr/16;
        c1=(char)(wAddr&0x0f);
        szCmd[1] = HexToAscii(c1);
        szCmd[2] = HexToAscii(c2);
        szCmd[3]='3';
        szCmd[4]=wChannel+'0';
        szCmd[5]=0;
   }
   else
   {
       if( wChannel>2 ) //7013 ,7033
          return(AdChannelError);
        szCmd[0]='$';    // $AA8N
        c2=(char)(wAddr&0x0f);
        wAddr=wAddr/16;
        c1=(char)(wAddr&0x0f);
        szCmd[1] = HexToAscii(c1);
        szCmd[2] = HexToAscii(c2);
        szCmd[3]='8';
        szCmd[4]=wChannel+'0';
        szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- Get active LED channel Display for 7013, 7033 --------*/
 WORD CALLBACK GetLedDisplay(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];                 // module ID: 0x7013, 0x7033
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wChannel=wBuf[5];   // which LED channel to display
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7016 :  wID=0;  break;
       case 0x7013 :  wID=1;  break;   //by cindy 20081225
       case 0x7033 :  wID=1;  break;
       default: return(ModuleIdError);
   }

   if( wID==0 )
   {
      szCmd[0]='$';    // 7016, $AA3
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='3';
      szCmd[4]=0;
   }
   else
   {
      szCmd[0]='$';    // 7033, $AA8
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='8';
      szCmd[4]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';

   return(NoError);
}


/*******************************************************/
/*-------- setup linear mapping for 7014/7016 ---------*/
/*  making a linear mapping from [a,b] to [c,d]        */
/*  where:  fBuf[0]=a,  fBuf[1]=b                    */
/*          fBuf[2]=c,  fBuf[3]=d                    */
/*******************************************************/
 WORD CALLBACK SetupLinearMapping(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wHiLo,wT,wRangeCode;
   WORD   wRecStrLen,wPoints;
   float  fSourceLow,fSourceHigh,fTargetLow,fTargetHigh;
   char   OutCh[20];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7014, 7016
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Lo Alarm    1: High Alarm
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wRangeCode=wBuf[7];
   //fValue=fBuf[0];      // Alarm Limit Value
   fSourceLow  = fBuf[0];
   fSourceHigh = fBuf[1];
   fTargetLow  = fBuf[2];
   fTargetHigh = fBuf[3];

   switch (wID)
   {
       case 0x7014 :
       case 0x7016 :  break;
       default: return(ModuleIdError);
   }

//   sprintf(OutCh,"%+07.2f",fValue);

   if( wID==7016 )
      if( wRangeCode>6 )
         return ExceedInputRange;

   if( fSourceLow>fSourceHigh )
      return  DataError;

   switch( wRangeCode )
   {
      case 0:
           if( fSourceHigh>15.00 )
              return ExceedInputRange;
           if( fSourceLow<-15.00 )
              return UnderInputRange;
           wPoints=3;
           break;
      case 1:
           if( fSourceHigh>50.00 )
              return ExceedInputRange;
           if( fSourceLow<-50.00 )
              return UnderInputRange;
           wPoints=3;
           break;
      case 2:
           if( fSourceHigh>100.00 )
              return ExceedInputRange;
           if( fSourceLow<-100.00 )
              return UnderInputRange;
           wPoints=2;
           break;
      case 3:
           if( fSourceHigh>500.00 )
              return ExceedInputRange;
           if( fSourceLow<-500.00 )
              return UnderInputRange;
           wPoints=2;
           break;
      case 4:
           if( fSourceHigh>1.00 )
              return ExceedInputRange;
           if( fSourceLow<-1.00 )
              return UnderInputRange;
           wPoints=4;
           break;
      case 5:
           if( fSourceHigh>2.50 )
              return ExceedInputRange;
           if( fSourceLow<-2.50 )
              return UnderInputRange;
           wPoints=4;
           break;
      case 6:
           if( fSourceHigh>20.00 )
              return ExceedInputRange;
           if( fSourceLow<-20.00 )
              return UnderInputRange;
           wPoints=3;
           break;
      case 7:
           if( fSourceHigh>500.00 )
              return ExceedInputRange;
           if( fSourceLow<-500.00 )
              return UnderInputRange;
           wPoints=2;
           break;
      case 8:
           if( fSourceHigh>10.00 )
              return ExceedInputRange;
           if( fSourceLow<-10.00 )
              return UnderInputRange;
           wPoints=3;
           break;
      case 9:
           if( fSourceHigh>5.00 )
              return ExceedInputRange;
           if( fSourceLow<-5.00 )
              return UnderInputRange;
           wPoints=4;
           break;
      case 0x0A:
           if( fSourceHigh>1.00 )
              return ExceedInputRange;
           if( fSourceLow<-1.00 )
              return UnderInputRange;
           wPoints=4;
           break;
      case 0x0B:
           if( fSourceHigh>500.00 )
              return ExceedInputRange;
           if( fSourceLow<-500.00 )
              return UnderInputRange;
           wPoints=2;
           break;
      case 0x0C:
           if( fSourceHigh>150.00 )
              return ExceedInputRange;
           if( fSourceLow<-150.00 )
              return UnderInputRange;
           wPoints=2;
           break;
      case 0x0D:
           if( fSourceHigh>20.00 )
              return ExceedInputRange;
           if( fSourceLow<-20.00 )
              return UnderInputRange;
           wPoints=3;
           break;
      case 0x0E:
           if( fSourceHigh>760.00 )
              return ExceedInputRange;
           if( fSourceLow<0.00 )
              return UnderInputRange;
           wPoints=3;
           break;
      case 0x0F:
           if( fSourceHigh>1320.00 )
              return ExceedInputRange;
           if( fSourceLow<0.00 )
              return UnderInputRange;
           wPoints=1;
           break;
      case 0x10:
           if( fSourceHigh>400.00 )
              return ExceedInputRange;
           if( fSourceLow<-100.00 )
              return UnderInputRange;
           wPoints=2;
           break;
      case 0x11:
           if( fSourceHigh>1000.00 )
              return ExceedInputRange;
           if( fSourceLow<0.00 )
              return UnderInputRange;
           wPoints=1;
           break;
      case 0x12:
           if( fSourceHigh>1750.00 )
              return ExceedInputRange;
           if( fSourceLow<500.00 )
              return UnderInputRange;
           wPoints=1;
           break;
      case 0x13:
           if( fSourceHigh>1750.00 )
              return ExceedInputRange;
           if( fSourceLow<500.00 )
              return UnderInputRange;
           wPoints=1;
           break;
      case 0x14:
           if( fSourceHigh>1800.00 )
              return ExceedInputRange;
           if( fSourceLow<500.00 )
              return UnderInputRange;
           wPoints=1;
           break;
      case 0x15:
           if( fSourceHigh>1300.00 )
              return ExceedInputRange;
           if( fSourceLow<-270.00 )
              return UnderInputRange;
           wPoints=1;
           break;
      case 0x16:
           if( fSourceHigh>2320.00 )
              return ExceedInputRange;
           if( fSourceLow<0.00 )
              return UnderInputRange;
           wPoints=1;
           break;
       default: return(ModuleIdError);
   }

   switch( wPoints )
   {
      case 1:
         sprintf(OutCh,"%+07.1f%+07.1f",fSourceLow,fSourceHigh);
         break;
      case 2:
         sprintf(OutCh,"%+07.2f%+07.2f",fSourceLow,fSourceHigh);
         break;
      case 3:
         sprintf(OutCh,"%+07.3f%+07.3f",fSourceLow,fSourceHigh);
         break;
      case 4:
         sprintf(OutCh,"%+07.4f%+07.4f",fSourceLow,fSourceHigh);
         break;
      default: return(ModuleIdError);
   }


   if( wBuf[2]==0x7014 )  // $AA6(data*2)
   {
       szCmd[0]='$';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='6';
   }
   else    // 7016, @AA6(data*2)
   {
       szCmd[0]='@';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='6';
   }
   szCmd[4]=OutCh[0];
   szCmd[5]=OutCh[1];
   szCmd[6]=OutCh[2];
   szCmd[7]=OutCh[3];
   szCmd[8]=OutCh[4];
   szCmd[9]=OutCh[5];
   szCmd[10]=OutCh[6];

   szCmd[11]=OutCh[7];
   szCmd[12]=OutCh[8];
   szCmd[13]=OutCh[9];
   szCmd[14]=OutCh[10];
   szCmd[15]=OutCh[11];
   szCmd[16]=OutCh[12];
   szCmd[17]=OutCh[13];
   szCmd[18]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   Sleep(100);
   /******************************************************************/
   if( fTargetLow>19999.0 )
      return ExceedInputRange;
   else if( fTargetLow>10000.0 )
      sprintf(OutCh,"%+07.1f",fTargetLow);
   else if( fTargetLow>1000.0 )
      sprintf(OutCh,"%+07.1f",fTargetLow);
   else if( fTargetLow>100.0 )
      sprintf(OutCh,"%+07.2f",fTargetLow);
   else if( fTargetLow>10.0 )
      sprintf(OutCh,"%+07.3f",fTargetLow);
   else if( fTargetLow>1.0 )
      sprintf(OutCh,"%+07.4f",fTargetLow);
   else if( fTargetLow>0.0 )
      sprintf(OutCh,"%+07.5f",fTargetLow);
   else if( fTargetLow>-1.0 )
      sprintf(OutCh,"%+07.4f",fTargetLow);
   else if( fTargetLow>-10.0 )
      sprintf(OutCh,"%+07.3f",fTargetLow);
   else if( fTargetLow>-100.0 )
      sprintf(OutCh,"%+07.2f",fTargetLow);
   else if( fTargetLow>-1000.0 )
      sprintf(OutCh,"%+07.1f",fTargetLow);
   else if( fTargetLow>-19999.0 )
      sprintf(OutCh,"%+07.1f",fTargetLow);
   else
      return UnderInputRange;

   if( fTargetHigh>19999.0 )
      return ExceedInputRange;
   else if( fTargetHigh>10000.0 )
      sprintf(OutCh+7,"%+07.1f",fTargetHigh);
   else if( fTargetHigh>1000.0 )
      sprintf(OutCh+7,"%+07.1f",fTargetHigh);
   else if( fTargetHigh>100.0 )
      sprintf(OutCh+7,"%+07.2f",fTargetHigh);
   else if( fTargetHigh>10.0 )
      sprintf(OutCh+7,"%+07.3f",fTargetHigh);
   else if( fTargetHigh>1.0 )
      sprintf(OutCh+7,"%+07.4f",fTargetHigh);
   else if( fTargetHigh>0.0 )
      sprintf(OutCh+7,"%+07.5f",fTargetHigh);
   else if( fTargetHigh>-1.0 )
      sprintf(OutCh+7,"%+07.4f",fTargetHigh);
   else if( fTargetHigh>-10.0 )
      sprintf(OutCh+7,"%+07.3f",fTargetHigh);
   else if( fTargetHigh>-100.0 )
      sprintf(OutCh+7,"%+07.2f",fTargetHigh);
   else if( fTargetHigh>-1000.0 )
      sprintf(OutCh+7,"%+07.1f",fTargetHigh);
   else if( fTargetHigh>-19999.0 )
      sprintf(OutCh+7,"%+07.1f",fTargetHigh);
   else
      return UnderInputRange;

   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   if( wBuf[2]==0x7014 )  // $AA7(data*2)
   {
       szCmd[0]='$';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='7';
   }
   else    // 7016, @AA7(data*2)
   {
       szCmd[0]='@';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='7';
   }
   szCmd[4]=OutCh[0];
   szCmd[5]=OutCh[1];
   szCmd[6]=OutCh[2];
   szCmd[7]=OutCh[3];
   szCmd[8]=OutCh[4];
   szCmd[9]=OutCh[5];
   szCmd[10]=OutCh[6];

   szCmd[11]=OutCh[7];
   szCmd[12]=OutCh[8];
   szCmd[13]=OutCh[9];
   szCmd[14]=OutCh[10];
   szCmd[15]=OutCh[11];
   szCmd[16]=OutCh[12];
   szCmd[17]=OutCh[13];
   szCmd[18]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   Sleep(100);

   return(NoError);
}

/*-------- Enable Linear Mapping for 7014/7016 ---------*/
 WORD CALLBACK EnableLinearMapping(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7014, 0x7016
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5] don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7014 :
       case 0x7016 :  break;
       default: return(ModuleIdError);
   }

   if( wID==0x7014 )
   {
      szCmd[0]='$';    // $AAAV
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='A';
      szCmd[4]='1';
      szCmd[5]=0;
   }
   else
   {
      szCmd[0]='@';    // @AAAV
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='A';
      szCmd[4]='1';
      szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- Disable Linear Mapping for 7014/7016 ---------*/
 WORD CALLBACK DisableLinearMapping(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7014, 0x7016
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wBuf[5] don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7014 :
       case 0x7016 :  break;
       default: return(ModuleIdError);
   }

   if( wID==0x7014 )
   {
      szCmd[0]='$';    // $AAAV
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='A';
      szCmd[4]='0';
      szCmd[5]=0;
   }
   else
   {
      szCmd[0]='@';    // @AAAV
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='A';
      szCmd[4]='0';
      szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- reading the linear mapping status for 7014/7016 --------*/
 WORD CALLBACK ReadLinearMappingStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7014, 0x7016
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wChannel=wBuf[5];   // which LED channel to display
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7014 :  wID=0;  break;
       case 0x7016 :  wID=1;  break;
       default: return(ModuleIdError);
   }

   if( wID==0 )
   {
      szCmd[0]='$';    // 7014, $AAA
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='A';
      szCmd[4]=0;
   }
   else     // 7016, @AAA
   {
      szCmd[0]='@';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='A';
      szCmd[4]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';

   return(NoError);
}

/*----- Reading Source Value of Linear Mapping for 7014/7016 -----*/
 WORD CALLBACK ReadSourceValueOfLM(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2,Buffer[10];
   WORD   wAddr,wRetVal,i,j,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];  // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];        // module address: 0x00 to 0xFF
   wID=wBuf[2];          // module ID: 0x7014/16
   wCheckSum=wBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];     // TimeOut constant: normal=100
   wFlag=(char)wBuf[6];  // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   // fBuf[0]= Low Source Value
   // fBuf[1]= High Source Value

   switch (wID)
   {
      case 0x7014 :
      case 0x7016 : break;
      default: return(ModuleIdError);
   }

   if( wID==0x7014 )
   {
      szCmd[0]='$';           //$AA3
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3] = '3';
      szCmd[4]=0;
   }
   else
   {
      szCmd[0]='@';           //@AA6
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3] = '6';
      szCmd[4]=0;
   }

   if( wFlag==1 )
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   for (j=0; j<7; j++) fBuf[j]=(float)1.1;
   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=17; else wRecStrLen=19;

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   for(i=0; i<7; i++)
      Buffer[i]=szRec[i+3];
   Buffer[7]=0;
   fBuf[0]=(float)atof(Buffer);

   for(i=0; i<7; i++)
      Buffer[i]=szRec[i+10];
   Buffer[7]=0;
   fBuf[1]=(float)atof(Buffer);

   return(NoError);
}

/*----- Reading Target Value of Linear Mapping for 7014/7016 -----*/
 WORD CALLBACK ReadTargetValueOfLM(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,Buffer[10],c1,c2;
   WORD   wAddr,wRetVal,i,j,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];  // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];        // module address: 0x00 to 0xFF
   wID=wBuf[2];          // module ID: 0x7014/16
   wCheckSum=wBuf[3];    // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];     // TimeOut constant: normal=100
   wFlag=(char)wBuf[6];  // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   // fBuf[0]= Low Target Value
   // fBuf[1]= High Target Value

   switch (wID)
   {
      case 0x7014 :
      case 0x7016 : break;
      default: return(ModuleIdError);
   }

   if( wID==0x7014 )
   {
      szCmd[0]='$';           //$AA5
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3] = '5';
      szCmd[4]=0;
   }
   else
   {
      szCmd[0]='@';           //@AA7
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3] = '7';
      szCmd[4]=0;
   }

   if( wFlag==1 )
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   for (j=0; j<7; j++) fBuf[j]=(float)0.0;
   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=17; else wRecStrLen=19;

   if(strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   for(i=0; i<7; i++)
      Buffer[i]=szRec[i+3];
   Buffer[7]=0;
   fBuf[0]=(float)atof(Buffer);

   for(i=0; i<7; i++)
      Buffer[i]=szRec[i+10];
   Buffer[7]=0;
   fBuf[1]=(float)atof(Buffer);

   return(NoError);
}


/*-------- Get active LED channel Display for 7033 --------*/
 WORD CALLBACK GetLedDisplay_7033(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD   wRecStrLen;

   //---------------------- input -----------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7033
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   // wChannel=wBuf[5];   // which LED channel to display
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   switch (wID)
   {
       case 0x7033 :  break;
       default: return(ModuleIdError);
   }

   szCmd[0]='$';    // $AA8
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='8';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';

   return(NoError);
}

/*---- Read Alarm State and Digital Output State ---------*/
 WORD CALLBACK ReadOutputAlarmState(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7011/12/14/16
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   //  wCounter=wBuf[5];      // Don't Care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   //               Alarm State:
   // wBuf(7):0       Disable  'for 7005 wBuf(7) not define
   // wBuf(7):1       Momentary
   // wBuf(7):2       Latch
   //               Digital Output //for 7005 wBuf(8) is the 6 channel DO output value
   // wBuf(8):0       Bit:1 Disable  Bit 0: Disable
   // wBuf(8):1       Bit:1 Disable  Bit 0: Enable
   // wBuf(8):2       Bit:1 Enable   Bit 0: Disable
   // wBuf(8):3       Bit:1 Enable   Bit 0: Enable

   switch (wID)
   {
       case 0x7011 :
       case 0x7012 :
       case 0x7014 :
       case 0x7016 : 
	   case 0x7005 : //07/07/2004	   
	   break;
       default: return(ModuleIdError);
   }


   szCmd[0]='@';    // @AADI
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='D';
   szCmd[4]='I';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      sprintf(szReceive,"%s",szRec);

   if(wRetVal != NoError) return wRetVal;

   //if(szRec[0] != '!') return ResultStrCheckError;
   if(wID==0x7005)
   {
		if(wCheckSum==0) 
			wRecStrLen=5; 
		else 
			wRecStrLen=7;
   }
   else
   {
		if(wCheckSum==0) 
			wRecStrLen=8; 
		else 
			wRecStrLen=10;
   }

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;
   if (wID==0x7005)
   {
		wBuf[8]=AsciiToHex(szRec[3])*16+AsciiToHex(szRec[4]);
   }
   else
   {
		wBuf[7]=szRec[3]-'0'; //!aasooii
		if( wID==0x7016 )
			wBuf[8]= AsciiToHex(szRec[5]); //for 7016 wBuf[8] is only output value not alarm value
		else
			wBuf[8]=(szRec[4]-'0')*2+(szRec[5]-'0');
   }

   return(NoError);
}




/************************************************************************/
/* -------------- 7080 Counter In-------------------------------------- */
 WORD CALLBACK CounterIn_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                           // wBuf[7]= Hi-Word of Counter Value
                           // wBuf[8]= Lo-Word of Counter Value

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }
   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   #AAN  read Counter/Frequency Value
   szCmd[0]='#';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]=wCounter+'0';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '>') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=9; else wRecStrLen=11;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[1]);
   B=AsciiToHex(szRec[2]);
   C=AsciiToHex(szRec[3]);
   D=AsciiToHex(szRec[4]);
   E=AsciiToHex(szRec[5]);
   F=AsciiToHex(szRec[6]);
   G=AsciiToHex(szRec[7]);
   H=AsciiToHex(szRec[8]);

   wBuf[7]=D+C*16+B*256+A*4096;
   wBuf[8]=H+G*16+F*256+E*4096;

   return(NoError);
}

/* -------------- 7080 Read Max Counter Value ------------------------- */
 WORD CALLBACK ReadCounterMaxValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                           // wBuf[7]= Hi-Word of Max Counter Value
                           // wBuf[8]= Lo-Word of Max Counter Value

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   $AA3N  read MAX Value
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='3';
   szCmd[4]=wCounter+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=11; else wRecStrLen=13;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);
   B=AsciiToHex(szRec[4]);
   C=AsciiToHex(szRec[5]);
   D=AsciiToHex(szRec[6]);
   E=AsciiToHex(szRec[7]);
   F=AsciiToHex(szRec[8]);
   G=AsciiToHex(szRec[9]);
   H=AsciiToHex(szRec[10]);

   wBuf[7]=D+C*16+B*256+A*4096;
   wBuf[8]=H+G*16+F*256+E*4096;

   return(NoError);
}

/* -------------- 7080 Set Max Counter Value ------------------------- */
#ifdef _WIN32_WCE
 WORD CALLBACK SetCounterMaxValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], unsigned long MaxValue)//WinCE Troy
#else
 WORD CALLBACK SetCounterMaxValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], double MaxValue) 
#endif
{
   char    wPort,wFlag,c1,c2;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD    wRecStrLen,i;
   DWORD   dwMaxValue;
   char    Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string


   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

#ifndef _WIN32_WCE
   if( MaxValue>4294967295.1 || MaxValue<0.0 )
      return( InvalidateCounterValue );
#endif

   dwMaxValue=(unsigned long)MaxValue;
   sprintf(Buffer,"%08x",dwMaxValue);
   for(i=0; i<8; i++)
      if( Buffer[i]>='a' && Buffer[i]<='f' )
      {
          Buffer[i]=Buffer[i]-'a'+'A';
      }

   //***   $AA3N(Data)  Set MAX Value
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='3';
   szCmd[4]=wCounter+'0';
   szCmd[5]=Buffer[0];
   szCmd[6]=Buffer[1];
   szCmd[7]=Buffer[2];
   szCmd[8]=Buffer[3];
   szCmd[9]=Buffer[4];
   szCmd[10]=Buffer[5];
   szCmd[11]=Buffer[6];
   szCmd[12]=Buffer[7];
   szCmd[13]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/* ------------ 7080 Read Alarm limit value of counter N----------------- */
 WORD CALLBACK ReadAlarmLimitValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                        // wBuf[7]= Hi-Word of Max Counter Value
                        // wBuf[8]= Lo-Word of Max Counter Value

   if( wID!=0x7080 )
   {
       return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   $AARP: read limit Value of counter 0
   //***   $AARA: read limit Value of counter 1
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='R';
   if( wCounter==0 )    // Counter 0
      szCmd[4]='P';
   else
      szCmd[4]='A';     // Counter 1
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=11; else wRecStrLen=13;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);
   B=AsciiToHex(szRec[4]);
   C=AsciiToHex(szRec[5]);
   D=AsciiToHex(szRec[6]);
   E=AsciiToHex(szRec[7]);
   F=AsciiToHex(szRec[8]);
   G=AsciiToHex(szRec[9]);
   H=AsciiToHex(szRec[10]);

   wBuf[7]=D+C*16+B*256+A*4096;
   wBuf[8]=H+G*16+F*256+E*4096;

   return(NoError);
}


/* ------------ 7080 Set Counter 0/1 Alarm Counter Value ---------------- */
#ifdef _WIN32_WCE
  WORD CALLBACK SetAlarmLimitValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], unsigned long  AlarmValue)
#else
 WORD CALLBACK SetAlarmLimitValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], double AlarmValue)  //WinCE Troy
#endif
{
   char    wPort,wFlag,c1,c2;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD    wRecStrLen,i;
   DWORD   dwMaxValue;
   char    Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,   1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string


   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

#ifndef _WIN32_WCE
   if( AlarmValue>4294967295.1 || AlarmValue<0.0 )
      return( InvalidateCounterValue );
#endif

   dwMaxValue=(unsigned long)AlarmValue;
   sprintf(Buffer,"%08x",dwMaxValue);
   for(i=0; i<8; i++)
      if( Buffer[i]>='a' && Buffer[i]<='f' )
      {
          Buffer[i]=Buffer[i]-'a'+'A';
      }

   //***   @AAPA(Data) counter 0,  @AASA(DATA) counter 1
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   if( wBuf[5]==0 )
      szCmd[3]='P';
   else
      szCmd[3]='S';
   szCmd[4]='A';
   szCmd[5]=Buffer[0];
   szCmd[6]=Buffer[1];
   szCmd[7]=Buffer[2];
   szCmd[8]=Buffer[3];
   szCmd[9]=Buffer[4];
   szCmd[10]=Buffer[5];
   szCmd[11]=Buffer[6];
   szCmd[12]=Buffer[7];
   szCmd[13]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*--------------- 7080D  Enable Counter Alarm ----------------------*/
 WORD CALLBACK EnableCounterAlarm_7080D(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Momentary,  1: Latch
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   @AAEAT  Enable Counter Alarm
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='E';
   szCmd[4]='A';
   if( wCounter==0 )   // Momentary Alarm State
      szCmd[5]='M';
   else
      szCmd[5]='L';    // Latch Alarm State
   szCmd[6]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*--------------- 7080D  Disable Counter Alarm ----------------------*/
 WORD CALLBACK DisableCounterAlarm_7080D(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // Don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }


   //***   @AADA  Disable Counter Alarm
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='D';
   szCmd[4]='A';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*--------------- 7080 Enable Counter Alarm ----------------------*/
 WORD CALLBACK EnableCounterAlarm_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   //WORD A;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   @AAEAN  Enable Counter Alarm
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='E';
   szCmd[4]='A';
   szCmd[5]=wCounter+'0';
   szCmd[6]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*--------------- 7080 Disable Counter Alarm ----------------------*/
 WORD CALLBACK DisableCounterAlarm_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   //WORD A;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   @AADAN  Enable Counter Alarm
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='D';
   szCmd[4]='A';
   szCmd[5]=wCounter+'0';
   szCmd[6]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}




/* -------------- 7080 Read Counter Status ----------------------- */
 WORD CALLBACK ReadCounterStatus_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                           // wBuf[7]=1: Counting  0: not counting

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   $AA5N  read Counter Status
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='5';
   szCmd[4]=wCounter+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);

   wBuf[7]=A;

   return(NoError);
}


/* -------------- 7080 Clear Counter ----------------------- */
 WORD CALLBACK ClearCounter_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   //WORD A;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   $AA6N  Clear Counter
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='6';
   szCmd[4]=wCounter+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*---- 7080 Read Counter 0/1 Alarm State and Digital Output State ---------*/
 WORD CALLBACK ReadOutputAlarmState_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A,B;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // Don't Care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
   // wBuf(7):0  for 7080 ->  Counter0:Disable   Counter1:Disable
   // wBuf(7):1               Counter0:Disable   Counter1:Enable
   // wBuf(7):2               Counter0:Enable    Counter1:Disable
   // wBuf(7):3               Counter0:Enable    Counter1:Enable
   // wBuf(7):0  for 7080D ->  Counter0:Disable
   // wBuf(7):1                Counter0:Momentary
   // wBuf(7):2                Counter0:Latch

   // wBuf(8):0    Bit:1 Disable  Bit 0: Disable
   // wBuf(8):1    Bit:1 Disable  Bit 0: Enable
   // wBuf(8):2    Bit:1 Enable   Bit 0: Disable
   // wBuf(8):3    Bit:1 Enable   Bit 0: Enable

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //***   @AADI  read counter's alarm state
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='D';
   szCmd[4]='I';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=8; else wRecStrLen=10;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);
   B=(szRec[4]-'0')*2+(szRec[5]-'0');

   wBuf[7]=A;
   wBuf[8]=B;

   return(NoError);
}

/*-------- 7080 Set Counter's Input Signal Mode  ---------*/
 WORD CALLBACK SetInputSignalMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT,wMode;
   //WORD A;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wMode=wBuf[5];         // Counter 0's and Counter 1's signal mode
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                     // wBuf(5):0    Counter:0 TTL     Counter:0 TTL
                     // wBuf(5):1    Counter:1 PHOTO   Counter:1 PHOTO
                     // wBuf(5):2    Counter:0 TTL     Counter:1 PHOTO
                     // wBuf(5):3    Counter:1 PHOTO   Counter:0 TTL

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //***   $AAB  read counter's input signal state
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='B';
   szCmd[4]=wMode+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}



/*-------- 7080 Read Counter's Input Signal Mode  ---------*/
 WORD CALLBACK ReadInputSignalMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // Don't Care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                     // wBuf(7):0    Counter:1 TTL     Counter:0 TTL
                     // wBuf(7):1    Counter:1 PHOTO   Counter:0 PHOTO
                     // wBuf(7):2    Counter:1         Counter:0
                     // wBuf(7):3    Counter:1         Counter:0

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //***   $AAB  read counter's input signal state
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='B';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4; else wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);

   wBuf[7]=A;

   return(NoError);
}


/* ------------ 7080 Preset Counter 0/1 Counter Value ---------------- */
#ifdef _WIN32_WCE
 WORD CALLBACK PresetCounterValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], unsigned long PresetValue)
#else
 WORD CALLBACK PresetCounterValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], double PresetValue) //WinCE Troy
#endif
{
   char    wPort,wFlag,c1,c2;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD    wRecStrLen,i;
   DWORD   dwPresetValue;
   char    Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,   1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

#ifndef _WIN32_WCE
   if( PresetValue>4294967295.1 || PresetValue<0.0 )
      return( InvalidateCounterValue );
#endif
   dwPresetValue=(unsigned long)PresetValue;

   sprintf(Buffer,"%08x",dwPresetValue);
   for(i=0; i<8; i++)
      if( Buffer[i]>='a' && Buffer[i]<='f' )
      {
          Buffer[i]=Buffer[i]-'a'+'A';
      }

   //***   @AAPN(Data) N:0 counter 0,  N:1 counter 1
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='P';
   szCmd[4]=wCounter+'0';
   szCmd[5]=Buffer[0];
   szCmd[6]=Buffer[1];
   szCmd[7]=Buffer[2];
   szCmd[8]=Buffer[3];
   szCmd[9]=Buffer[4];
   szCmd[10]=Buffer[5];
   szCmd[11]=Buffer[6];
   szCmd[12]=Buffer[7];
   szCmd[13]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/* ------------ 7080 Read Preset Counter 0/1 Counter Value ------------- */
 WORD CALLBACK ReadPresetCounterValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD A,B,C,D,E,F,G,H;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0,  1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
                      // wBuf[7]= Hi-Word of Preset Counter Value
                      // wBuf[8]= Lo-Word of Preset Counter Value

   if( wID!=0x7080 )
   {
       return(ModuleIdError);
   }

   if( wCounter>1 )
       return(InvalidateCounterNo);

   //***   @AAGN: N:0 Counter 0,  1: Counter 1
   szCmd[0]='@';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='G';
   szCmd[4]=wCounter+'0';     // Counter
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=11; else wRecStrLen=13;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);
   B=AsciiToHex(szRec[4]);
   C=AsciiToHex(szRec[5]);
   D=AsciiToHex(szRec[6]);
   E=AsciiToHex(szRec[7]);
   F=AsciiToHex(szRec[8]);
   G=AsciiToHex(szRec[9]);
   H=AsciiToHex(szRec[10]);

   wBuf[7]=D+C*16+B*256+A*4096;
   wBuf[8]=H+G*16+F*256+E*4096;

   return(NoError);
}


/*-------- 7080 Start Counting ---------*/
 WORD CALLBACK StartCounting_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD wStart;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // 0: Counter 0   1: Counter 1
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   wStart=wBuf[7];        // 0: Stop Counting  1: Start Counting

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //***   $AA5NS     Start/Stop Counting
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='5';
   szCmd[4]=wCounter+'0';
   szCmd[5]=wStart+'0';
   szCmd[6]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0)  wRecStrLen=3;  else  wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*-------- Set 7080 module mode  ---------*/
 WORD CALLBACK SetModuleMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wMode,wT;
   //WORD wStart;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wMode=wBuf[5];         // 0: 7080 mode   1: 7080D mode
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //***   ~AAAM   set module mode
   szCmd[0]='~';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='A';
   szCmd[4]=wMode+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0)  wRecStrLen=3;   else  wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/*-------- Read 7080 module mode  ---------*/
 WORD CALLBACK ReadModuleMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char wPort,wFlag,c1,c2;
   WORD wAddr,wRetVal,wID,wCheckSum,wTimeOut,wMode,wT;
   //WORD wStart;
   WORD wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wMode=wBuf[5];         // Don't care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
               //------------- Output -------------------
               //   wBuf[5]=   0: 7080,    1: 7080D

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //***   ~AAA   set module mode
   szCmd[0]='~';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='A';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0)  wRecStrLen=4;    else  wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';

   return(NoError);
}


/*-------- 7080 Set Level Volt ---------*/
 WORD CALLBACK SetLevelVolt_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wHiLo,wT;
   WORD   wRecStrLen;
   float  fValue;
   char   OutCh[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Low Level,  1: High Level
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   fValue=fBuf[0];        // Level Voltage
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }


   if( fValue>5.0 )
      return ExceedInputRange;
   if( fValue<0.0 )
      return UnderInputRange;

   sprintf(OutCh,"%3.1f",fValue);

   if( wHiLo==0 )   // Set Low Level Voltage, $AA1L(data)
   {
      szCmd[0]='$';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='1';
      szCmd[4]='L';

      szCmd[5]=OutCh[0];
      szCmd[6]=OutCh[2];
      szCmd[7]=0;
   }
   else   // Set High Level Voltage, $AA1H(data)
   {
      szCmd[0]='$';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='1';
      szCmd[4]='H';

      szCmd[5]=OutCh[0];
      szCmd[6]=OutCh[2];
      szCmd[7]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- 7080 Read Level Volt ---------*/
 WORD CALLBACK ReadLevelVolt_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wHiLo,wT;
   WORD   wRecStrLen;
   //float  fValue;
   //char   OutCh[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Low Level,  1: High Level
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- output ------------------------------------
               //   fValue=fBuf[0];        // Level Voltage

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wHiLo==0 )   // Read Low Level Voltage, $AA1L
   {
      szCmd[0]='$';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='1';
      szCmd[4]='L';
      szCmd[5]=0;
   }
   else   // Read High Level Voltage, $AA1H
   {
      szCmd[0]='$';
      c2=(char)(wAddr&0x0f);
      wAddr=wAddr/16;
      c1=(char)(wAddr&0x0f);
      szCmd[1] = HexToAscii(c1);
      szCmd[2] = HexToAscii(c2);
      szCmd[3]='1';
      szCmd[4]='H';
      szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=5; else wRecStrLen=7;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   fBuf[0]=(float)((szRec[3]-'0')*1.0+(szRec[4]-'0')*0.1);

   return(NoError);
}


/* -------------- 7080 Set Min Signal Width ------------------------- */
 WORD CALLBACK SetMinSignalWidth_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], long MinWidth)
{
   char    wPort,wFlag,c1,c2;
   WORD    wHiLo,wMinWidth;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD    wRecStrLen;
   char    Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Low Level,  1: High Level
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wHiLo>1 )
       return(InvalidateCounterNo);

   if( MinWidth>65536 || MinWidth<0 )
      return( InvalidateCounterValue );

   wMinWidth=(WORD)MinWidth;
   sprintf(Buffer,"%05u",wMinWidth);

   if( wHiLo==0 ) // $AA0L(data),  Set Min Input Signal Width at Low Level
   {
       szCmd[0]='$';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='0';
       szCmd[4]='L';
       szCmd[5]=Buffer[0];
       szCmd[6]=Buffer[1];
       szCmd[7]=Buffer[2];
       szCmd[8]=Buffer[3];
       szCmd[9]=Buffer[4];
       szCmd[10]=0;
   }
   else         // $AA0H(data),  Set Min Input Signal Width at High Level
   {
       szCmd[0]='$';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='0';
       szCmd[4]='H';
       szCmd[5]=Buffer[0];
       szCmd[6]=Buffer[1];
       szCmd[7]=Buffer[2];
       szCmd[8]=Buffer[3];
       szCmd[9]=Buffer[4];
       szCmd[10]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/* -------------- 7080 Get Min Signal Width ------------------------- */
 WORD CALLBACK ReadMinSignalWidth_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char    wPort,wFlag,c1,c2;
   WORD    wHiLo;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD    wRecStrLen;
   WORD    A,B,C,D,E;
   //char    Buffer[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wHiLo=wBuf[5];         // 0: Low Level,  1: High Level
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //---------------------- Output ------------------------------------
   // wBuf[7]= Min Width

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wHiLo>1 )
       return(InvalidateCounterNo);


   if( wHiLo==0 ) // $AA0L,  Get Min Input Signal Width at Low Level
   {
       szCmd[0]='$';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='0';
       szCmd[4]='L';
       szCmd[5]=0;
   }
   else         // $AA0H,  Get Min Input Signal Width at High Level
   {
       szCmd[0]='$';
       c2=(char)(wAddr&0x0f);
       wAddr=wAddr/16;
       c1=(char)(wAddr&0x0f);
       szCmd[1] = HexToAscii(c1);
       szCmd[2] = HexToAscii(c2);
       szCmd[3]='0';
       szCmd[4]='H';
       szCmd[5]=0;
   }

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=8;  else  wRecStrLen=10;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   A=AsciiToHex(szRec[3]);
   B=AsciiToHex(szRec[4]);
   C=AsciiToHex(szRec[5]);
   D=AsciiToHex(szRec[6]);
   E=AsciiToHex(szRec[7]);

   wBuf[7]=A*10000+B*1000+C*100+D*10+E;

   return(NoError);
}


/* -------------- 7080 Set Gate Mode ------------------------- */
 WORD CALLBACK SetGateMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char    wPort,wFlag,c1,c2;
   WORD    wGateMode;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD    wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wGateMode=wBuf[5];         // 0: Low,  1: High,  2: None
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   if( wGateMode>2 )
       return(InvalidateGateMode);

   szCmd[0]='$';//$AAAG set gate mode
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='A';
   szCmd[4]=wGateMode+'0';
   szCmd[5]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3;  else  wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}


/* -------------- 7080 Read Gate Mode ------------------------- */
 WORD CALLBACK ReadGateMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char    wPort,wFlag,c1,c2;
   WORD    wAddr,wRetVal,wID,wCheckSum,wTimeOut,wT;
   WORD    wRecStrLen;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   //  wGateMode=wBuf[5];     // 0: Low,  1: High,  2: None
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   //--------------------- output ---------------------------
   // wBuf[5]:   // 0: Low,  1: High,  2: None

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }

   //if( wGateMode>2 )
   //    return(InvalidateGateMode);

   szCmd[0]='$';//$AAA Read gate mode
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='A';
   szCmd[4]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=4;  else  wRecStrLen=6;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   wBuf[5]=szRec[3]-'0';
   return(NoError);
}



/*-------- 7080 Send data to LED  ---------*/
 WORD CALLBACK DataToLED_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag,c1,c2;
   WORD   wAddr,wRetVal,wID,wCheckSum,wTimeOut,wCounter,wT;
   WORD   wRecStrLen;
   float  fOutValue;
   char   OutCh[10];

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddr=wBuf[1];         // module address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSum=wBuf[3];     // checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wCounter=wBuf[5];      // Don't Care
   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string
   fOutValue=fBuf[0];     // Data to LED
   //---------------------- output ------------------------------------

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   }


   if( fOutValue>19999.0 )
      return ExceedInputRange;
   else if( fOutValue>10000.0 )
      sprintf(OutCh,"%6.1f",fOutValue);
   else if( fOutValue>1000.0 )
      sprintf(OutCh,"%06.1f",fOutValue);
   else if( fOutValue>100.0 )
      sprintf(OutCh,"%06.2f",fOutValue);
   else if( fOutValue>10.0 )
      sprintf(OutCh,"%06.3f",fOutValue);
   else if( fOutValue>1.0 )
      sprintf(OutCh,"%06.4f",fOutValue);
   else if( fOutValue>0.0 )
      sprintf(OutCh,"%06.5f",fOutValue);
   else
      return UnderInputRange;


   //***   $AA9(DATA)  read counter's input signal state
   szCmd[0]='$';
   c2=(char)(wAddr&0x0f);
   wAddr=wAddr/16;
   c1=(char)(wAddr&0x0f);
   szCmd[1] = HexToAscii(c1);
   szCmd[2] = HexToAscii(c2);
   szCmd[3]='9';

   szCmd[4]=OutCh[0];
   szCmd[5]=OutCh[1];
   szCmd[6]=OutCh[2];
   szCmd[7]=OutCh[3];
   szCmd[8]=OutCh[4];
   szCmd[9]=OutCh[5];

   szCmd[10]=0;

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSum,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSum==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}

/*-------- Set configuration for 7080 ---------*/
 WORD CALLBACK SetConfiguration_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[])
{
   char   wPort,wFlag;
   WORD   wRetVal,wID,wTimeOut,wT;
   WORD   wRecStrLen;
   WORD   wAddrOld,wAddrNew,wType,wBaudrate,wCheckSumOld,wCheckSumNew;
   WORD   wTypeValue,wFormatValue,wFreqGateTime;

   //---------------------- input -------------------------------------
   wPort=(char)wBuf[0];   // RS-232 port number: 1/2/3/4/..../255
   wAddrOld=wBuf[1];      // module original address: 0x00 to 0xFF
   wID=wBuf[2];           // module ID: 0x7080
   wCheckSumOld=wBuf[3];          // module original checksum: 0=disable, 1=enable
   wTimeOut=wBuf[4];      // TimeOut constant: normal=100
   wFreqGateTime=wBuf[5]; // desired frequency gate time:
                                // 0: 0.1 second
                                // 1: 1.0 second
                                // Don't care wBuf[5] if set the module into Counter mode

   wFlag=(char)wBuf[6];   // flag: 0=no save, 1=save send/receive string

   wAddrNew=wBuf[7];      // desired new address
   wType=wBuf[8];         // desired Type->  0:Counter mode
                                             //  1:Frequency mode
   wBaudrate=wBuf[9];     // desired Baudrate:
                           //    3: 1200 BPS    4:  2400 BPS
                           //    5: 4800 BPS    6:  9600 BPS
                           //    7: 19200 BPS   8:  38400 BPS
                           //    9: 57600 BPS   10: 115200 BPS
   wCheckSumNew=wBuf[10]; // desired Checksum Address

   //------------------- check parameter ----------------------
//   if( wAddrNew<0 || wAddrNew>255 )
   if(wAddrNew>255 )
      return(CmdError);
//   if( wAddrOld<0 || wAddrOld>255 )
   if( wAddrOld>255 )
      return(CmdError);
//   if( wType<0 || wType>1 )
   if( wType>1 )
      return(CmdError);
   if( wBaudrate<3 || wBaudrate>10 )
      return(CmdError);

   if( wID!=0x7080 )
   {
      return(ModuleIdError);
   } //%AANNTTCCFF set module configuration

   wTypeValue=(wType==0)?80:81; //50:51 (Dec), 80:81 (Hex)
   wFormatValue=64*wCheckSumNew+4*wFreqGateTime;

   sprintf(szCmd,"%%%02X%02X%02X%02X%02X",wAddrOld,wAddrNew,wTypeValue,
                                                     wBaudrate,wFormatValue);

   if(wFlag==1)
   {
      strcpy(szSend,szCmd);
      szReceive[0]=0;
   }
   wRetVal=Send_Receive_Cmd(wPort, szCmd, szRec, wTimeOut, wCheckSumOld,&wT);
   if(wFlag==1)      strcpy(szReceive,szRec);

   if(wRetVal != NoError) return wRetVal;

   if(szRec[0] != '!') return ResultStrCheckError;

   if(wCheckSumOld==0) wRecStrLen=3; else wRecStrLen=5;

   if( strlen(szRec) != wRecStrLen) return ResultStrCheckError;

   return(NoError);
}









