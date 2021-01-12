#include "stm32f4xx.h"

void _GPIO_Init(void);

void BEEP(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

int floor;  // ���� ������ ���� ���� ����
int select; // ���� ����� �Է� ���� ����

int main(void)
{  
    floor = 0;  // floor ���� 0������ ����
    DelayMS(100);
    _GPIO_Init();
   
    GPIOG->ODR = 0xFFFFFF00;	
    GPIOG->ODR |= 0x0001; 	
    
     while(1)
    {
	
 	if((GPIOH->IDR &  0x0100 || 0x0200 || 0x0400 || 0x0800 || 0x1000 || 0x2000 || 0x4000 || 0x8000) == 0) // sw0~7�߿� �ϳ� ������(0�� ����) if	
	{	BEEP(); //����ġ�� ������ ������ beep �ѹ�
                selectswitch(); // ���� ����ġ�� ���� select�� ����
                if (floor == select) // ���� ������ ���� ����ġ���� �Ȱ����� �ƹ��͵� ���� 
                {
                }
                else if (floor < select) //���� �������� ���� ����ġ���� �� ������
                {
                  for (i=floor+1; i<=select; i++) // ���� ������ ����ġ ������ �������� ����, ������ �ݺ��ϸ鼭 �ö�
                  {
                    floorled()
                  }
                  DelayMS(500);
                  BEEP();        // ��ȯ�� �ٳ����� �����ϸ� beep ���� �︲
                  BEEP();
                  BEEP();
                }
                else if ( floor > select) // ���� �������� ���� ����ġ���� �� ������
                   {
                  for (i=floor-1; i<=select; i--) // ���� ������ ����ġ ������ �������� ����, ������ �ݺ��ϸ鼭 ������
                  {
                    floorled()
                  }
                  DelayMS(500);
                  BEEP();        // ��ȯ�� �ٳ����� �����ϸ� beep ���� �︲
                  BEEP();
                  BEEP();
                }
        }



void _GPIO_Init(void)
{
	
    	RCC->AHB1ENR	|=  0x00000040;								
	GPIOG->MODER 	|=  0x00005555;							
	GPIOG->OTYPER	&= ~0x000000FF;	
 	GPIOG->OSPEEDR 	|=  0x00005555;	
    
	
	RCC->AHB1ENR    |=  0x00000080;							
	GPIOH->MODER 	&= ~0xFFFF0000;					
	GPIOH->PUPDR 	&= ~0xFFFF0000;	

	 
    	RCC->AHB1ENR	|=  0x00000020; 							
	GPIOF->MODER 	|=  0x00040000;						
	GPIOF->OTYPER 	&= ~0x00000200;		
 	GPIOF->OSPEEDR 	|=  0x00040000;	
}	

void BEEP(void)			
{ 	
	GPIOF->ODR |=  0x0200;	
	DelayMS(30);		
	GPIOF->ODR &= ~0x0200;	
}

void DelayMS(unsigned short wMS)
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000); 	
}

void DelayUS(unsigned short wUS)
{
	volatile int Dly = (int)wUS*17;
	for(; Dly; Dly--);
}

void floorled() // floor�� x���ϋ� x���� led�� �����ϴ� �Լ�
{
  if (floor =0) // floor�� 0�϶� 0.5�ʵ�����, 0�� led ����, 0.5�� ������, led ���� 
  {
    DelayMS(500);
    GPIOG->ODR |= 0x0001;
    DelayMS(500);
    GPIOG->ODR &= ~0x0001;
  }
  else if (floor = 1) // floor�� 1�϶� 0.5�ʵ�����, 1�� led ����, 0.5�� ������, led ���� 
  {
    DelayMS(500);
    GPIOG->ODR |= 0x0002;
    DelayMS(500);
    GPIOG->ODR &= ~0x0002;
  }
  else if (floor = 2)  // floor�� 2�϶� 0.5�ʵ�����, 2�� led ����, 0.5�� ������, led ���� 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0004;
    DelayMS(500);
    GPIOG->ODR &= ~0x0004;
  }
  else if (floor = 3)  // floor�� 3�϶� 0.5�ʵ�����, 3�� led ����, 0.5�� ������, led ���� 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0008;
    DelayMS(500)
    GPIOG->ODR &= ~0x0008;
  }
  else if (floor = 4)  // floor�� 4�϶� 0.5�ʵ�����, 4�� led ����, 0.5�� ������, led ���� 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0010;
    DelayMS(500);
    GPIOG->ODR &= ~0x0010;
  
  else if (floor = 5)   // floor�� 5�϶� 0.5�ʵ�����, 5�� led ����, 0.5�� ������, led ���� 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0020;
    DelayMS(500);
    GPIOG->ODR &= ~0x0020;
  }
  else if (floor = 6) // floor�� 6�϶� 0.5�ʵ�����, 6�� led ����, 0.5�� ������, led ���� 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0040;
    DelayMS(500);
    GPIOG->ODR &= ~0x0040;
  }
  else if (floor = 7)  // floor�� 7�϶� 0.5�ʵ�����, 7�� led ����, 0.5�� ������, led ���� 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0080;
    DelayMS(500);
    GPIOG->ODR &= ~0x0080;
  }
  else // 0���� 7���� �ƴϸ� �ƹ��͵� ����
  {
  }
}

void selectswitch() //����ġ�� ������ select�� ���� �׿� �°� �ٲٴ� �Լ�
{
  if ((GPIOH->IDR & 0x0100) == 0) //sw0 ����ġ�� �������� select�� 0 �Է�
  {
    select = 0;
  }
  else if ((GPIOH->IDR & 0x0200) == 0) //sw1 ����ġ�� �������� select�� 1 �Է�
  {
    select = 1;
  }
   else if ((GPIOH->IDR & 0x0400) == 0) //sw2 ����ġ�� �������� select�� 2 �Է�
  {
    select = 2;
  }
   else if ((GPIOH->IDR & 0x0800) == 0) //sw3 ����ġ�� �������� select�� 3 �Է�
  {
    select = 3;
  }
   else if ((GPIOH->IDR & 0x1000) == 0) //sw4 ����ġ�� �������� select�� 4 �Է�
  {
    select = 4;
  }
  else if ((GPIOH->IDR & 0x2000) == 0) //sw5 ����ġ�� �������� select�� 5 �Է�
  {
    select = 5;
  }
  else if ((GPIOH->IDR & 0x4000) == 0) //sw6 ����ġ�� �������� select�� 6 �Է�
  {
    select = 6;
  }
  else if ((GPIOH->IDR & 0x8000) == 0) //sw7 ����ġ�� �������� select�� 7 �Է�
  {
    select = 7;
  }
  else ()        //0~7 ����ġ�� �ƴѰ�� �ƹ��͵� ����
   {
    }
}