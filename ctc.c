/*
 * ctc.c
 *
 * Created: 2020-06-01 ���� 4:12:17
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
 DDRE = 0xf0;          //fnd��
 DDRB = 0xff;          //fnd ������

 

}

void Init_Timer2()
{
 TCCR2 |= (1<<WGM21) | (1<<CS22) | (1<<CS20);          //ctc���
 TIMSK = (1<<OCIE2);              //�����÷ο� ���ͷ�Ʈ
 TIFR = 0xff;                     //Ÿ�̸� ���ͷ�Ʈ �÷��� �㰡            
 OCR2 = 99;                       //100������ �� ���ͷ�Ʈ �߻� 
 SREG |= 0x80;                    //���ͷ�Ʈ �㰡  

}



interrupt [TIM2_COMP] void timer2_outcomp (void)
{
 count++;               //Ÿ�̸� ���ͷ�Ʈ �߻����� ī��Ʈ �߻�, �ð� ���� 
 if(count == 10)
 {
  sec++;
  count = 0;
 
 }          
 
 if(sec == 100)          //101�� 0�� �ٽ� �ǰ��ؼ� 100���� 0���� �ٽ� ���� 

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
 Init_Timer2();           //Ÿ�̸�, ���� �ʱ�ȭ 
 

while (1)
    {
    // Please write your application code here
      FND_Display(min, sec);
    }
}
