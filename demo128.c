/*
 * demo128.c
 *
 * Created: 2020-04-14 ���� 2:05:32
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
 DDRE = 0xf0;             //fnd ������
 DDRB = 0xff;             //fnd ������
 DDRD = 0x00;             // ��� d��Ʈ
 

}


void Init_Timer0(void)
{
 TIMSK = (1<<TOIE0);        //�����÷ο� ���ͷ�Ʈ
 TCCR0 = (1<<CS02)|(1<<CS01) | (1<<CS00);        //�Ϲݸ��, 1024���ֺ�
 TCNT0 = -5;                                     //ī���Ͱ� ����
 SREG |= 0x80;                                   //���ͷ�Ʈ ��ü �㰡

}


void Interrupt_init()
{
 EIMSK = 0x01;        //���ͷ�Ʈ ����
 EICRA = 0x02;        // ���ͷ�Ʈ ������(�ϰ�����)
 DDRB = 0xff;         //��Ʈb ���
 DDRD = 0x00;         //��Ʈd �Է� 
 SREG |= 0x80;        //��ü ���ͷ�Ʈ �㰡 
}

interrupt [EXT_INT0] void ext_int0_isr (void)
{
 count = 0;
 min = 0;
 sec = 0;                   //0��ư ������ �ð� ���� 
}

interrupt [TIM0_OVF] void timer0_overflow (void)
{

 TCNT0 = -5;
 count++;            //Ÿ�̸� �߻��ø��� ī��Ʈ�� �ö󰡰� �׿� ���� �ð� ���
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
 FND_DIG = FND_dig[0];               //����1�ڸ� 
 FND_DATA = FND_char[Sec%10];
 delay_ms(1);
 
 FND_DIG = FND_dig[1];               //����10�ڸ� 
 FND_DATA = FND_char[Sec/10];
 delay_ms(1);
 
 if(Sec % 2)
 {
  FND_DIG = FND_dig[2];              //����1�ڸ� 
  FND_DATA = FND_char[Min%10] & 0x7F;
  delay_ms(1);
 }
                                     //����10�ڸ� 
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
 Interrupt_init();             //���ͷ�Ʈ, ���� �ʱ�
 

 
while (1)
    {
    // Please write your application code here    
    
    
    
    
     FND_Display(min, sec); 
  
    }
}
