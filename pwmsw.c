/*
 * pwmsw.c
 *
 * Created: 2020-06-03 오전 2:07:51
 * Author: ehkim
 */
#include <mega128.h>


void Interrupt_init()
{
 EIMSK = 0x03; //sw0,1 선택
 EICRA = 0x10; //0,1 폴링 엣지
 DDRB = 0xff; //포트b 출력 
 DDRD = 0x00; //포트d 입력 
 SREG |= 0x80; //전체 인터럽트 허

}        

interrupt [EXT_INT0] void ext_int0_isr (void)
{
 OCR1B = OCR1B + (int)(3687*0.1);
}
interrupt [EXT_INT1] void ext_int1_isr (void)
{
 OCR1B = OCR1B - (int)(3687*0.1);
}

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
 #asm("nop");
  
}



void main(void)
{
Interrupt_init();
Init_Timer1();
OCR1B = (int)(3687*0.1); //초기값 10프로 펄스 

while (1)
    {
    // Please write your application code here

    }
}

