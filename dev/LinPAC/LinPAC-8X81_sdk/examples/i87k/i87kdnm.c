//********************************************//
//   I-87124 for I-8000 series test program   //
//                                            //
//                                            //
//********************************************//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "msw.h"
#include <sys/ioctl.h>
#include "i87124dnm.h"

void StrToHex(unsigned char *DataBuf, unsigned char *Str);

void StrToHex(unsigned char *DataBuf, unsigned char *Str)
{
        int j = 0;
        char *tmpval;
        char *lpStop = NULL;

        tmpval = strtok(Str, " ");

        while(tmpval != NULL)
        {
                //printf("val : %s\n", tmpval);
                *(DataBuf) = strtol(tmpval, &lpStop, 16);
                tmpval = strtok(NULL, " ");
                DataBuf++;
        }
}

char Kbhit(void){
  int n=0;
  
  ioctl(0,FIONREAD,&n);
  if(n) return kbhit();
  
  return n;
}

int main(void)
{
  DWORD Ret;
  //DWORD Slot, Baud, MasterMACID;
  DWORD Slot, Baud, MasterMACID;
  char Command;
  int IsStartDevice[64];
  
  printf("\r\n");
  printf("/******************************/\r\n");
  printf("/*I87124 Library Version = %.02f*/\r\n",I87124_GetDLLVersion());
  printf("/******************************/\r\n");
  
  printf("I-87124 Slot(1~7):");
  scanf("%d",&Slot);

  printf("I-87124 master ID(0~63):");
  scanf("%d",&MasterMACID);

  printf("0:125Kbps\r\n");
  printf("1:250Kbps\r\n");
  printf("2:500Kbps\r\n");
  printf("I-87124 baud(0~2):");
  scanf("%d",&Baud);

  getch();

  Ret = I87124_ActiveModule();

  if(Ret)
  {
    printf("i-87124 Initial Error, Error Code = %d\r\n",Ret);
    printf("Press any key to exit program");
    getch();
    return 0;
  }
  else
  {
    printf("i-87124 Initial OK\r\n");
  }

  Ret = I87124_SetMasterMACID(Slot, MasterMACID);

  if(Ret)
  {
    printf("Configure i-87124 Master MACID error, Error Code = %d\r\n",Ret);
    printf("Press any key to exit program");
    getch();
    return 0;		
  }

  Ret = I87124_SetBaudRate(Slot, Baud);

  if(Ret)
  {
    printf("Configure i-87124 BaudRate error, Error Code = %d\r\n",Ret);
    printf("Press any key to exit program");
    getch();
    return 0;
  }

  Ret = I87124_ResetFirmware(Slot); 
  	
  if(Ret)
  {
    printf("Reset i-87124 Firmware error, Error Code = %d\r\n",Ret);
    printf("Press any key to exit program");
    getch();
    return 0;
  }
  
  sleep(3);	//wait for firmware reset

  printf("\r\n");

  printf("i-87124 Module Demo Item\r\n");
  printf("a. Get i-87124 Module Firmware Version:\r\n");//done
  printf("b. Get i-87124 Module Master MacID and BaudRate:\r\n");//done
  printf("c. Reset i-87124 Module Master MacID:\r\n");//done
  printf("d. Reset i-87124 Module BaudRate:\r\n");//done
  printf("e. The i-87124 Add Slave Device:\r\n");
  printf("f. The i-87124 Remove Slave Device:\r\n");
  printf("g. Configure Poll Connection:\r\n");
  printf("h. Configure BitStrobe Connection:\r\n");
  printf("i. Configure COS/Cyclic Connection:\r\n");
  printf("j. Remove I/O Connection:\r\n");
  printf("k. The i-87124 Startup Slave Device:\r\n");
  printf("l. The i-87124 Stop Slave Device:\r\n");
  printf("m. Get Slave Attribute:\r\n");
  printf("n. Set Slave Attribute:\r\n");
  printf("o. Write I/O Output Data:\r\n");
  printf("p. Read I/O Input Data:\r\n");
  printf("y. Show i-87124 Demo Item:\r\n");  
  printf("z. Shutdown and exit:\r\n");
    
  memset(IsStartDevice, 0, (sizeof(int)*64));
  sleep(1);

  while(1)
  {
    {
      Command = Kbhit();
      
      if(Command)
      {
        switch(Command)
        {
          case 'a':	//Get i-87124 Module Firmware Version
          {
	    DWORD ret;
            WORD firm_ver = 0;
        
	    getch();

	    ret = I87124_GetFirmwareVersion(Slot, &firm_ver);

	    if(ret)
	    {
	      printf("Get i-87124 Firmware Version Failure : Error Code = %d\r\n",ret);	
            }
            else
            {		
              printf("Slot %d i-87124 Firmware Version : 0x%x\r\n", Slot, firm_ver);
            }
          }
          break;

          case 'b':	//Get i-87124 Module Master MacID and BaudRate
	  {
	    DWORD ret;
	    BYTE master_id, baud;

	    getch();

	    ret = I87124_GetMasterStatus(Slot);

	    if(ret)
	    {
	      printf("Get i-87124 Master Online Failure : Error Code = %d\r\n",ret);	
	    }
	    else
	    {		
              ret = I87124_GetMasterMACID(Slot, &master_id);

              if(ret)
              {
                printf("Get i-87124 Master MacID Failure : Error Code = %d\r\n",ret);
              }
              else
              {
                printf("Slot %d i-87124 Master MacID : %d\r\n", Slot, master_id);
              }

	      ret = I87124_GetBaudRate(Slot, &baud);

              if(ret)
              {
                printf("Get i-87124 BaudRate Failure : Error Code = %d\r\n",ret);
              }
              else
              {
                switch(baud)
                {
                  case 0:     //Get i-87124 Module Firmware Version
                  {
                    printf("Slot %d i-87124 BaudRate : 125 Kbps\r\n", Slot);
                  }
                  break;

                  case 1:
                  {
                    printf("Slot %d i-87124 BaudRate : 250 Kbps\r\n", Slot);
                  }
                  break;

                  case 2:
                  {
                    printf("Slot %d i-87124 BaudRate : 500 Kbps\r\n", Slot);
                  }
                  break;
                }
              }
	    }	
	  }
          break;
	  
	  case 'c':     //Reset i-87124 Module Master MacID
          {
            DWORD ret;

            printf("I-87124 master ID(0~63):");
            scanf("%d",&MasterMACID);
            getch();

            ret = I87124_SetMasterMACID(Slot, MasterMACID);

            if(ret)
            {
              printf("Set i-87124 Master MacID Failure : Error Code = %d\r\n",ret);
              printf("Press any key to exit program");
              getch();
              return 0;
            }

	    ret = I87124_ResetFirmware(Slot);

            if(ret)
            {
              printf("Reset i-87124 Firmware error, Error Code = %d\r\n",ret);
              printf("Press any key to exit program");
              getch();
              return 0;
            }

            sleep(3);     //wait for firmware reset

            printf("Set Slot %d i-87124 Master MacID : %d OK\r\n", Slot, MasterMACID);
	  }
	  break;

	  case 'd':	//Reset i-87124 Module BaudRate
	  {
            DWORD ret;

	    printf("0:125Kbps\r\n");
	    printf("1:250Kbps\r\n");
	    printf("2:500Kbps\r\n");
	    printf("I-87124 baud(0~2):");
	    scanf("%d",&Baud);

            getch();

	    ret = I87124_SetBaudRate(Slot, Baud);

	    if(ret)
            {
              printf("Set i-87124 BaudRate Failure : Error Code = %d\r\n",ret);
              printf("Press any key to exit program");
              getch();
              return 0;
            }

	    ret = I87124_ResetFirmware(Slot);

	    if(ret)
	    {
	      printf("Reset i-87124 Firmware error, Error Code = %d\r\n",ret);
              printf("Press any key to exit program");
              getch();
              return 0;
            }

	    sleep(3);     //wait for firmware reset	

            printf("Set Slot %d i-87124 Baudrate OK\r\n", Slot, Baud);
          }	
	  break;

	  case 'e':     //The i-87124 Add Slave Device
          {
            DWORD ret;
            DWORD slave_id;

	    printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);	

            getch();

            ret = I87124_AddDevice(Slot, slave_id, 2500);

            if(ret)
            {
              printf("The i-87124 Add Slave(%d) Failure : Error Code = %d\r\n", slave_id, ret);
            }
            else
            {
              printf("Slot %d i-87124 Add Slave(%d) OK\r\n", Slot, slave_id);
            }
          }
          break;

	  case 'f':     //The i-87124 Remove Slave Device
          {
            DWORD ret;
	    DWORD slave_id;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            getch();

            ret = I87124_RemoveDevice(Slot, slave_id);

            if(ret)
            {
              printf("The i-87124 Add Slave(%d) Failure : Error Code = %d\r\n", slave_id, ret);
            }
            else
            {
              printf("Slot %d i-87124 Add Slave(%d) OK\r\n", Slot, slave_id);
            }
          }
          break;	

	  case 'g':     //Configure Poll Connection
          {
            DWORD ret;
            DWORD slave_id;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            getch();

            ret = I87124_AddIOConnection(Slot, slave_id, ConType_Poll, 2, 2, 2500);

            if(ret)
            {
              printf("The i-87124 Configure Poll Connection Failure : Error Code = %d\r\n", ret);
            }
            else
            {
              printf("Slot %d i-87124 Configure Poll Connection OK\r\n", Slot);
            }
          }
          break;		

	  case 'h':     //Configure BitStrobe Connection
          {
            DWORD ret;
            DWORD slave_id;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            getch();

            ret = I87124_AddIOConnection(Slot, slave_id, ConType_BitStrobe, 2, 2, 2500);

            if(ret)
            {
              printf("The i-87124 Configure BitStrobe Connection Failure : Error Code = %d\r\n", ret);
            }
            else
            {
              printf("Slot %d i-87124 Configure BitStrobe Connection OK\r\n", Slot);
            }
          }
          break;

	  case 'i':     //Configure COS/Cyclic Connection
          {
            DWORD ret;
            DWORD slave_id, connect_type;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            printf("0:COS Connection\r\n");
            printf("1:Cyclic Connection\r\n");
            printf("Connection Type(0~1):");
            scanf("%d",&connect_type);

            getch();

	    if(connect_type == 0)
            {
	      ret = I87124_AddIOConnection(Slot, slave_id, ConType_COS, 2, 2, 2500);

	      if(ret)
              {
                printf("The i-87124 Configure COS Connection Failure : Error Code = %d\r\n", ret);
              }
              else
              {
                printf("Slot %d i-87124 Configure COS Connection OK\r\n", Slot);
              }	
            }
            else if(connect_type == 1)
            {
	      ret = I87124_AddIOConnection(Slot, slave_id, ConType_Cyclic, 2, 2, 2500);

	      if(ret)
              {
                printf("The i-87124 Configure Cyclic Connection Failure : Error Code = %d\r\n", ret);
              }
              else
              {
                printf("Slot %d i-87124 Configure Cyclic Connection OK\r\n", Slot);
              }	
            }
	    else
	    {
	      printf("Unknow Connection Type\r\n");
	    }
          }
          break;

	  case 'j':	//Remove I/O Connection
	  {
	    DWORD ret;
            DWORD slave_id, connect_type;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            printf("1:Poll Connection\r\n");
            printf("2:BitStrobe Connection\r\n");
            printf("3:COS Connection\r\n");
            printf("4:Cyclic Connection\r\n");
	    printf("Connection Type(1~4):");
            scanf("%d",&connect_type);

            getch();

	    if(connect_type < 1 || connect_type > 4)
	    {
	      printf("Unknow Connection Type\r\n");
	    }
	    else
	    {	
              ret = I87124_RemoveIOConnection(Slot, slave_id, connect_type);

              if(ret)
              {
                printf("The i-87124 Remove DeviceNet I/O Connection Failure : Error Code = %d\r\n", ret);
              }
              else
              {
                printf("Slot %d i-87124 Remove DeviceNet I/O Connection OK\r\n", Slot);
              }
            }
	  }
	  break;

	  case 'k':     //The i-87124 Startup Slave Device
          {
            DWORD ret;
            DWORD slave_id;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            getch();

            ret = I87124_StartDevice(Slot, slave_id);

            if(ret)
            {
              printf("The i-87124 Startup Slave(%d) Failure : Error Code = %d\r\n", slave_id, ret);
            }
            else
            {
	      IsStartDevice[slave_id] = 1;	
              printf("Slot %d i-87124 Startup Slave(%d) Device OK\r\n", Slot, slave_id);
            }
          }
          break;

	  case 'l':     //The i-87124 Stop Slave Device
          {
            DWORD ret;
            DWORD slave_id;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            getch();

            ret = I87124_StopDevice(Slot, slave_id);

            if(ret)
            {
              printf("The i-87124 Stop Slave(%d) Failure : Error Code = %d\r\n", slave_id, ret);
            }
            else
            {
	      IsStartDevice[slave_id] = 0;	
              printf("Slot %d i-87124 Stop Slave(%d) OK\r\n", Slot, slave_id);
            }
          }
          break;

	  case 'm':     //Get Slave Attribute
          {
            DWORD ret;
            DWORD slave_id, class, instance, attribute, index;
            BYTE get_value[128];
            WORD length;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

	    printf("Please input class ID(hex):");
            scanf("%x",&class);

            printf("Please input instance ID(hex):");
            scanf("%x",&instance);

            printf("Please input attribute ID(hex):");
            scanf("%x",&attribute);

            getch();

	    if(!IsStartDevice[slave_id])
	    {
	      printf("Please startup Slave(%d) Device First\r\n", slave_id);	
	    }	
	    else
	    {
	      ret = I87124_GetAttribute(Slot, slave_id, class, instance, attribute);


	      if(ret)
	      {
		printf("The i-87124 Send GetAttribute Request To Slave(%d) Failure : Error Code = %d\r\n", slave_id, ret);
	      }
              else
              {
	        ret = I87124_IsGetAttributeOK(Slot, slave_id);

		if(ret)
		{
		  printf("The i-87124 Get Slave(%d) Attribute Failure : Error Code = %d\r\n", slave_id, ret);	
		}
		else
		{
		  memset(get_value, 0, (sizeof(BYTE)*64));

		  ret = I87124_GetAttributeValue(Slot, slave_id, &length, get_value);

		  if(ret)
		  {
		    printf("The i-87124 Get Slave(%d) Attribute Value Failure : Error Code = %d\r\n", slave_id, ret);	
		  }
		  else
		  {
		    printf("Slot %d i-87124 Get Slave(%d) Attribute Value OK\r\n", Slot, slave_id);

                    printf("Length : %d\n",length);
                    printf("Data : ");
                    for(index = 0; index < length; index++)
                    {
                      printf("%02x ", get_value[index]);
                    }
                    printf("\r\n");	
		  }
		}
              }
            }
	  }
          break;

	  case 'n':     //Set Slave Attribute
          {
            DWORD ret;
            DWORD slave_id, class, instance, attribute, index, length;
	    BYTE tmpdata[64] = {0}, data[64] = {0};

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            printf("Please input class ID(hex):");
            scanf("%x",&class);

            printf("Please input instance ID(hex):");
            scanf("%x",&instance);

            printf("Please input attribute ID(hex):");
            scanf("%x",&attribute);

	    printf("The Length of Attribute Data:");
            scanf("%d",&length);

            getch();

	    printf("The Set Attribute Data(hex):");

            if( fgets( tmpdata, 10, stdin ) == NULL )
            {
              ret = I87124_CloseModule();
              printf("ShutdownMaster OK\r\n");
              return 0;
            }

            StrToHex(data, tmpdata);

            if(!IsStartDevice[slave_id])
            {
              printf("Please startup Slave(%d) Device First\r\n", slave_id);
            }
            else
            {
	      ret = I87124_SetAttribute(Slot, slave_id, class, instance, attribute, length, data);

	      if(ret)
              {
                printf("The i-87124 Send SetAttribute Request To Slave(%d) Failure : Error Code = %d\r\n", slave_id, ret);
              }
              else
              {
                ret = I87124_IsSetAttributeOK(Slot, slave_id);

		if(ret)
                {
                  printf("The i-87124 Set Slave(%d) Attribute Failure : Error Code = %d\r\n", slave_id, ret);
                }
                else
                {
                  printf("The i-87124 Set Slave(%d) Attribute OK\r\n", slave_id);
		}
	      }	
	    }
	  }
	  break;
	  
	  case 'o':	//Write I/O Output Data
	  {
	    DWORD ret;
            DWORD slave_id, connect_type, length;
	    BYTE tmpdata[64] = {0}, data[64] = {0};

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            printf("1:Poll Connection\r\n");
            printf("2:BitStrobe Connection\r\n");
            printf("3:COS Connection\r\n");
            printf("4:Cyclic Connection\r\n");
            printf("Connection Type(1~4):");
            scanf("%d",&connect_type);

	    printf("The Length of Output Data:");
	    scanf("%d",&length);
            getch();

	    printf("Write I/O Output Data(hex):");

	    if( fgets( tmpdata, 10, stdin ) == NULL )
            {
	      ret = I87124_CloseModule(); 	
	      printf("ShutdownMaster OK\r\n");
              return 0;
	    }

            StrToHex(data, tmpdata);	

	    if(connect_type < 1 || connect_type > 4)
            {
              printf("Unknow Connection Type\r\n");
            }
            else
            {
	      ret = I87124_WriteOutputData(Slot, slave_id, connect_type, length, data);

	      if(ret)
              {
                printf("The i-87124 Write I/O Output Data Failure : Error Code = %d\r\n", ret);
              }
              else
              {
                printf("Slot %d i-87124 Write I/O Output Data OK\r\n", Slot);
 	      }
	    }
	  }
	  break;

	  case 'p':	//Read I/O Input Data
	  {
	    DWORD ret;
            DWORD slave_id, connect_type;
	    WORD length;
            BYTE data[64] = {0};
	    int i;

            printf("Slave Device MAC ID(0~63):");
            scanf("%d",&slave_id);

            printf("1:Poll Connection\r\n");
            printf("2:BitStrobe Connection\r\n");
            printf("3:COS Connection\r\n");
            printf("4:Cyclic Connection\r\n");
            printf("Connection Type(1~4):");
            scanf("%d",&connect_type);

            getch();	

	    if(connect_type < 1 || connect_type > 4)
            {
              printf("Unknow Connection Type\r\n");
            }
            else
            {
	      ret = I87124_ReadInputData(Slot, slave_id, connect_type, &length, data);	
	
	      if(ret)
	      {
		printf("The i-87124 Read I/O Input Data Failure : Error Code = %d\r\n", ret);
	      }
	      else
	      {		
                printf("Slot %d i-87124 Read I/O Input Data OK\r\n", Slot);

	        for(i = 0; i < length; i++)
                {
                  printf("Data[%d] : 0x%x ", i, data[i]);
                }
	      }

              printf("\r\n");	
	    }
	  }
	  break;

	  case 'y':
	  {
	    printf("a. Get i-87124 Module Firmware Version:\r\n");//done
	    printf("b. Get i-87124 Module Master MacID and BaudRate:\r\n");//done
	    printf("c. Reset i-87124 Module Master MacID:\r\n");//done
	    printf("d. Reset i-87124 Module BaudRate:\r\n");//done
	    printf("e. The i-87124 Add Slave Device:\r\n");
	    printf("f. The i-87124 Remove Slave Device:\r\n");
	    printf("g. Configure Poll Connection:\r\n");
	    printf("h. Configure BitStrobe Connection:\r\n");
	    printf("i. Configure COS/Cyclic Connection:\r\n");
	    printf("j. Remove I/O Connection:\r\n");
	    printf("k. The i-87124 Startup Slave Device:\r\n");
	    printf("l. The i-87124 Stop Slave Device:\r\n");
	    printf("m. Get Slave Attribute:\r\n");
	    printf("n. Set Slave Attribute:\r\n");
	    printf("o. Write I/O Output Data:\r\n");
	    printf("p. Read I/O Input Data:\r\n");
	    printf("y. Show i-87124 Demo Item:\r\n");
	    printf("z. Shutdown and exit:\r\n");
	  }
	  break;

	  case 'z':
	  {
	    DWORD ret;
            ret = I87124_CloseModule();
            printf("ShutdownMaster OK\r\n");
            return 0;
          }
          break;

          default:
          {	
            printf("Unknow Command = %X\r\n",Command);
          }
          break;
        }
      }
    }
  }
}
