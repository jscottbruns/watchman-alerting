/* Digital Input DEMO for i7k series module ,ex:i-7051
 
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

   Dio7051.java

 	 v 0.0 2007.6.7 by moki matsushima
 
 	Copyright (c) 2003 ICPDAS, Inc. All  Rights Reserved.
*/  

import java.io.*;								      	//For System.in.read()
import com.icpdas.comm.*;								//ICPDAS communication packages

public class Dio7051 
{
	public static void main(String[] args) throws java.io.IOException
	{
		int rev;
		int fd;
		int i=1;
		byte a[] = new byte[100];
		Comm comm1 = new Comm();						//ICPDAS communication object
		IoBuf i7kBuf = new IoBuf();					//control matrix
		rev = comm1.open(2,9600,comm1.DATABITS_8,comm1.PARITY_NONE,comm1.STOPBITS_1);//open serial port
		if (rev!=0) System.out.println("Open port error code : "+rev);
		else{
			i7kBuf.dwBuf[0] = 2;							//Serial Port No
			i7kBuf.dwBuf[1] = 1;							//Address
			i7kBuf.dwBuf[2] = 0x7051 ;				//0x7051; //module name
			i7kBuf.dwBuf[3] = 0;							//check sum disable
			i7kBuf.dwBuf[4] = 10;							//Timeout 100ms
			i7kBuf.dwBuf[6] = 1;							//Enable String Debug
			while(a[0]!=113) {
				rev = comm1.getDigitalIn(i7kBuf);					//Get Digital Input Value from 7060
				System.out.println("szSend = "+ i7kBuf.szSend +" szReceive = "+i7kBuf.szReceive);
				if (rev!=0) System.out.println("Digital In Error Code : "+ rev);
				if (rev==0) System.out.println("Digital In : "+ i7kBuf.dwBuf[5]);
				System.in.read(a);
		  }
    }
		comm1.close(2);
		System.out.println("End of program");		
	}
}

