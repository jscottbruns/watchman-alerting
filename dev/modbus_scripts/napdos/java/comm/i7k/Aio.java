/* Analog Input/Output DEMO for i7k series module
 
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
	
	 Aio.java
 
   
 	 v 0.0 2003.8.7 by moki matsushima
 
 	 Copyright (c) 2003 ICPDAS, Inc. All  Rights Reserved.
 */ 

import java.io.*;
import com.icpdas.comm.*;

public class Aio 
{
	public static void main(String[] args) throws java.io.IOException
	{
		int rev;
		int fd,ao=1;
		byte a[] = new byte[100];
		Comm comm1 = new Comm();
		IoBuf i7kBuf = new IoBuf();
		rev = comm1.open(1,9600,comm1.DATABITS_8,comm1.PARITY_NONE,comm1.STOPBITS_1);
		if (rev!=0) System.out.println("Open port error code : "+rev);
		else{
			i7kBuf.dwBuf[0] = 1;                //Serial Port
			i7kBuf.dwBuf[1] = 3;                //Address
			i7kBuf.dwBuf[3] = 0;                //check sum disable
			i7kBuf.dwBuf[4] = 10;               //Timeout 100ms
			i7kBuf.dwBuf[6] = 1;                //Enable String Debug
			while(a[0]!=113) {
				i7kBuf.dwBuf[2] = 0x7016 ;        //0x7016; //module name
				i7kBuf.fBuf[0] = ao;
				rev = comm1.setAnalogOut(i7kBuf);
				if (rev!=0) System.out.println("Analog Out Error Code : "+ rev);
				System.out.println("szSend = "+ i7kBuf.szSend +" szReceive = "+i7kBuf.szReceive);
				i7kBuf.dwBuf[1] = 3;              //Address
				i7kBuf.dwBuf[2] = 0x7012 ;        //0x7012; //module name
				rev = comm1.getAnalogIn(i7kBuf);
				if (rev!=0) System.out.println("Analog In Error Code : "+ rev);
				System.out.println("szSend = "+ i7kBuf.szSend +" szReceive = "+i7kBuf.szReceive);
				System.out.println("Analog In Value : "+i7kBuf.fBuf[0]);
				System.in.read(a);
				ao=(ao>128)?1:(ao<<1);
			}
		}
		comm1.close(1);
		System.out.println("End of program");		
	}
}

