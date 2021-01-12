/*
 * demo128.c
 *
 * Created: 2020-04-14 오후 2:05:32
 * Author: ehkim
 */

#include <mega128.h>
#include <delay.h>

#define FND_DIG PORTE
#define FND_DATA    PORTB

unsigned int count;
unsigned char sec,min;
unsigned char key;

unsigned char FND_char[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90, 0x88, 0x83,
                            0xc4, 0xa1, 0x84, 0x8e};
unsigned int FND_dig[] = {0x1f, 0x2f, 0x4f, 0x8f, 0x0f};




void Init_Port(void)
{
 DDRE = 0xf0;             //fnd 열선택
 DDRB = 0xff;             //fnd 데이터
 DDRD = 0x00;             // 출력 d포트
 

}


void Init_Timer0(void)
{
 TIMSK = (1<<TOIE0);        //오퍼플로우 인터럽트
 TCCR0 = (1<<CS02)|(1<<CS01) | (1<<CS00);        //일반모드, 1024분주비
 TCNT0 = -5;                                     //카운터값 설정
 SREG |= 0x80;                                   //인터럽트 전체 허가

}


void Interrupt_init()
{
 EIMSK = 0x01;        //인터럽트 종류
 EICRA = 0x02;        // 인터럽트 제어방법(하강엣지)
 DDRB = 0xff;         //포트b 출력
 DDRD = 0x00;         //포트d 입력 
 SREG |= 0x80;        //전체 인터럽트 허가 
}

interrupt [EXT_INT0] void ext_int0_isr (void)
{
 count = 0;
 min = 0;
 sec = 0;                   //0버튼 누를시 시간 리셋 
}

interrupt [TIM0_OVF] void timer0_overflow (void)
{

 TCNT0 = -5;
 count++;            //타이머 발생시마다 카운트가 올라가고 그에 따른 시간 계수
 if(count == 2881)
 {
  sec++;
  count = 0;
 
 }          
 
 if(sec == 60)
 {
  sec = 0;
  min++;
 }      
 if(min == 60) min = 0;
}

void FND_Display(unsigned char Min, unsigned char Sec)
{
 FND_DIG = FND_dig[0];               //초의1자리 
 FND_DATA = FND_char[Sec%10];
 delay_ms(1);
 
 FND_DIG = FND_dig[1];               //초의10자리 
 FND_DATA = FND_char[Sec/10];
 delay_ms(1);
 
 if(Sec % 2)
 {
  FND_DIG = FND_dig[2];              //분의1자리 
  FND_DATA = FND_char[Min%10] & 0x7F;
  delay_ms(1);
 }
                                     //분의10자리 
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
 Init_Timer0(); 
 Interrupt_init();             //인터럽트, 변수 초기
 

 
while (1)
    {
    // Please write your application code here    
    
    
    
    
     FND_Display(min, sec); 
  
    }
}
