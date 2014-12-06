#include "msw.h"
#include <sys/ioctl.h>
#include "i87120can.h"

typedef struct {
  unsigned char Mode;
  unsigned char RTR;
  unsigned char DataLen;
  unsigned char Data[8];
  unsigned long MsgID;
}PacketStruct;

int Kbhit(void){
  int n;
  ioctl(0,FIONREAD,&n);
  return n;
}

int main(void)
{
  char CANRegStatus,OverFlowStatus,OldCANRegStatus=0,OldOverFlowStatus=0;
  int Ret,SlotNo,i,Temp;
  PacketStruct RxMsg;
  
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
  printf("Input the slot (1~7) where the I-87KCAN is plugged.");
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
  printf("Start to receive CAN messages.(press Enter key to exit...)\n");
  while(!Kbhit()){
    //Get CAN Message
    Ret=GetCANMsg(&RxMsg.Mode,&RxMsg.MsgID,&RxMsg.RTR,&RxMsg.DataLen,RxMsg.Data);
    if (Ret){
      if (Ret!=CAN87K_FIFO_EMPTY){
      	printf("Receive Message Error, Error Code=%d.\n",Ret);
      	break;
      }	
    }
    else{
      if (RxMsg.RTR){
      	printf("%s,ID=%lx,RTR=%d,Dlen=%d\n",
               RxMsg.Mode?"2.0B":"2.0A",RxMsg.MsgID,RxMsg.RTR,RxMsg.DataLen);
      }
      else{
        printf("%s,ID=%lx,RTR=%d,Dlen=%d",
               RxMsg.Mode?"2.0B":"2.0A",RxMsg.MsgID,RxMsg.RTR,RxMsg.DataLen);
        if (RxMsg.DataLen>0){
          printf(",Data=");
          for (i=0;i<RxMsg.DataLen;i++){
            printf("%02X",RxMsg.Data[i]);
            if (i!=RxMsg.DataLen-1) printf(",");
          }
        }
        printf("\n");
      }
    }
    
    //Check I-87KCAN State
    Ret=GetStatus(&CANRegStatus,&OverFlowStatus);
    if (Ret){
      if (Ret!=CAN87K_FIFO_EMPTY){
      	printf("Receive Message Error, Error Code=%d.\n",Ret);
      	break;
      }
    }
    if (CANRegStatus!=OldCANRegStatus){
    	printf("CANRegStatus=%02x.\n",CANRegStatus);
    	OldCANRegStatus=CANRegStatus;
    }
    if (OverFlowStatus!=OldOverFlowStatus){
    	printf("OverFlowStatus=%02x.\n",OverFlowStatus);
    	OldOverFlowStatus=OverFlowStatus;
    }
  }
  //Recover the COM 0 setting
  Close_Com(COM1);
  Close_SlotAll();
  printf("Exit Program.\n");
  return 0;
}
