#ifndef __DHT11_H__
#define __DHT11_H__

#include <ioCC2530.h>
#include "OnBoard.h"

typedef unsigned char uchar;
typedef unsigned int  uint;

#define DATA_PIN P0_7            //����P0.7��Ϊ����λ

extern void DHT11(uchar sd[3], uchar wd[3]);                //��ʪ��������

#endif