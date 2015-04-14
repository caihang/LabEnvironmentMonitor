#ifndef COORDINATOR_H
#define COORDINATOR_H

#ifdef __cplusplus
extern "C"
{
#endif
  
#include "ZComDef.h"
  
#define SAMPLEAPP_ENDPOINT           20

#define SAMPLEAPP_PROFID             0x0F08
#define SAMPLEAPP_DEVICEID           0x0001
#define SAMPLEAPP_DEVICE_VERSION     0
#define SAMPLEAPP_FLAGS              0

#define SAMPLEAPP_MAX_CLUSTERS       1
#define SAMPLEAPP_PERIODIC_CLUSTERID 1
#define SAMPLEAPP_FLASH_CLUSTERID    2
#define SAMPLEAPP_COM_CLUSTERID      3
#define SAMPLEAPP_P2P_CLUSTERID      4
  
#define SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT   3000     // Every 3 seconds
//#define SAMPLEAPP_SEND_PERIODIC_MSG_EVT       0x0001   
#define SAMPLEAPP_SEND_TO_ALL                 0x0001  //¹ã²¥ÊÂ¼þID
#define SAMPLEAPP_FLASH_GROUP                 0x0001
#define SAMPLEAPP_FLASH_DURATION              1000
  
typedef union h
{
  uint8 RFbuf[20];
  struct RFBUF
  {
    unsigned char Head;
    uint8 IEEEAddr[8];
    unsigned char Nop1;
    unsigned char temperature[3];
    unsigned char Nop2;
    unsigned char hummidity[3];
    unsigned char Nop3;
    unsigned char MQstatus;
    unsigned char Tail;
  }rfbuf;
}RFDATA;

extern void SampleApp_Init( uint8 task_id );
extern UINT16 SampleApp_ProcessEvent( uint8 task_id, uint16 events );

#ifdef __cplusplus
}
#endif

#endif /* COORDINATOR_H */