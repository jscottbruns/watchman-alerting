/*	Digital Input/Output DEMO for i87k series module

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    Dio.java
 
 	  v 0.0 2005.4.1 by moki matsushima
 
  	Copyright (c) 2005 ICPDAS, Inc. All  Rights Reserved.
 */

import java.io.*;						                	//For System.in.read()
import com.icpdas.comm.*;					          	//ICPDAS communication packages
import com.icpdas.slot.*;					           	//Slot I/O packages

public class Dio 
{
	public static void main(String[] args) throws java.io.IOException
	{
		int rev;
		int fd;
		int i=1;
		byte a[] = new byte[100];
		Comm comm1 = new Comm();				         //ICPDAS communication object
		Slot slot1 = new Slot();
		IoBuf i87kBuf = new IoBuf();				     //control matrix
		slot1.open(0);			                     //open slot 0 fot i87k slot select
		rev = comm1.open(1,115200,comm1.DATABITS_8,comm1.PARITY_NONE,comm1.STOPBITS_1);//open serial port		
		if (rev!=0) System.out.println("Open port error code : "+rev);
		else{
			i87kBuf.dwBuf[0] = 1;					        //Serial Port
			i87kBuf.dwBuf[1] = 0;				        	//Module Address		
			i87kBuf.dwBuf[3] = 0;		        			//check sum disable
			i87kBuf.dwBuf[4] = 10;					      //Timeout 100ms
			i87kBuf.dwBuf[6] = 1;			        		//Enable String Debug
		
			while(a[0]!=113) {                    
				i87kBuf.dwBuf[2] = 0x87057 ;			  //0x87057;16-channel Digital Output Module; //module name
				i87kBuf.dwBuf[5] =i;				        //Digital Output Value
				slot1.setChangeToSlot(1);				    //change to slot 1
				rev = comm1.setDigitalOut(i87kBuf);
				System.out.println("szSend = "+ i87kBuf.szSend +" szReceive = "+i87kBuf.szReceive);
				if (rev!=0) System.out.println("Digital Out Error Code : "+ rev);
				i87kBuf.dwBuf[2] = 0x87051 ;       //0x87051; 16-channel Digital Input Module; //module name
				slot1.setChangeToSlot(2);				   //change to slot 2
				rev = comm1.getDigitalIn(i87kBuf);
				System.out.println("szSend = "+ i87kBuf.szSend +" szReceive = "+i87kBuf.szReceive);
				if (rev!=0) System.out.println("Digital In Error Code : "+ rev);
				else System.out.println("Digital In : "+ i87kBuf.dwBuf[5]);
				System.in.read(a);
				i=(i>255)?1:(i<<1);
			}
		}
		slot1.close(0);
		comm1.close(1);
		System.out.println("End of program");		
	}
}

