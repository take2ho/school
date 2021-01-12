/*
 * fastpwm.c
 *
 * Created: 2020-06-03 ���� 1:36:06
 * Author: ehkim
 */

#include <mega128.h>
unsigned int count;
unsigned char duty;

void Init_Timer1()
{
 TIMSK = (1<<OCIE1A)|(1<<OCIE1B); //Ÿ�̸� ī����1�� ��º� ��ġ ���ͷ�Ʈ �㰡
 TCCR1A |= (1<<COM1B1) | (1<<WGM10); //OC1B ����� ���
 TCCR1B |= (1<<WGM13) | (1<<CS11); // 8����
 OCR1A = (3687); //2ms���� ���ͷ�Ʈ  
 ICR1 = (3687/2);

 DDRB = 0xff; //��Ʈb ���
 SREG |= 0x80; //��ü ���ͷ�Ʈ �㰡

}

interrupt [TIM1_COMPB] void timer1_PCPWM_OC1B_out (void)
{
 #asm("nop");
}

interrupt [TIM1_COMPA] void timer1_COMPA (void)
{
 count++;
 if(count == 500) //1�ʸ��� �߻�
 {
  count = 0;
  duty++;
  switch(duty)
  {
   case 1:
    OCR1B = 0; //��Ƽ�� 0����

   break;    
   case 2:
    OCR1B = (int)(3687*0.2); //��Ƽ�� 20����

   break;
   case 3:
    OCR1B = (int)(3687*0.4); //��Ƽ�� 40����

   break;
   case 4:
    OCR1B = (int)(3687*0.6); //��Ƽ�� 60����

   break;
   case 5:
    OCR1B = (int)(3687*0.8); //��Ƽ�� 80����

   break;
   case 6:
    OCR1B = 3687; //��Ƽ�� 100����

   break;           
   default:
    duty = 0;
    break;
  
  }
  
 
 } 
}



void main(void)
{
duty = 0;
count = 0;
Init_Timer1();

while (1)
    {
    // Please write your application code here

    }
}
