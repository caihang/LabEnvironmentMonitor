#ifndef __DHT11_H__
#define __DHT11_H__

#include <ioCC2530.h>
#include "OnBoard.h"

typedef unsigned char uchar;
typedef unsigned int  uint;

#define DATA_PIN P0_7            //定义P0.7口为数据位

extern void DHT11(uchar sd[3], uchar wd[3]);                //温湿传感启动

#endif