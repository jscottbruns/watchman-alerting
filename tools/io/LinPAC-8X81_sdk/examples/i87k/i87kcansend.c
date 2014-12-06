#include "msw.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include "i87120can.h"

int Kbhit(void){
  int n;
  ioctl(0,FIONREAD,&n);
  return n;
}

int main(void)
{
  char Mode=1,RTR=0,DataLen=8,Data[8]={0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89};
  int Ret,i,SlotNo;
  long MsgID=0x12345678L;
  
  //Open COM 1
  Ret = Open_Slot(0);
  if (Ret > 0) {
    printf("open Slot failed!\n");
    return (-1);
  }
		
  Ret = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
  if (Ret > 0) {
    printf("open port failed!\n");
    return (-1);
  }
  
  //Select the slot where the I-87KCAN is plugged.
  printf("Input the slot (1~7) where the I-87KCAN is plugged: ");
  scanf("%d",&SlotNo);
  if (SlotNo<1 || SlotNo>7){
    printf("Slot number error, default to slot 1.\n");
    SlotNo=1;
  }
  printf("Baud is set to 125K\n");
  ChangeToSlot(SlotNo);
  
  //Initiate the I-87KCAN module
  Ret=I87KCANInit(125000L,0,0,0x00000000L,0xffffffffL,0,0);
  if (Ret){
    printf("Init error Code=%d\n",Ret);
    return (-1);	
  }
  printf("Start to send CAN messages.(press Enter key to exit...)\n");
  while(!Kbhit()){
    //Send CAN message per second
    Ret=SendCANMsg(Mode,MsgID,RTR,DataLen,Data);
    if (Ret){
      printf("Send Message Error, Error Code=%d.\n",Ret);
      break;
    }
    sleep(1);
  }
  
  //Recover the COM 0 setting
  Close_Com(COM1);
  Close_SlotAll();
  printf("Exit Program.\n\r");
  return 0;
}
