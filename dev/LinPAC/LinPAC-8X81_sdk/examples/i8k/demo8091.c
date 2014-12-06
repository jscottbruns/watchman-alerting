/* Motion/Encoder demo for I-8091/I-8090 i8k modules.

   Author: Moki Matsushima

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* File level history (record changes for this file here.)

   v 0.0.0  1 Sep 2005 by Moki Matsushima
     create, blah blah... */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "msw.h"
#define DisplayEncoder() ""
typedef struct {
  int           address;
  unsigned char DDA,AD;
  unsigned int  LSP,HSP;
  unsigned char xmode,ymode;
  unsigned char xdir,ydir;
  unsigned char xson,yson;
  unsigned char NCmode;
} i8091CardType;

i8091CardType card1;
static int i8091Slot;

void stop_process()
{
	I8091_STOP_ALL(i8091Slot);
	I8091_SET_VAR(i8091Slot, card1.DDA, card1.AD, card1.LSP, card1.HSP);
	I8091_SET_DEFDIR(i8091Slot, card1.xdir, card1.ydir);
	I8091_SET_MODE(i8091Slot, card1.xmode, card1.ymode);
	I8091_SET_SERVO_ON(i8091Slot, card1.xson, card1.yson); 
	I8091_SET_NC(i8091Slot, card1.NCmode);
	usleep(50);
//	Delay(50);
}

