SimplyModbus6.3.6.zip includes these files:

Simply Modbus 6.3.6.exe  - program 
serpdrv                - serial port driver
readme.txt             - this file

INSTRUCTIONS
***** Context help is also available by pressing Ctrl-H while in the program *****
*****  information is displayed about items as the cursor hovers over them   *****

See also http:/www.simplymodbus.ca/manual.htm

1) Unzip the files into the same folder.

2) Double-click Simply Modbus 6.3.6.exe to start the program.

3) Enter the input info: 
      eg.  mode = RTU
           COM port = 1 to 99
           port settings = 19200 baud, 8 data bits, 1 stop bit, no parity 
           Slave ID = 10 
           First Register = 40001 
           No. of Registers = 10
   The Command to be sent will appear in "Request".

4) Press the SEND button.  All bytes received will appear in "Response".
   The response time is shown in 0.1 increments. The response progress bar fills as the the time approached the "fail in" time entered.
   If no answer appears within the "fail in" time, increase the "fail in" time and press SEND again.

5) Set the format of the received data.
   check or uncheck 'High byte/Low byte'
   check or uncheck 'Low word/High word'
   Data Types: 32bit Float - 32bit IEEE Floating Point
               32bit UINT  - Unsigned Integer (0 to 4.3 billion)
               32bit INT   - Signed Integer   (-2.1 to 2.1 billion)
               16bit UINT  - Unsigned Integer (0 to 65535)
               16bit INT   - Signed Integer   (-32768 to 32767)
               2chString   - 16bit - 2 character STRING
               4chString   - 32bit - 4 character STRING
               6chString   - 48bit - 6 character STRING
               8chString   - 64bit - 8 character STRING
               12chString  - 96bit -12 character STRING
               16chString  - 128bit-16 character STRING
               8 boolean   - 8 bit - 8 status ON/OFF

--- Auto set ---

The automatic settings are shown inside the Auto set box.
These settings are automatically set to these defaults whenever "First Register" is changed.

First Register   Function Code   Register Size   Offset
00001-10000           1           1 bit coils        1
10001-30000           2           1 bit coils    10001
30001-40000           4           16 bit regs    30001
40001+                3           16 bit regs    40001

Enron exceptions:
32        (events)    3           32 bit regs        0
701-799   (history)   3           32 bit regs        0
1001-1999 (boolean)   1           1 bit coils        0
3001-3999 (short int) 3           16 bit regs        0
5001-5999 (long int)  3           32 bit regs        0
7001-7999 (floats)    3           32 bit regs        0

For Simply Write Modbus:
First Register   Function Code   Register Size   Offset
00001-10000           5(15*)      1 bit coils        1
10001-30000           5(15*)      1 bit coils    10001
30001-40000           6(16*)      16 bit regs    30001
40001+                6(16*)      16 bit regs    40001
 *Function codes 15 and 16 are used when writing multiple values.

The "send continuously" check box may be turned on. This will have the effect of repeatedly pressing the SEND button. Increase "pause between sends" in seconds to slow down the rate the requests are sent.

If RTS delay is required, for communication through MDS4310A and MDS4710B radios for example, check the RTS delay check box. Enter the 'ON' and 'OFF' delay in milliseconds.

--- Saved Configurations ---
The settings can be saved to text files with the SAVE CFG button and resored with the RESTORE CFG button.

The format of the file is csv (comma separated values).
1st column contains the setting descriptions, 2nd column contains the setting values:
serial port      0=COM1, 1=COM2 etc
baud rate
data bits
stop bits        0=1bit, 1=1.5bits, 2=2bits			
parity           0=none , 1=odd, 2=even, 3=mark, 4=space
slave ID
1st register
No. of registers or No.of Events (for Events) or Record No.(for History)
Record size      (for History)
mode             0=RTU, 1=ASCII
byte order       0=Low byte first, 1=High byte first
word order       0=Low word first, 1=High word first
autoset          1=Auto set advanced settings when 1st Register changes
Function Code
register size    0=1 bit coils, 1=16bit registers, 2=32bit registers
offset
Events           0=not Events, 1= Event Poll
History          0=not History, 1= History Poll
2 byte addr      0=1byte addr, 1=2byte addr
RTS delay        0=off, 1=on
ON delay         milliseconds
OFF delay        milliseconds
Read twice       0=off, 1=try again if timeout
fail in          seconds until poll failure if expected bytes are not received
pause            seconds from the response to the next request when send continuously is checked.

3rd column contains data formats.
  0=32bit Float - 32bit IEEE Floating Point
  1=32bit UINT  - Unsigned Integer (0 to 4.3 billion)
  2=32bit INT   - Signed Integer   (-2.1 to 2.1 billion)
  3=16bit UINT  - Unsigned Integer (0 to 65535)
  4=16bit INT   - Signed Integer   (-32768 to 32767)
  5=2chString   - 16bit - 2 character STRING
  6=4chString   - 32bit - 4 character STRING
  7=6chString   - 48bit - 6 character STRING
  8=8chString   - 64bit - 8 character STRING
  9=12chString  - 96bit -12 character STRING
 10=16chString  - 128bit-16 character STRING
 11=8 boolean   - 8 bit - 8 status ON/OFF

4th column contains data notes.

5th column contains data register numbers.

6th column contains data values.

7th column contains the latest response received.

--------------
Visit www.simplymodbus.ca for updates and detailed information on how the modbus protocol works.

See www.simplymodbus.ca/manual.htm for a detailed manual.

Send all questions and comments to info@simplymodbus.ca