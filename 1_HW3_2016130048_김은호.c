#include "stm32f4xx.h"

void _GPIO_Init(void);

void BEEP(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

int floor;  // 층수 구현을 위해 변수 선언
int select; // 층수 사용자 입력 변수 선언

int main(void)
{  
    floor = 0;  // floor 변수 0층으로 시작
    DelayMS(100);
    _GPIO_Init();
   
    GPIOG->ODR = 0xFFFFFF00;	
    GPIOG->ODR |= 0x0001; 	
    
     while(1)
    {
	
 	if((GPIOH->IDR &  0x0100 || 0x0200 || 0x0400 || 0x0800 || 0x1000 || 0x2000 || 0x4000 || 0x8000) == 0) // sw0~7중에 하나 누를시(0값 들어옴) if	
	{	BEEP(); //스위치를 누르면 무조건 beep 한번
                selectswitch(); // 누른 스위치에 따라 select값 생성
                if (floor == select) // 현재 층수와 누른 스위치값이 똑같으면 아무것도 안함 
                {
                }
                else if (floor < select) //현재 층수보다 누른 스위치값이 더 높을때
                {
                  for (i=floor+1; i<=select; i++) // 현재 층수가 스위치 값까지 갈떄까지 점멸, 꺼짐을 반복하면서 올라감
                  {
                    floorled()
                  }
                  DelayMS(500);
                  BEEP();        // 순환이 다끝나고 도착하면 beep 세번 울림
                  BEEP();
                  BEEP();
                }
                else if ( floor > select) // 현재 층수보다 누른 스위치값이 더 낮을때
                   {
                  for (i=floor-1; i<=select; i--) // 현재 층수가 스위치 값까지 갈떄까지 점멸, 꺼짐을 반복하면서 내려감
                  {
                    floorled()
                  }
                  DelayMS(500);
                  BEEP();        // 순환이 다끝나고 도착하면 beep 세번 울림
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

void floorled() // floor가 x층일떄 x번의 led가 점멸하는 함수
{
  if (floor =0) // floor가 0일때 0.5초딜레이, 0층 led 점멸, 0.5초 딜레이, led 꺼짐 
  {
    DelayMS(500);
    GPIOG->ODR |= 0x0001;
    DelayMS(500);
    GPIOG->ODR &= ~0x0001;
  }
  else if (floor = 1) // floor가 1일때 0.5초딜레이, 1층 led 점멸, 0.5초 딜레이, led 꺼짐 
  {
    DelayMS(500);
    GPIOG->ODR |= 0x0002;
    DelayMS(500);
    GPIOG->ODR &= ~0x0002;
  }
  else if (floor = 2)  // floor가 2일때 0.5초딜레이, 2층 led 점멸, 0.5초 딜레이, led 꺼짐 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0004;
    DelayMS(500);
    GPIOG->ODR &= ~0x0004;
  }
  else if (floor = 3)  // floor가 3일때 0.5초딜레이, 3층 led 점멸, 0.5초 딜레이, led 꺼짐 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0008;
    DelayMS(500)
    GPIOG->ODR &= ~0x0008;
  }
  else if (floor = 4)  // floor가 4일때 0.5초딜레이, 4층 led 점멸, 0.5초 딜레이, led 꺼짐 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0010;
    DelayMS(500);
    GPIOG->ODR &= ~0x0010;
  
  else if (floor = 5)   // floor가 5일때 0.5초딜레이, 5층 led 점멸, 0.5초 딜레이, led 꺼짐 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0020;
    DelayMS(500);
    GPIOG->ODR &= ~0x0020;
  }
  else if (floor = 6) // floor가 6일때 0.5초딜레이, 6층 led 점멸, 0.5초 딜레이, led 꺼짐 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0040;
    DelayMS(500);
    GPIOG->ODR &= ~0x0040;
  }
  else if (floor = 7)  // floor가 7일때 0.5초딜레이, 7층 led 점멸, 0.5초 딜레이, led 꺼짐 
  { 
    DelayMS(500);
    GPIOG->ODR |= 0x0080;
    DelayMS(500);
    GPIOG->ODR &= ~0x0080;
  }
  else // 0에서 7층이 아니면 아무것도 안함
  {
  }
}

void selectswitch() //스위치를 누를때 select의 값을 그에 맞게 바꾸는 함수
{
  if ((GPIOH->IDR & 0x0100) == 0) //sw0 스위치를 눌렀을떄 select값 0 입력
  {
    select = 0;
  }
  else if ((GPIOH->IDR & 0x0200) == 0) //sw1 스위치를 눌렀을떄 select값 1 입력
  {
    select = 1;
  }
   else if ((GPIOH->IDR & 0x0400) == 0) //sw2 스위치를 눌렀을떄 select값 2 입력
  {
    select = 2;
  }
   else if ((GPIOH->IDR & 0x0800) == 0) //sw3 스위치를 눌렀을떄 select값 3 입력
  {
    select = 3;
  }
   else if ((GPIOH->IDR & 0x1000) == 0) //sw4 스위치를 눌렀을떄 select값 4 입력
  {
    select = 4;
  }
  else if ((GPIOH->IDR & 0x2000) == 0) //sw5 스위치를 눌렀을떄 select값 5 입력
  {
    select = 5;
  }
  else if ((GPIOH->IDR & 0x4000) == 0) //sw6 스위치를 눌렀을떄 select값 6 입력
  {
    select = 6;
  }
  else if ((GPIOH->IDR & 0x8000) == 0) //sw7 스위치를 눌렀을떄 select값 7 입력
  {
    select = 7;
  }
  else ()        //0~7 스위치가 아닌경우 아무것도 안함
   {
    }
}