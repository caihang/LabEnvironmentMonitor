#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "DebugTrace.h"

#include "Coordinator.h"
#include "SampleAppHw.h"

#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#include "DHT11.h"
#include "MQ.h"

const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID   //1
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  0,                               //  uint8  AppNumInClusters;
  (cId_t *)NULL,                    //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList  //  uint8 *pAppInClusterList;
};

endPointDesc_t SampleApp_epDesc;
uint8 SampleApp_TaskID;
uint8 SampleApp_TransID;
devStates_t SampleApp_NwkState;
afAddrType_t SampleApp_P2P_DstAddr;

void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_Send_The_Message(void);
void SampleApp_Send_The_Error_Message(void);
void To_String(uint8 *dest, char *src, uint8 length);

void SampleApp_Init( uint8 task_id )
{ 
  P0SEL &= 0x7f;                  //P0_7配置成通用io
  
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;
  
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;
  
  afRegister( &SampleApp_epDesc );
}

uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;
          
        default:
        break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}

void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  RFDATA rfdata;
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_PERIODIC_CLUSTERID:
      osal_memcpy(rfdata.RFbuf, pkt->cmd.Data, 20);
      if(rfdata.rfbuf.Head == '&'&&rfdata.rfbuf.Nop1 == 'C'&&rfdata.rfbuf.Nop2 == 'J'&&rfdata.rfbuf.Nop3 == 'S'&&rfdata.rfbuf.Tail == 'J')
      {
        SampleApp_Send_The_Message();
      }
      else
      {
        SampleApp_Send_The_Error_Message();
      }
      break;
  }
}

void SampleApp_Send_The_Message(void)
{
  RFDATA rfdata;
  uint8 WD_H, WD_L, SD_H, SD_L;
  uint8 MQStatus;
  
  DHT11(&SD_H, &SD_L, &WD_H, &WD_L);
  MQStatus = readMQ();
  
  rfdata.rfbuf.Head = '#';
  To_String(rfdata.rfbuf.IEEEAddr, NLME_GetExtAddr(), 8);  //获取自己的IEEE节点信息
  rfdata.rfbuf.Nop1 = '#';
  rfdata.rfbuf.temperature[0] = '0';
  rfdata.rfbuf.temperature[1] = WD_H;
  rfdata.rfbuf.temperature[2] = WD_L;
  rfdata.rfbuf.Nop2 = '#';
  rfdata.rfbuf.hummidity[0] = '0';
  rfdata.rfbuf.hummidity[1] = SD_H;
  rfdata.rfbuf.hummidity[2] = SD_L;
  rfdata.rfbuf.Nop3 = '#';
  rfdata.rfbuf.MQstatus = MQStatus;
  rfdata.rfbuf.Tail = '*';
  AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc, SAMPLEAPP_PERIODIC_CLUSTERID, 20, (uint8 *)&rfdata, &SampleApp_TransID,  
                 AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
}
  
void SampleApp_Send_The_Error_Message(void)
{
  RFDATA rfdata;
  osal_memcpy(rfdata.RFbuf, "can't read the commd", 20);
  AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc, SAMPLEAPP_PERIODIC_CLUSTERID, 20, (uint8 *)&rfdata, &SampleApp_TransID,  
                 AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
}
  
void To_String(uint8 *dest, char *src, uint8 length)
{
  uint8 *xad;
  uint8 i = 0; 
  uint8 ch;
  xad = src + length - 1;
  for(i = 0; i < length; i++, xad--)
  {
    ch = (*xad >> 4)&0x0F;
    dest[i<<1] = ch + ((ch < 10) ? '0' : '7');
    ch = *xad & 0x0F;
    dest[(i<<1) + 1] = ch + ((ch < 10) ? '0' : '7');
  }
}


  

  