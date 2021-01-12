/*
 * fastpwm.c
 *
 * Created: 2020-06-03 오전 1:36:06
 * Author: ehkim
 */

#include <mega128.h>
unsigned int count;
unsigned char duty;

void Init_Timer1()
{
 TIMSK = (1<<OCIE1A)|(1<<OCIE1B); //타이머 카운터1의 출력비교 일치 인터럽트 허가
 TCCR1A |= (1<<COM1B1) | (1<<WGM10); //OC1B 비반전 출력
 TCCR1B |= (1<<WGM13) | (1<<CS11); // 8분주
 OCR1A = (3687); //2ms마다 인터럽트  
 ICR1 = (3687/2);

 DDRB = 0xff; //포트b 출력
 SREG |= 0x80; //전체 인터럽트 허가

}

interrupt [TIM1_COMPB] void timer1_PCPWM_OC1B_out (void)
{
 #asm("nop");
}

interrupt [TIM1_COMPA] void timer1_COMPA (void)
{
 count++;
 if(count == 500) //1초마다 발생
 {
  count = 0;
  duty++;
  switch(duty)
  {
   case 1:
    OCR1B = 0; //듀티비 0프로

   break;    
   case 2:
    OCR1B = (int)(3687*0.2); //듀티비 20프로

   break;
   case 3:
    OCR1B = (int)(3687*0.4); //듀티비 40프로

   break;
   case 4:
    OCR1B = (int)(3687*0.6); //듀티비 60프로

   break;
   case 5:
    OCR1B = (int)(3687*0.8); //듀티비 80프로

   break;
   case 6:
    OCR1B = 3687; //듀티비 100프로

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
