#include "DHT11.h"

//数据定义
uchar ucharFLAG, uchartemp;   //uchartemp是每次读取的位值
uchar ucharT_data_H = 0, ucharT_data_L = 0, ucharRH_data_H = 0, ucharRH_data_L = 0, ucharcheckdata;
uchar ucharT_data_H_temp, ucharT_data_L_temp, ucharRH_data_H_temp, ucharRH_data_L_temp, ucharcheckdata_temp;
uchar ucharcomdata;

void Delay_us(void) //1 us延时
{
    MicroWait(1);   
}

void Delay_10us(void) //10 us延时
{
   MicroWait(10);
}

void Delay_ms(uint Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us();
  }
}

void Read(void)    // 温湿写入
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG = 2; 
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us();  //延时30us后如果还是高电平，则此位为1，否则为0
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;   //如果还是高电平，则uchartemp = 1，否则值不变为0
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);    //等待剩余高电平结束
        if(ucharFLAG==1)break;       
        ucharcomdata<<=1;         
        ucharcomdata|=uchartemp;   //数据最新位赋值
    }    
}

void DHT11(uchar sd[3], uchar wd[3])   //温湿传感启动
{
  DATA_PIN=0;    //输出低电平
  Delay_ms(19);  //需大于18ms
  DATA_PIN=1;    //输出高电平
  P0DIR &= ~0x80;                 //配置P0.7口方向为输入
  
  Delay_10us();  //延时20-40us,等待一段时间后检测应答信号,应答信号是从机拉低数据线80us，检测应答信号，应答信号是低电平
  Delay_10us();                        
  Delay_10us();
  Delay_10us(); 
  
  if(!DATA_PIN)   //如果是应答信号
  {
    ucharFLAG = 2; 
    while((!DATA_PIN)&&ucharFLAG++);   //等待应答信号完毕
    ucharFLAG = 2;
    while((DATA_PIN)&&ucharFLAG++);    //应答信号完后会有80us的高电平，此处等待高电平结束
    
    Read();                             //读取湿度高8位
    ucharRH_data_H_temp = ucharcomdata;
    Read();                            //读取湿度低8位
    ucharRH_data_L_temp = ucharcomdata;
    Read();                            //读取温度高8位
    ucharT_data_H_temp = ucharcomdata;
    Read();                            //读取温度低8位
    ucharT_data_L_temp = ucharcomdata;
    Read();                            //校验
    ucharcheckdata_temp = ucharcomdata;
    DATA_PIN=1;                        //拉高引脚
    
    uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
    if(uchartemp==ucharcheckdata_temp)      //如果校验和正确
    {
      ucharRH_data_H = ucharRH_data_H_temp;
      ucharRH_data_L = ucharRH_data_L_temp;
      ucharT_data_H = ucharT_data_H_temp;
      ucharT_data_L = ucharT_data_L_temp;
      ucharcheckdata = ucharcheckdata_temp;
    }
    
    wd[0] = ucharT_data_H/10 + 0x30;    //如果校验和正确则会得到正确值，否则值全为0
    wd[1] = ucharT_data_H%10 + 0x30;
    wd[2] = '\0';
    sd[0] = ucharRH_data_H/10 + 0x30; 
    sd[1] = ucharRH_data_H%10 + 0x30; 
    sd[2] = '\0';
  } 
  else //没有检测到应答信号，返回0
  {
      osal_memcpy(wd, "fff", 3);
      osal_memcpy(sd, "fff", 3);
  } 
  
  P0DIR |= 0x80; //IO口需要重新配置 
}
