#include "DHT11.h"

//���ݶ���
uchar ucharFLAG, uchartemp;   //uchartemp��ÿ�ζ�ȡ��λֵ
uchar ucharT_data_H = 0, ucharT_data_L = 0, ucharRH_data_H = 0, ucharRH_data_L = 0, ucharcheckdata;
uchar ucharT_data_H_temp, ucharT_data_L_temp, ucharRH_data_H_temp, ucharRH_data_L_temp, ucharcheckdata_temp;
uchar ucharcomdata;

void Delay_us(void) //1 us��ʱ
{
    MicroWait(1);   
}

void Delay_10us(void) //10 us��ʱ
{
   MicroWait(10);
}

void Delay_ms(uint Time)//n ms��ʱ
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us();
  }
}

void Read(void)    // ��ʪд��
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG = 2; 
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us();  //��ʱ30us��������Ǹߵ�ƽ�����λΪ1������Ϊ0
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;   //������Ǹߵ�ƽ����uchartemp = 1������ֵ����Ϊ0
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);    //�ȴ�ʣ��ߵ�ƽ����
        if(ucharFLAG==1)break;       
        ucharcomdata<<=1;         
        ucharcomdata|=uchartemp;   //��������λ��ֵ
    }    
}

void DHT11(uchar *SD_H, uchar *SD_L, uchar *WD_H, uchar *WD_L)   //��ʪ��������
{
  P0DIR &= ~0x80;                 //����P0.7�ڷ���Ϊ����
  
  DATA_PIN=0;    //����͵�ƽ
  Delay_ms(20);  //�����18ms
  DATA_PIN=1;    //����ߵ�ƽ
  
  Delay_10us();  //��ʱ20-40us,�ȴ�һ��ʱ�����Ӧ���ź�,Ӧ���ź��Ǵӻ�����������80us�����Ӧ���źţ�Ӧ���ź��ǵ͵�ƽ
  Delay_10us();                        
  Delay_10us();
  Delay_10us(); 
  
  if(!DATA_PIN)   //�����Ӧ���ź�
  {
    ucharFLAG = 2; 
    while((!DATA_PIN)&&ucharFLAG++);   //�ȴ�Ӧ���ź����
    ucharFLAG = 2;
    while((DATA_PIN)&&ucharFLAG++);    //Ӧ���ź�������80us�ĸߵ�ƽ���˴��ȴ��ߵ�ƽ����
    
    Read();                             //��ȡʪ�ȸ�8λ
    ucharRH_data_H_temp = ucharcomdata;
    Read();                            //��ȡʪ�ȵ�8λ
    ucharRH_data_L_temp = ucharcomdata;
    Read();                            //��ȡ�¶ȸ�8λ
    ucharT_data_H_temp = ucharcomdata;
    Read();                            //��ȡ�¶ȵ�8λ
    ucharT_data_L_temp = ucharcomdata;
    Read();                            //У��
    ucharcheckdata_temp = ucharcomdata;
    DATA_PIN=1;                        //��������
    
    uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
    if(uchartemp==ucharcheckdata_temp)      //���У�����ȷ
    {
      ucharRH_data_H = ucharRH_data_H_temp;
      ucharRH_data_L = ucharRH_data_L_temp;
      ucharT_data_H = ucharT_data_H_temp;
      ucharT_data_L = ucharT_data_L_temp;
      ucharcheckdata = ucharcheckdata_temp;
    }
    
    *WD_H = ucharT_data_H/10;    //���У�����ȷ���õ���ȷֵ������ֵȫΪ0
    *WD_L = ucharT_data_H%10;
    
    *SD_H = ucharRH_data_H/10; 
    *SD_L = ucharRH_data_H%10;        
  } 
  else //û�м�⵽Ӧ���źţ�����0
  {
      *WD_H = *WD_L = *SD_H = *SD_L = 0;   
  } 
  
  P0DIR |= 0x80; //IO����Ҫ�������� 
}