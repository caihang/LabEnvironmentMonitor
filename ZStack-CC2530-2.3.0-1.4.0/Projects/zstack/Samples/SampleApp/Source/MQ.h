#ifndef _MQ_H
#define _MQ_H

#include <ioCC2530.h>
#define LED1     P1_0            //����P1.0��ΪLED1���ƶ�
#define DATA_PIN_YW P0_5            //����P0.5��Ϊ�������������

typedef unsigned char uchar;
typedef unsigned int  uint;

extern uchar readMQ(void);
#endif