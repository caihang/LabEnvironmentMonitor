#include "MQ.h"

void Delay_MS(uint msec)
{ 
    uint i,j;
    
    for (i=0; i<msec; i++)
        for (j=0; j<535; j++);
}

void InitGpio(void)
{
  P1SEL &= ~0x01;              //P1.0Ϊ��ͨIO��
  P1DIR |= 0x01;               //P1.0����Ϊ�����
  P0SEL &= ~0x20;              //P0.5Ϊ��ͨIO��
  P0DIR &= ~0x20;              //P0.5����Ϊ�����
}

uchar readMQ(void)
{
  uint i=0;
    
  InitGpio();                   //����LED�ƺ�MQ2��Ӧ��IO��

  LED1 = 1;                    //Ϩ��P1.0�ڵ� 
  Delay_MS(100);                //��ʱһ��ʱ�䣬�ô�������һЩʱ��ȥ��Ӧ
  if(DATA_PIN_YW == 0)            //��Ũ�ȸ����趨ֵʱ ��ִ����������        
  {
    Delay_MS(10);               //��ʱ������
    if(DATA_PIN_YW == 0)         //ȷ�� Ũ�ȸ����趨ֵʱ ��ִ����������
    {
      for (i=0; i<10; i++)
      {
          LED1 = ~LED1;       //��˸LED1����ʾ�û�
      }
      return 1;               //����1��ʾTURE�о������
    }
  }
  return 0;              //��Ũ������ʱ����0��ʾ�޾������
}