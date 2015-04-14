#include "MQ.h"

void Delay_MS(uint msec)
{ 
    uint i,j;
    
    for (i=0; i<msec; i++)
        for (j=0; j<535; j++);
}

void InitGpio(void)
{
  P1SEL &= ~0x01;              //P1.0为普通IO口
  P1DIR |= 0x01;               //P1.0定义为输出口
  P0SEL &= ~0x20;              //P0.5为普通IO口
  P0DIR &= ~0x20;              //P0.5定义为输入口
}

uchar readMQ(void)
{
  uint i=0;
    
  InitGpio();                   //设置LED灯和MQ2相应的IO口

  LED1 = 1;                    //熄灭P1.0口灯 
  Delay_MS(100);                //延时一段时间，让传感器多一些时间去感应
  if(DATA_PIN_YW == 0)            //当浓度高于设定值时 ，执行条件函数        
  {
    Delay_MS(10);               //延时抗干扰
    if(DATA_PIN_YW == 0)         //确定 浓度高于设定值时 ，执行条件函数
    {
      for (i=0; i<10; i++)
      {
          LED1 = ~LED1;       //闪烁LED1，提示用户
      }
      return 1;               //返回1表示TURE有警告情况
    }
  }
  return 0;              //当浓度正常时返回0表示无警告情况
}