int main(int argc, char **argv)
{
	int	RetValue,type;
	int	chkey,tempint,speed;
	int	xpulse,ypulse;
	long	pulse,x,y,R;
	unsigned char j,temp,axis,dir;

	i8091Slot=1;
	RetValue = Open_Slot(i8091Slot);
	
	if (RetValue >0) {
		printf("open Slot %d failed!\n",i8091Slot);
		return FAILURE;
	}

	card1.address=0;         //PortAddress[i8091Slot];
	card1.DDA   = 10;
	card1.AD    = 3;
	card1.LSP   = 5;
	card1.HSP   = 150;
	card1.xmode = CW_CCW;
	card1.ymode = CW_CCW;
	card1.xdir  = NORMAL_DIR;
	card1.ydir  = NORMAL_DIR;
	card1.xson  = ON;
	card1.yson  = ON;
	card1.NCmode= OFF;
	I8091_REGISTRATION(i8091Slot, card1.address); 
	I8091_RESET_SYSTEM(i8091Slot);
	I8091_SET_VAR(i8091Slot, card1.DDA, card1.AD, card1.LSP, card1.HSP);     //set DDA cycle,accelerating/decelerating speed, low speed and high speed value
	I8091_SET_DEFDIR(i8091Slot, card1.xdir, card1.ydir);                     //define direction
	I8091_SET_MODE(i8091Slot, card1.xmode, card1.ymode);                     //define output mode
	I8091_SET_SERVO_ON(i8091Slot, card1.xson, card1.yson);                   //set servo ON/OFF, define output mode.
	I8091_SET_NC(i8091Slot, card1.NCmode);                                   //To config limit switch as N.C. or N.O.

	do{
	printf("\n----------------------i8091 testing kit-----------------------------------\r\n");
	printf("(0)Exit                 (A)i8091_IS_X_STOP       (K)i8091_CSP_MOVE        \r\n");
	printf("(1)i8091_RESET_SYSTEM   (B)i8091_IS_Y_STOP       (L)i8091_SLOW_DOWN       \r\n");
	printf("(2)i8091_SET_VAR        (C)i8091_LIMIT_X         (M)i8091_SLOW_STOP       \r\n");
	printf("(3)i8091_SET_DEFDIR     (D)i8091_LIMIT_Y         (N)i8091_INTP_PULSE      \r\n");
	printf("(4)i8091_SET_MODE       (E)i8091_LSP_ORG         (O)i8091_INTP_LINE       \r\n");
	printf("(5)i8091_SET_SERVO_ON   (F)i8091_HSP_ORG         (P)i8091_INTP_LINE02     \r\n");
	printf("(6)i8091_SET_NC         (G)i8091_LSP_PULSE_MOVE  (Q)i8091_CIRCLE02        \r\n");
	printf("(7)i8091_STOP_X         (H)i8091_HSP_PULSE_MOVE  (R)i8091_ARC02           \r\n");
	printf("(8)i8091_STOP_Y         (I)i8091_LSP_MOVE        (S)User Define Testing   \r\n");
	printf("(9)i8091_STOP_ALL       (J)i8091_HSP_MOVE                                 \r\n");
	printf("DDA=%d ,AD=%d ,LSP=%d ,HSP=%d ,xmode=",card1.DDA,card1.AD,card1.LSP,card1.HSP);
	if (card1.xmode==CW_CCW) printf("CW_CCW ,"); else printf("PULSE_DIR ,");
	printf("ymode=");
	if (card1.ymode==CW_CCW) printf("CW_CCW\r\n"); else printf("PULSE_DIR\r\n");
	printf("xdir=");
	if (card1.xdir==NORMAL_DIR) printf("NORMAL_DIR ,"); else printf("REVERSE_DIR ,");
	printf("ydir=");
	if (card1.ydir==NORMAL_DIR) printf("NORMAL_DIR ,"); else printf("REVERSE_DIR ,");
	printf("xson=");
	if (card1.xson==ON) printf("ON ,"); else printf("OFF ,");
	printf("yson=");
	if (card1.yson==ON) printf("ON ,"); else printf("OFF ,");
	printf("NCmode=");
	if (card1.NCmode==ON) printf("ON\r\n"); else printf("OFF\r\n");
	printf("Command(0~9,A~U):");
	
	chkey=getch();
	if (chkey !=10 ) getch();
	//printf("command ^_^ = %d %c\n",chkey,chkey);

	switch (chkey)
	{
		case '0':
			I8091_RESET_SYSTEM(i8091Slot);
			printf(">>bye-bye...\r\n");
			break;
			//return;
	//---------------------------------------------------------------
		case '1':
			printf(">>i8091_RESET_SYSTEM\r\n");
			I8091_RESET_SYSTEM(i8091Slot);
			stop_process();
			break;
	//---------------------------------------------------------------
		case '2':
			printf(">>i8091_SET_VAR\r\n");
			printf("Input DDA cycle(1~254)=");
			scanf("%d",&tempint); card1.DDA=(unsigned char)tempint;
			printf("Input AD acceleratio/deceleration(1~254)=");
			scanf("%d",&tempint); card1.AD=(unsigned char)tempint;
			printf("Input LSP low speed(1~254)=");
			scanf("%d",&tempint); card1.LSP=(unsigned char)tempint;
			printf("Input HSP high speed(LSP~2040)=");
			scanf("%d",&tempint); card1.HSP=(int)tempint;
			I8091_SET_VAR(i8091Slot, card1.DDA, card1.AD, card1.LSP, card1.HSP);
			break;
       //---------------------------------------------------------------
		case '3':
			printf(">>i8091_SET_DEFDIR\r\n");
			printf("Input X axis direction definition(0:NORMAL 1:REVERSE)=");
			scanf("%d",&tempint);
			if (tempint==0) card1.xdir=NORMAL_DIR;
			else            card1.xdir=REVERSE_DIR;
			printf("Input Y axis direction definition(0:NORMAL 1:REVERSE)=");
			scanf("%d",&tempint);
			if (tempint==0) card1.ydir=NORMAL_DIR;
			else            card1.ydir=REVERSE_DIR;
			I8091_SET_DEFDIR(i8091Slot, card1.xdir, card1.ydir);
			break;
	//---------------------------------------------------------------
		case '4':
			printf(">>i8091_SET_MODE\r\n");
			printf("Input X axis output pulse mode(0:CW_CCW 1:PULSE_DIR)=");
			scanf("%d",&tempint);
			if (tempint==0) card1.xmode=CW_CCW;
			else            card1.xmode=PULSE_DIR;
			printf("Input Y axis output pulse mode(0:CW_CCW 1:PULSE_DIR)=");
			scanf("%d",&tempint);
			if (tempint==0) card1.ymode=CW_CCW;
			else            card1.ymode=PULSE_DIR;
			I8091_SET_MODE(i8091Slot, card1.xmode, card1.ymode);
			break;
	//---------------------------------------------------------------
		case '5':
			printf(">>i8091_SET_SERVO_ON\r\n");
			printf("Input X axis servo on(0:OFF 1:ON)=");
			scanf("%d",&tempint);
			if (tempint==0) card1.xson=OFF;
			else            card1.xson=ON;
			printf("Input Y axis servo on(0:OFF 1:ON)=");
			scanf("%d",&tempint);
			if (tempint==0) card1.yson=OFF;
			else            card1.yson=ON;
			I8091_SET_SERVO_ON(i8091Slot, card1.xson, card1.yson);
			break;
	//---------------------------------------------------------------
		case '6':
			printf(">>i8091_SET_NC\r\n");
			printf("Input Normal Close mode(0:OFF 1:ON)=");
			scanf("%d",&tempint);
			if (tempint==1) card1.NCmode=ON;
			else            card1.NCmode=OFF;
			I8091_SET_NC(i8091Slot, card1.NCmode);
			break;
	//---------------------------------------------------------------
		case '7':
			printf(">>i8091_STOP_X\r\n");
			I8091_STOP_X(i8091Slot);
		break;
	//---------------------------------------------------------------
		case '8':
			printf(">>i8091_STOP_Y\r\n");
			I8091_STOP_Y(i8091Slot);
			break;
	//---------------------------------------------------------------
		case '9':
			printf(">>i8091_STOP_ALL\r\n");
			stop_process();
			break;
	//---------------------------------------------------------------
		case 'a':
		case 'A':
			printf(">>i8091_IS_X_STOP -->");
			if (I8091_IS_X_STOP(i8091Slot)==YES) printf("YES\r\n");
			else                             printf("NO\r\n");
			break;
	//---------------------------------------------------------------
		case 'b':
		case 'B':
			printf(">>i8091_IS_Y_STOP -->");
			if (I8091_IS_Y_STOP(i8091Slot)==YES) printf("YES\r\n");
			else                             printf("NO\r\n");
			break;
	//---------------------------------------------------------------
		case 'C':
		case 'c':
			printf(">>i8091_LIMIT_X\r\n");
			temp=I8091_LIMIT_X(i8091Slot);
			printf(">>/HOME1 /LS11 xxx xxx /LS14 /FF1EF /FF1FF /EMG\r\n");
			printf(">>   %d     %d    %d   %d     %d     %d      %d     %d\r\n",(temp&0x01),(temp&0x02) >> 1,(temp&0x04) >> 2,(temp&0x08) >> 3,(temp&0x10) >> 4,(temp&0x20) >> 5,(temp&0x40) >> 6,(temp&0x80) >> 7);
			break;
	//---------------------------------------------------------------
		case 'd':
		case 'D':
			printf(">>i8091_LIMIT_Y\r\n");
			temp=I8091_LIMIT_Y(i8091Slot);
			printf(">>/HOME2 /LS21 xxx xxx /LS24 CPUS /XSTOP /YSTOP\r\n");
			printf(">>   %d     %d    %d   %d    %d    %d      %d      %d\r\n",(temp&0x01),(temp&0x02) >> 1,(temp&0x04) >> 2,(temp&0x08) >> 3,(temp&0x10) >> 4,(temp&0x20) >> 5,(temp&0x40) >> 6,(temp&0x80) >> 7);
			break;
	//---------------------------------------------------------------
		case 'e':
		case 'E':
			printf(">>i8091_LSP_ORG\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char) tempint;
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			I8091_LSP_ORG(i8091Slot,CCW,axis);
			break;
	//---------------------------------------------------------------
		case 'f':
		case 'F':
			printf(">>i8091_HSP_ORG\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			I8091_HSP_ORG(i8091Slot,CCW,axis);
			break;
	//---------------------------------------------------------------
		case 'g':
		case 'G':
			printf(">>i8091_LSP_PULSE_MOVE\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			printf("input pulse number(+- 2^31 -1) = ");
			scanf("%ld",&pulse);
			if ((axis!=1) && (axis!=2)) break;
			I8091_LSP_PULSE_MOVE(i8091Slot,axis,pulse);
			break;
	//---------------------------------------------------------------
		case 'h':
		case 'H':
			printf(">>i8091_HSP_PULSE_MOVE\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			printf("input pulse number(+- 2^31 -1) = ");
			scanf("%ld",&pulse);
			if ((axis!=1) && (axis!=2)) break;
			printf("constant speed(1~2040)=");
			scanf("%d",&speed);
			I8091_HSP_PULSE_MOVE(i8091Slot,axis,pulse,speed);
			break;
       //---------------------------------------------------------------
		case 'i':
		case 'I':
			printf(">>i8091_LSP_MOVE\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			printf("Which direction (0:CW 1:CCW)=");
			scanf("%d",&tempint);
			dir=(unsigned char)tempint;
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			if ((dir!=CW) && (dir!=CCW)) break;
			I8091_LSP_MOVE(i8091Slot,dir,axis);
			break;
       //---------------------------------------------------------------
		case 'j':
		case 'J':
			printf(">>i8091_HSP_MOVE\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			printf("Which direction (0:CW 1:CCW)=");
			scanf("%d",&tempint);
			dir=(unsigned char)tempint;
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			if ((dir!=CW) && (dir!=CCW)) break;
			I8091_HSP_MOVE(i8091Slot,dir,axis);
			break;
       //---------------------------------------------------------------
		case 'k':
		case 'K':
			printf(">>i8091_CSP_MOVE\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			printf("Which direction (0:CW 1:CCW)=");
			scanf("%d",&tempint);
			dir=(unsigned char)tempint;
			printf("constant speed(1~2040)=");
			scanf("%d",&speed);
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			if ((dir!=CW) && (dir!=CCW)) break;
			if ((speed==0) || (speed>2040)) break;
			I8091_CSP_MOVE(i8091Slot,dir,axis,speed);
			break;
       //---------------------------------------------------------------
		case 'l':
		case 'L':
			printf(">>i8091_SLOW_DOWN\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			I8091_SLOW_DOWN(i8091Slot,axis);
			break;
       //---------------------------------------------------------------
		case 'm':
		case 'M':
			printf(">>i8091_SLOW_STOP\r\n");
			printf("Which axis (1:X-axis 2:Y-axis)=");
			scanf("%d",&tempint);
			axis=(unsigned char)tempint;
			if ((axis!=X_axis) && (axis!=Y_axis)) break;
			I8091_SLOW_STOP(i8091Slot,axis);
			break;
       //---------------------------------------------------------------
		case 'n':
		case 'N':
			printf(">>i8091_INTP_PULSE\r\n");
			printf("X pulse number (-2047 ~ 2047) = ");
			scanf("%d",&xpulse);
			printf("Y pulse number (-2047 ~ 2047) = ");
			scanf("%d",&ypulse);
			if (abs(xpulse) > 2047 ) break;
			if (abs(ypulse) > 2047 ) break;
			I8091_INTP_PULSE(i8091Slot,xpulse,ypulse);
			break;
       //---------------------------------------------------------------
		case 'o':
		case 'O':
			printf(">>i8091_INTP_LINE\r\n");
			printf("X pulse number (+- 524287) = ");
			scanf("%ld",&x);
			printf("Y pulse number (+- 524287) = ");
			scanf("%ld",&y);
			if (labs(x) > 524287) break;
			if (labs(y) > 524287) break;
			I8091_INTP_LINE(i8091Slot,x,y);
			break;
       //---------------------------------------------------------------
		case 'p':
		case 'P':
			printf(">>i8091_INTP_LINE02\r\n");
			printf("X pulse number (+- 2^31 -1) = ");
			scanf("%ld",&x);
			printf("Y pulse number (+- 2^31 -1) = ");
			scanf("%ld",&y);
			printf("Input speed (LSP~2040) = ");
			scanf("%d",&speed);
			if (speed<card1.LSP) break;
			if (speed>2040) break;
			printf("Press any key to break...\r\n");
			I8091_INTP_LINE02(i8091Slot,x,y,speed,0);
			do
			{
				DisplayEncoder();
				if (getch()) stop_process();
			} while (I8091_INTP_STOP()!=READY);
			break;
       //---------------------------------------------------------------
		case 'q':
		case 'Q':
			printf(">>i8091_INTP_CIRCLE02\r\n");
			printf("center point X = ");
			scanf("%ld",&x);
			printf("center point Y = ");
			scanf("%ld",&y);
			printf("Which direction (0:CW 1:CCW)=");
			scanf("%d",&tempint);
			printf("Input speed (LSP~2040) = ");
			scanf("%d",&speed);
			dir=(unsigned char)tempint;
			if ((dir!=CW) && (dir!=CCW)) break;
			if (speed<card1.LSP) break;
			if (speed>2040) break;

			printf("Press any key to break...\r\n");
			I8091_INTP_CIRCLE02(i8091Slot,x,y,dir,speed,0);
			do
			{
				DisplayEncoder();
				if (getch()) stop_process();
			} while (I8091_INTP_STOP()!=READY);
			break;
       //---------------------------------------------------------------
		case 'r':
		case 'R':
			printf(">>i8091_INTP_ARC02\r\n");
			printf("center point X = ");
			scanf("%ld",&x);
			printf("center point Y = ");
			scanf("%ld",&y);
			printf("Radius R = ");
			scanf("%ld",&R);
			printf("Which direction (0:CW 1:CCW)=");
			scanf("%d",&tempint);
			printf("Input speed (LSP~2040) = ");
			scanf("%d",&speed);
			dir=(unsigned char)tempint;
			if ((dir!=CW) && (dir!=CCW)) break;
			if (speed<card1.LSP) break;
			if (speed>2040) break;

			printf("Press any key to break...\r\n");
			I8091_INTP_ARC02(i8091Slot,x,y,R,dir,speed,0);
			do
			{
				DisplayEncoder();
				if (getch()) stop_process();
			} while (I8091_INTP_STOP()!=READY);
			break;
       //---------------------------------------------------------------
		case 's':
		case 'S':
			printf(">>User define testing\r\n");
			//test LINE02, no acceleration
			I8091_INTP_LINE02(i8091Slot,1000,1000,20,1);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			I8091_INTP_LINE02(i8091Slot,2000,1000,22,1);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			I8091_INTP_LINE02(i8091Slot,2000,2000,24,1);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			I8091_INTP_LINE02(i8091Slot,1000,2000,26,1);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			do {DisplayEncoder();} while (I8091_IS_X_STOP(i8091Slot)==NO);
			usleep(2000);
			//Delay(2000);

			//test WAIT_X, WAIT_Y, WAIT_Z
			I8091_INTP_LINE02(i8091Slot, 10000,-10000,150,0);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			I8091_INTP_LINE02(i8091Slot, -10000, 10000,150,0);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			do {DisplayEncoder();} while (I8091_IS_X_STOP(i8091Slot)==NO);
			do {DisplayEncoder();} while (I8091_IS_Y_STOP(i8091Slot)==NO);
			usleep(2000);
			//Delay(2000);

			I8091_INTP_CIRCLE02(i8091Slot, 5000,-5000, CW, 150, 0);
			do {DisplayEncoder();} while (I8091_INTP_STOP()!=READY);
			do {DisplayEncoder();} while (I8091_IS_X_STOP(i8091Slot)==NO);
			do {DisplayEncoder();} while (I8091_IS_Y_STOP(i8091Slot)==NO);
			break;
		}
		
	}while (chkey != '0'); 
	
	printf("\n");
	Close_Slot(i8091Slot);
	return SUCCESS;
}
