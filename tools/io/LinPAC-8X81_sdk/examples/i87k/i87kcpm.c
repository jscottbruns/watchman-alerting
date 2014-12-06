//********************************************//
//   I-87123 for I-8000 series test program   //
//                                            //
//                                            //
//********************************************//

#include <stdio.h>
#include <stdlib.h>
#include "msw.h"
#include <sys/ioctl.h>
#include "i87123cpm.h"

char Kbhit(void){
  int n=0;
  
  ioctl(0,FIONREAD,&n);
  if(n) return kbhit();
  
  return n;
}

int main(void)
{
  int ret;
  int slot,baud;
  char command;
  
  printf("\r\n");
  printf("/************************/\r\n");
  printf("/*Library Version = %.02f*/\r\n",I87123_GetVersion());
  printf("/************************/\r\n");
  
  printf("I-87123 slot(0~7):");
  scanf("%d",&slot);
  printf("0:10Kbps\r\n");
  printf("1:20Kbps\r\n");
  printf("2:50Kbps\r\n");
  printf("3:125Kbps\r\n");
  printf("4:250Kbps\r\n");
  printf("5:500Kbps\r\n");
  printf("6:800Kbps\r\n");
  printf("7:1000Kbps\r\n");
  printf("I-87123 baud(0~7):");
  scanf("%d",&baud);
  getch();

  ret = Open_Slot(SLOT0);
  if(ret){
    printf("Open_Slot error, Error Code = %d\r\n",ret);
    printf("Press any key to exit program");
    getch();
    return 0;
  }

  //slot = slot + 1;
  
  //ChangeToSlot(slot);
  ret = I87123_Configure((unsigned char)baud, slot);
  if(ret){
    printf("Configure error, Error Code = %d\r\n",ret);
    printf("Press any key to exit program");
    getch();
    return 0;
  }
  else{
    printf("Configure OK\r\n");
  }
  
  printf("a. Add Node:\r\n");
  printf("b. Remove Node:\r\n");
  printf("c. Get state:\r\n");
  printf("d. Change state:\r\n");
  printf("e. Guarding:\r\n");
  printf("f. Change EMCY ID:\r\n");
  printf("g. Change SYNC ID:\r\n");
  printf("h. Send SYNC:\r\n");
  printf("i. ReadSDO:\r\n");
  printf("j. WriteSDO:\r\n");
  printf("k. Install PDO:\r\n");
  printf("l. Remove PDO:\r\n");
  printf("m. Set PDOResponse:\r\n");
  printf("n. Remote PDO:\r\n");
  printf("o. Write PDO:\r\n");
  printf("p. Set Tx type:\r\n");
  printf("q. Shutdown and exit:\r\n");
    
  while(1){
    {
      int i;
      unsigned char node;
      unsigned short cobid;
      unsigned char len;
      unsigned char rdata[40];
      ret = I87123_MsgResponse(&node, &cobid, &len, rdata);
      if(ret != I87123_EMPTY){
        if(ret == I87123_OK){
          printf("PDO: ");
          printf("[%X] ",cobid);
          for(i=0;i<len;i++){
            printf("[%X]",rdata[i]);
          }
          printf("\r\n");
        }
        else if(ret == I87123_EMCY){
          printf("EMCY: ");
          printf("[%X] ",cobid);
          for(i=0;i<len;i++){
            printf("[%X]",rdata[i]);
          }
          printf("\r\n");
        }
        else{
          printf("Node[%d] Other Err: Error Code: %d\r\n",node,ret);
        }
      }
    }

    command = Kbhit();
    if(command){
      switch(command){
      case 'a':
      {
        int node=0;
        
        printf("Add node:Press node number(1~127):");
        scanf("%d",&node);
        getch();
        ret = I87123_AddNode((unsigned char)node);
        if(ret){
          printf("AddNode Error Code = %d\r\n",ret);
        }
        else{
          printf("Add Node OK\r\n");
        }
      }
      break;
      case 'b':
      {
        int node=0;
        
        printf("Remove node:Press node number(1~127):");
        scanf("%d",&node);
        getch();
        ret = I87123_RemoveNode((unsigned char)node);
        if(ret){
          printf("RemoveNode Error Code = %d\r\n",ret);
        }
        else{
          printf("RemoveNode OK\r\n");
        }
      }
      break;
      case 'c':
      {
        int node=0;
        unsigned char state;
        
        printf("Get State:Press node number(1~127):");
        scanf("%d",&node);
        getch();
        ret = I87123_GetState((unsigned char)node,&state);
        if(ret){
          printf("GetState Error Code = %d\r\n",ret);
        }
        else{
          printf("GetState OK: state is [%x] ",state);
          switch(state){
            case 4:
              printf("Stop Mode\r\n");
            break;
            case 5:
              printf("Operational Mode\r\n");
            break;
            case 127:
              printf("Pre-Operational Mode\r\n");
            break;
          }
        }
      }
      break;
      case 'd':
      {
        int node=0,state=0;
        
        printf("Change State:Press node number(0~127, node 0 means all slaves):");
        scanf("%d",&node);
        
        printf("1:Operational mode\r\n");
        printf("2:Stop mode\r\n");
        printf("128:Pre-Operational mode\r\n");
        printf("129:Reset Slave\r\n");
        printf("Change State:Press state:");
        scanf("%d",&state);
        getch();
        ret = I87123_ChangeState((unsigned char)node,(unsigned char)state);
        if(ret){
          printf("ChangeState Error Code = %d\r\n",ret);
        }
        else{
          printf("ChangeState OK\r\n");
        }
      }
      break;
      case 'e':
      {
        int node=0,life=0,guard=0;
        
        printf("Guarding:Press node number(1~127):");
        scanf("%d",&node);
        printf("Guarding:Press guard time(0~65535 ms):");
        scanf("%u",&guard);
        printf("Guarding:Press life time(0~255):");
        scanf("%d",&life);
        getch();
        ret = I87123_Guarding((unsigned char)node,(unsigned short)guard,(unsigned char)life);
        if(ret){
          printf("Guarding Error Code = %d\r\n",ret);
        }
        else{
          printf("Guarding OK\r\n");
        }
      }
      break;
      case 'f':
      {
        int node=0,cobid=0;
        
        printf("Change EMCY ID:Press node number(1~127):");
        scanf("%d",&node);
        printf("Change EMCY ID:Press EMCY COB ID(hex):");
        scanf("%x",&cobid);
        getch();
        ret = I87123_ChaneEMCYID(node, cobid);
        if(ret){
          printf("Change EMCY ID Error Code = %d\r\n",ret);
        }
        else{
          printf("Change EMCY ID OK\r\n");
        }
      }
      break;
      case 'g':
      {
        int node=0,cobid=0;
        
        printf("Change SYNC ID:Press node number(1~127):");
        scanf("%d",&node);
        printf("Change SYNC ID:Press SYNC COB ID(hex):");
        scanf("%x",&cobid);
        getch();
        ret = I87123_ChaneSYNCID(node, cobid);
        if(ret){
          printf("Change SYNC ID Error Code = %d\r\n",ret);
        }
        else{
          printf("Change SYNC ID OK\r\n");
        }
      }
      break;
      case 'h':
      {
        int cyclic=0,cobid=0,timer=0;
        
        printf("SYNC:Press SYNC COB ID(hex):");
        scanf("%x",&cobid);
        printf("SYNC:Press cyclic type(0:non-cyclic, 1:cyclic):");
        scanf("%d",&cyclic);
        if(cyclic){
          printf("SYNC:Press cyclic timer(0~65535 ms):");
          scanf("%u",&timer);
        }
        else{
          timer = 0;
        }
        getch();
        
        ret = I87123_SendSYNC(cobid, (unsigned char)cyclic, (unsigned short)timer);
        if(ret){
          printf("SendSYNC Error Code = %d\r\n",ret);
        }
        else{
          printf("SendSYNC OK\r\n");
        }
      }
      break;
      case 'i':
      {
        int i;
        unsigned char rdata[40],len;
        int node=0,index=0,subindex=0;
        
        printf("Read SDO:Press node number(1~127):");
        scanf("%d",&node);
        printf("Read SDO:Press index(hex):");
        scanf("%x",&index);
        printf("Read SDO:Press sub-index(hex):");
        scanf("%x",&subindex);
        getch();
        ret = I87123_ReadSDO((unsigned char)node,index,(unsigned char)subindex,&len,rdata);
        if(ret){
          printf("ReadSDO Error Code = %d\r\n",ret);
        }
        else{
          printf("SDO data: ");
          for(i=0;i<len;i++){
            printf("[%X]",rdata[i]);
          }
          printf("\r\n");
        }
        break;
      }
      case 'j':
      {
        //int i;
        unsigned char tdata[40],rdata[40],rlen;
        int node=0,len=0,index=0,subindex=0;
        
        printf("Write SDO:Press node number(1~127):");
        scanf("%d",&node);
        printf("Write SDO:Press index(hex):");
        scanf("%x",&index);
        printf("Write SDO:Press sub-index(hex):");
        scanf("%x",&subindex);
        printf("Write SDO:data length and data(hex)(len,d0,d1,d2,d3):");
        scanf("%d,%x,%x,%x,%x",&len,&tdata[0],&tdata[1],&tdata[2],&tdata[3]);
        getch();
        ret = I87123_WriteSDO((unsigned char)node,index,(unsigned char)subindex,(unsigned char)len,tdata,&rlen,rdata);
        if(ret){
          printf("WriteSDO Error Code = %d\r\n",ret);
          printf("Error data:[%X][%X][%X][%X][%X][%X][%X][%X]\r\n",
                 rdata[0],rdata[1],rdata[2],rdata[3],rdata[4],rdata[5],rdata[6],rdata[7]);
        }
        else{
          printf("Write SDO data OK.\r\n");
          //for(i=0;i<rlen;i++){
          //  printf("[%X]",rdata[i]);
          //}
          //printf("\r\n");
        }
      }
      break;  
      case 'k':
      {
        //int i;
        unsigned char tdata[40];
        int node=0,cobid=0,txrxtype=0,pdoch=0,ioch=0,io=0;
        
        printf("Install PDO:Press node number(1~127):");
        scanf("%d",&node);
        printf("Install PDO:Press COB ID(hex):");
        scanf("%x",&cobid);
        printf("Install PDO:Press tx/rx type(0:RxPDO, 1:TxPDO):");
        scanf("%x",&txrxtype);
        if(txrxtype){
          printf("Install PDO:Press AI/DI type(0:AI, 1:DI):");
          scanf("%x",&io);
          if(io){
            tdata[0] = 0x08;
            tdata[2] = 0x00;
            tdata[3] = 0x60;
          }
          else{
            tdata[0] = 0x10;
            tdata[2] = 0x01;
            tdata[3] = 0x64;
          }
        }
        else{
          printf("Install PDO:Press AO/DO type(0:AO, 1:DO):");
          scanf("%x",&io);
          if(io){
            tdata[0] = 0x08;
            tdata[2] = 0x00;
            tdata[3] = 0x62;
          }
          else{
            tdata[0] = 0x10;
            tdata[2] = 0x11;
            tdata[3] = 0x64;
          }
        }
        
        printf("Install PDO:Press the nth input or output channel want to mape:");
        scanf("%d",&ioch);
        tdata[1] = ioch;
        
        printf("Install PDO:Press the nth PDO application object to be mapped:");
        scanf("%d",&pdoch);
        getch();
        ret = I87123_InstallPDO((unsigned char)node,cobid,(unsigned char)txrxtype,(unsigned char)pdoch,tdata);
        if(ret){
          printf("Install PDO Error Code = %d\r\n",ret);
        }
        else{
          printf("Install PDO OK\r\n");
          //for(i=0;i<len;i++){
          //  printf("[%X]",rdata[i]);
          //}
          //printf("\r\n");
        }
      }
      break;        
      case 'l':
      {
        int node=0,cobid=0,txrxtype=0,pdoch=0;
        
        printf("Remove PDO:Press node number(1~127):");
        scanf("%d",&node);
        printf("Remove PDO:Press COB ID(hex):");
        scanf("%x",&cobid);
        printf("Remove PDO:Press tx/rx type(0:RxPDO, 1:TxPDO):");
        scanf("%x",&txrxtype);
        printf("Remove PDO:Nth PDO application object(0:Remove this COB-ID):");
        scanf("%d",&pdoch);
        getch();
        ret = I87123_RemovePDO((unsigned char)node,cobid,(unsigned char)txrxtype,(unsigned char)pdoch);
        if(ret){
          printf("RemovePDO Error Code = %d\r\n",ret);
        }
        else{
          printf("Remove OK\r\n");
        }
      }
      break;
      case 'm':
      {
        int node=0,cobid=0,type=0;
        
        printf("Response type:Press node number(1~127):");
        scanf("%d",&node);
        printf("Response type:Press COB ID(hex):");
        scanf("%x",&cobid);
        printf("Response type:0:remote only, 1:receive all PDO response:");
        scanf("%x",&type);
        getch();
        ret = I87123_SetPDOResponse((unsigned char)node,cobid,(unsigned char)type);
        if(ret){
          printf("PDOResponse Error Code = %d\r\n",ret);
        }
        else{
          printf("PDOResponse OK\r\n");
        }
        break;
      }
      case 'n':
      {
        int i;
        unsigned char rdata[40];
        unsigned char len=0;
        int cobid=0;
        
        printf("Remote PDO:Press COB ID(hex):");
        scanf("%x",&cobid);
        getch();
        ret = I87123_RemotePDO(cobid,&len,rdata);
        if(ret){
          printf("RemotePDO Error Code = %d\r\n",ret);
        }
        else{
          printf("Remote data: ");
          for(i=0;i<len;i++){
            printf("[%X]",rdata[i]);
          }
          printf("\r\n");
          printf("RemotePDO OK\r\n");
        }
        break;
      }
      case 'o':
      {
        unsigned char rdata[40];
        int cobid=0,len=0,offset=0;
        
        printf("Write PDO:Press COB ID(hex):");
        scanf("%x",&cobid);
        printf("Write PDO:Press data offset:");
        scanf("%d",&offset);
        printf("Write PDO:Press data length:");
        scanf("%d",&len);
        printf("Write PDO:Press data (d0,d1,d2,d3,d4,d5,d6,d7):");
        scanf("%x,%x,%x,%x,%x,%x,%x,%x",&rdata[0],&rdata[1],&rdata[2],&rdata[3],&rdata[4],&rdata[5],&rdata[6],&rdata[7]);
        getch();
        ret = I87123_WritePDO(cobid,(unsigned char)offset,(unsigned char)len,rdata);
        if(ret){
          printf("WritePDO Error Code = %d\r\n",ret);
        }
        else{
          printf("WritePDO OK\r\n");
        }
        break;
      }
      case 'p':
      {
        int node=0,cobid=0,txtype=0;
        
        printf("PDO Tx type:Press node number(1~127):");
        scanf("%d",&node);
        printf("PDO Tx type:Press COB ID(hex):");
        scanf("%x",&cobid);
        printf("PDO Tx type:Press tx type(0~255 ,reserved: 241~251):");
        scanf("%d",&txtype);
        getch();
        ret = I87123_PDOTxType((unsigned char)node, cobid, (unsigned char)txtype);
        if(ret){
          printf("PDOTxType Error Code = %d\r\n",ret);
        }
        else{
          printf("PDOTxType OK\r\n");
        }
        break;
      }
      case 'q':
      {
        ret = I87123_ShutdownMaster();
        if(ret){
          printf("ShutdownMaster Error Code = %d\r\n",ret);
        }
        else{
          printf("ShutdownMaster OK\r\n");
          printf("Press any key to exit program...");
          getch();
          return 0;
        }
      }
      break;
      default:
        printf("Unknow command = %X\r\n",command);
      break;
      }
    }
  }
}
