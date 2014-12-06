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
 
 	 v 0.0 2003.8.7 by moki matsushima
 
 	Copyright (c) 2003 ICPDAS, Inc. All  Rights Reserved.   */
 
import java.io.*;							                        //For System.in.read()
import com.icpdas.comm.*;					                  	//ICPDAS communication packages

public class Dio 
{
	public static void main(String[] args) throws java.io.IOException
	{
		int rev;
		int fd;
		int i=1;
		byte a[] = new byte[100];
		Comm comm1 = new Comm();		                     //ICPDAS communication object
		IoBuf i87kBuf = new IoBuf();				             //control matrix
		rev = comm1.open(1,9600,comm1.DATABITS_8,comm1.PARITY_NONE,comm1.STOPBITS_1);//open serial port		
		if (rev!=0) System.out.println("Open port error code : "+rev);
		else{
			i87kBuf.dwBuf[0] = 1;			                    //Serial Port
			i87kBuf.dwBuf[1] = 3;			                	 	//Module Address		
			i87kBuf.dwBuf[3] = 0;					                //check sum disable
			i87kBuf.dwBuf[4] = 10;			              		//Timeout 100ms
			i87kBuf.dwBuf[6] = 1;			                  	//Enable String Debug
		
			while(a[0]!=113) {
				i87kBuf.dwBuf[2] = 0x87063 ;			          //0x87063; //module name
				i87kBuf.dwBuf[5] =i;		                		//Digital Output Value
				rev = comm1.setDigitalOut(i87kBuf);
				System.out.println("szSend = "+ i87kBuf.szSend +" szReceive = "+i87kBuf.szReceive);
				if (rev!=0) System.out.println("Digital Out Error Code : "+ rev);
				i87kBuf.dwBuf[1] = 4;				               //Module Address		
				i87kBuf.dwBuf[2] = 0x87051 ;               //0x7060; //module name
				rev = comm1.getDigitalIn(i87kBuf);
				System.out.println("szSend = "+ i87kBuf.szSend +" szReceive = "+i87kBuf.szReceive);
				if (rev!=0) System.out.println("Digital In Error Code : "+ rev);
				else System.out.println("Digital In : "+ i87kBuf.dwBuf[5]);
				System.in.read(a);
				i=(i>8)?1:(i<<1);
			}
		}
		comm1.close(1);
		System.out.println("End of program");		
	}
}

