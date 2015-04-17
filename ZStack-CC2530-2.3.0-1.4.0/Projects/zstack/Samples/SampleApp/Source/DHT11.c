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

void DHT11(uchar sd[3], uchar wd[3])   //��ʪ��������
{
  DATA_PIN=0;    //����͵�ƽ
  Delay_ms(19);  //�����18ms
  DATA_PIN=1;    //����ߵ�ƽ
  P0DIR &= ~0x80;                 //����P0.7�ڷ���Ϊ����
  
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
    
    wd[0] = ucharT_data_H/10 + 0x30;    //���У�����ȷ���õ���ȷֵ������ֵȫΪ0
    wd[1] = ucharT_data_H%10 + 0x30;
    wd[2] = '\0';
    sd[0] = ucharRH_data_H/10 + 0x30; 
    sd[1] = ucharRH_data_H%10 + 0x30; 
    sd[2] = '\0';
  } 
  else //û�м�⵽Ӧ���źţ�����0
  {
      osal_memcpy(wd, "fff", 3);
      osal_memcpy(sd, "fff", 3);
  } 
  
  P0DIR |= 0x80; //IO����Ҫ�������� 
}
