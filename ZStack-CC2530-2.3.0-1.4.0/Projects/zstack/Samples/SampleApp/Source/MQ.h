#ifndef _MQ_H
#define _MQ_H

#include <ioCC2530.h>
#define LED1     P1_0            //定义P1.0口为LED1控制端
#define DATA_PIN_YW P0_5            //定义P0.5口为传感器的输入端

typedef unsigned char uchar;
typedef unsigned int  uint;

extern uchar readMQ(void);
#endif