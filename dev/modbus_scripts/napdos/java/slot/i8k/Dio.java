/*	Digital Input/Output DEMO for i8k series module
  
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

import java.io.*;							            //For System.in.read()
import com.icpdas.slot.*;			       			//Slot I/O packages

public class Dio 
{
	public static void main(String[] args) throws java.io.IOException
	{
		int rev;
		long diValue;
		int i=1;
		byte a[] = new byte[100];

		Slot slot1 = new Slot();
		rev = slot1.open(1);						     //open slot 1 for digital in/out
		if (rev!=0) System.out.println("Open port error code : "+rev);
		else{
			while(a[0]!=113) {
				rev = slot1.setDigitalOut(1,8,i);
				if (rev!=0) System.out.println("Digital Out Error Code : "+ rev);
				diValue = slot1.getDigitalIn(1,8);
				System.out.println("Digital Out :" + i + "  Digital In : "+ diValue);
				System.in.read(a);
				i=(i>255)?1:(i<<1);
			}
		}
		slot1.close(1);
		System.out.println("End of program");		
	}
}

