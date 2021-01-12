/*
 * ctc.c
 *
 * Created: 2020-06-01 오후 4:12:17
 * Author: ehkim
 */

#include <mega128.h>
#include <delay.h>

#define FND_DIG PORTE
#define FND_DATA    PORTB

unsigned int count;
unsigned char sec,min;

unsigned char FND_char[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90, 0x88, 0x83,
                            0xc4, 0xa1, 0x84, 0x8e};
unsigned int FND_dig[] = {0x1f, 0x2f, 0x4f, 0x8f, 0x0f};


void Init_Port(void)
{
 DDRE = 0xf0;          //fnd열
 DDRB = 0xff;          //fnd 데이터

 

}

void Init_Timer2()
{
 TCCR2 |= (1<<WGM21) | (1<<CS22) | (1<<CS20);          //ctc모드
 TIMSK = (1<<OCIE2);              //오버플로우 인터럽트
 TIFR = 0xff;                     //타이머 인터럽트 플래그 허가            
 OCR2 = 99;                       //100번마다 비교 인터럽트 발생 
 SREG |= 0x80;                    //인터럽트 허가  

}



interrupt [TIM2_COMP] void timer2_outcomp (void)
{
 count++;               //타이머 인터럽트 발생마다 카운트 발생, 시간 증가 
 if(count == 10)
 {
  sec++;
  count = 0;
 
 }          
 
 if(sec == 100)          //101에 0이 다시 되게해서 100에서 0으로 다시 시작 

 {
  sec = 0;

 }      
 if((min == 1) && (sec ==1) ) min = 0;   
 }    

void FND_Display(unsigned char Min, unsigned char Sec)
{
 FND_DIG = FND_dig[0];
 FND_DATA = FND_char[Sec%10];
 delay_ms(1);
 
 FND_DIG = FND_dig[1];
 FND_DATA = FND_char[Sec/10];
 delay_ms(1);
 
 if(Sec % 2)
 {
  FND_DIG = FND_dig[2];
  FND_DATA = FND_char[Min%10] & 0x7F;
  delay_ms(1);
 }
 else {
  FND_DIG = FND_dig[2];
  FND_DATA = FND_char[Min%10];
  delay_ms(1);
  }
 
 FND_DIG = FND_dig[3];
 FND_DATA = FND_char[Min/10];
 delay_ms(1);
 
}

void main(void)
{

count = 0;
 sec = 0;
 min = 0;
 Init_Port();
 Init_Timer2();           //타이머, 변수 초기화 
 

while (1)
    {
    // Please write your application code here
      FND_Display(min, sec);
    }
}
