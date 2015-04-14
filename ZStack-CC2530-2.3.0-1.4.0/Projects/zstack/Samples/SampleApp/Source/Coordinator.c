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

#if !defined( WIN32 )
#include "OnBoard.h"
#endif

#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"
  
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
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  0,                               //  uint8  AppNumInClusters;
  (cId_t *)NULL                    //  uint8 *pAppInClusterList;
};

endPointDesc_t SampleApp_epDesc;
uint8 SampleApp_TaskID;
uint8 SampleApp_TransID;
devStates_t SampleApp_NwkState;
afAddrType_t SampleApp_Periodic_DstAddr; //广播地址结构

void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_Send_The_Message(void);  //广播发送数据

void SampleApp_Init( uint8 task_id )
{ 
  SampleApp_TaskID = task_id;
  SampleApp_TransID = 0;
  
  MT_UartInit();                  //串口初始化
  MT_UartRegisterTaskID(task_id); //注册串口任务
  //P0SEL &= 0x7f;                  //P0_7配置成通用io
  
  #if defined ( BUILD_ALL_DEVICES )
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
  #endif // BUILD_ALL_DEVICES
  
  #if defined ( HOLD_AUTO_START )
  ZDOInitDevice(0);
  #endif
  
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;
  
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;
  
  afRegister( &SampleApp_epDesc );
  
  #if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
  #endif
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
        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:           //如果建立网络，则设置事件
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( SampleApp_NwkState == DEV_ZB_COORD )
          {
            osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_TO_ALL, 60000 );        //每一分钟广播一次信息
          }
          break;
        default:
        break;
      }
      
      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }
    return (events ^ SYS_EVENT_MSG);
  }
  
  if( events & SAMPLEAPP_SEND_TO_ALL )
  {
    SampleApp_Send_The_Message();
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_TO_ALL, 60000 );
    return (events ^ SAMPLEAPP_SEND_TO_ALL);
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
    HalUARTWrite(0, rfdata.RFbuf, 20);
    break;
  }
}

void SampleApp_Send_The_Message(void)
{
  RFDATA rfdata;
  osal_memcpy(rfdata.RFbuf, "&00000000C000J000S0J", 20);
  AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc, SAMPLEAPP_PERIODIC_CLUSTERID, 20, (uint8 *)&rfdata, &SampleApp_TransID,  
                 AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
}




