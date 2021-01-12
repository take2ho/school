/*
 * pwmsw.c
 *
 * Created: 2020-06-03 ���� 2:07:51
 * Author: ehkim
 */
#include <mega128.h>


void Interrupt_init()
{
 EIMSK = 0x03; //sw0,1 ����
 EICRA = 0x10; //0,1 ���� ����
 DDRB = 0xff; //��Ʈb ��� 
 DDRD = 0x00; //��Ʈd �Է� 
 SREG |= 0x80; //��ü ���ͷ�Ʈ ��

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
 #asm("nop");
  
}



void main(void)
{
Interrupt_init();
Init_Timer1();
OCR1B = (int)(3687*0.1); //�ʱⰪ 10���� �޽� 

while (1)
    {
    // Please write your application code here

    }
}

