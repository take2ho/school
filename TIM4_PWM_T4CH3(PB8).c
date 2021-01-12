/////////////////////////////////////////////////////////////
// PWM Mode
// Timer4 CH3
// OC3을 통한 PWM Pulse(DC motor 구동) 출력
/////////////////////////////////////////////////////////////

#include "stm32f4xx.h"
#include "GLCD.h"

#define SW0_PUSH        0xFE00  //PH8
#define SW1_PUSH        0xFD00  //PH9
#define SW2_PUSH        0xFB00  //PH10
#define SW3_PUSH        0xF700  //PH11
#define SW4_PUSH        0xEF00  //PH12
#define SW5_PUSH        0xDF00  //PH13
#define SW6_PUSH        0xBF00  //PH14
#define SW7_PUSH        0x7F00  //PH15

void _GPIO_Init(void);
uint16_t KEY_Scan(void);
void DisplayInitScreen(void);

void TIMER4_PWM_Init(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);

int main(void)
{
	_GPIO_Init();
	LCD_Init();			// LCD 구동 함수
	DelayMS(10);			// LCD 구동 딜레이
	DisplayInitScreen();		// LCD 초기화면구동 함수
	BEEP();

	GPIOG->ODR &= ~0x00FF;		// 초기값: LED0~7 Off
	GPIOG->ODR |= (1<<0); 		// LED0 ON

	GPIOB->ODR |= (1<<9);		// 초기 모터 방향(PB9):CW 
	TIMER4_PWM_Init();		// TIM4 Init (PWM mode)
    
	while(1)
	{
		/* SW0~SW3 Duty 변경*/
		switch(KEY_Scan())
		{
			case SW0_PUSH : 	//SW0
				TIM5->CCR1 = 10;		// DR: 10%
				LCD_DisplayText(1,6,"10");
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<0);		// LED0 ON 
 			break;

			case SW1_PUSH : 	//SW1
				TIM5->CCR1 = 35;		// DR: 35%
				LCD_DisplayText(1,6,"35");
                             	GPIOG->ODR &= ~0x00FF;		// All LED Off
                            	GPIOG->ODR |= (1<<1);		// LED1 ON 
			break;

                        case SW2_PUSH  : 	//SW2
				TIM5->CCR1 = 70;		// DR: 70%
				LCD_DisplayText(1,6,"70");
                             	GPIOG->ODR &= ~0x00FF;		// All LED Off
                            	GPIOG->ODR |= (1<<2);		// LED2 ON 
			break;

                        case SW3_PUSH  : 	//SW3
				TIM5->CCR1 = 90;		// DR: 90%	  
				LCD_DisplayText(1,6,"90");
                             	GPIOG->ODR &= ~0x00FF;		// All LED Off
                            	GPIOG->ODR |= (1<<3);		// LED3 ON 
			break;

                }      
      
		/*Navi.SW로  DIR 변경*/
		if((GPIOI->IDR & 0x0200) == 0)	//NAVI.SW_LEFT (PI_9)
		{
			 GPIOA->ODR |= (1<<3); 	//DIR = CW(HIGH) (PA_3)
			LCD_DisplayText(2,6," CW");	 	   
		}
		if((GPIOI->IDR & 0x0100) == 0)	//NAVI.SW_RIGHT (PI_8)	
		{
			GPIOA->ODR &= ~(1<<3); 	//DIR = CCW(LOW)(PA_3
			LCD_DisplayText(2,6,"CCW");
		}
	}
}


void TIMER4_PWM_Init(void)
{   
// TIM CH3 : PB8 
// Clock Enable : GPIOB & TIMER4
	RCC->AHB1ENR	|= (1<<0);	// GPIOA CLOCK Enable
	RCC->APB1ENR 	|= (1<<3);	// TIMER5 CLOCK Enable 
    						
// PB8을 출력설정하고 Alternate function(TIM5_CH1)으로 사용 선언 : PWM 출력
	GPIOA->MODER 	|= (2<<0);	// 0x00020000 PA0 Output Alternate function mode					
	GPIOA->OSPEEDR 	|= (3<<0);	// 0x00030000 PA0 Output speed (100MHz High speed)
	GPIOA->OTYPER	&= ~(1<<0);	// PA0 Output type push-pull (reset state)
	GPIOA->PUPDR	|= (1<<0);	// 0x00010000 PA0 Pull-up
 	GPIOA->AFR[0]	|= (2<<0);	// 0x00000002 (AFR[1].(3~0)=0b0010): Connect TIM4 pins(PA0) to AF2(TIM3..5)
    
// TIM5 Channel 1 : PWM 1 mode
	// Assign 'PWM Pulse Period'
	TIM5->PSC	= 8400-1;	// Prescaler 84,000,000Hz/8400 = 10,000 Hz(0.1ms)  (1~65536)
	TIM5->ARR	= 100-1;	// Auto reload  (0.1ms * 100 = 10ms : PWM Period)

	// Setting CR1 : 0x0000 (Up counting)
	TIM5->CR1 &= ~(1<<4);	// DIR=0(Up counter)(reset state)
	TIM5->CR1 &= ~(1<<1);	// UDIS=0(Update event Enabled): By one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM5->CR1 &= ~(1<<2);	// URS=0(Update event source Selection): one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM5->CR1 &= ~(1<<3);	// OPM=0(The counter is NOT stopped at update event) (reset state)
	TIM5->CR1 &= ~(1<<7);	// ARPE=0(ARR is NOT buffered) (reset state)
	TIM5->CR1 &= ~(3<<8); 	// CKD(Clock division)=00(reset state)
	TIM5->CR1 &= ~(3<<5); 	// CMS(Center-aligned mode Sel)=00 (Edge-aligned mode) (reset state)
				// Center-aligned mode: The counter counts Up and DOWN alternatively
    	
	// Define the corresponding pin by 'Output'  
	// CCER(Capture/Compare Enable Register) : Enable "Channel 3" 
	TIM5->CCER	|= (3<<0);	// CC3E=1: OC3(TIM4_CH3) Active(Capture/Compare 3 output enable)
					// 해당핀(167번)을 통해 신호출력
	TIM5->CCER	&= ~(1<<1);	// CC3P=0: CC3 Output Polarity (OCPolarity_High : OC3으로 반전없이 출력)

	// Duty Ratio 
	TIM5->CCR1	= 10;		// CCR3 value

	// 'Mode' Selection : Output mode, PWM 1
	// CCMR2(Capture/Compare Mode Register 2) : Setting the MODE of Ch3 or Ch4
	TIM5->CCMR1 	&= ~(3<<0); 	// CC3S(CC3 channel)= '0b00' : Output 
//	TIM4->CCMR2 	&= ~(1<<3); 	// OC3PE=0: Output Compare 3 preload disable
	TIM5->CCMR1 	|= (1<<3); 	// OC3PE=1: Output Compare 3 preload Enable
	TIM5->CCMR1	|= (6<<4);	// OC3M=0b110: Output compare 3 mode: PWM 1 mode
	TIM5->CCMR1	|= (1<<7);	// OC3CE=1: Output compare 3 Clear enable
	
	//Counter TIM5 enable
	TIM5->CR1	|= (1<<7);	// ARPE: Auto-reload preload enable
	TIM5->CR1	|= (1<<0);	// CEN: Counter TIM4 enable
}

void _GPIO_Init(void)
{
	// LED (GPIO G) 설정
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
    
	// SW (GPIO H) 설정 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
    	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER 	&= ~0x0200;	// GPIOF 9 : Push-pull  	
 	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
	
	//NAVI.SW(PORT I) 설정
	RCC->AHB1ENR 	|= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER 	= 0x00000000;	// GPIOI PIN8~PIN15 Input mode (reset state)
	GPIOI->PUPDR    = 0x00000000;	// GPIOI PIN8~PIN15 Floating input (No Pull-up, pull-down) (reset state)
	
	// DC_MOTOR DIR (PB9)
	RCC->AHB1ENR	|= 0x00000002; 	// RCC_AHB1ENR GPIOB Enable							
	GPIOB->MODER 	= 0x00040000;	// GPIOB PIN9 Output mode						
	GPIOB->OSPEEDR 	= 0x00040000;	// GPIOB PIN9 Output speed (25MHZ Medium speed) 
}	

void BEEP(void)			// Beep for 20 ms 
{ 	GPIOF->ODR |= (1<<9);	// PF9 'H' Buzzer on
	DelayMS(20);		// Delay 20 ms
	GPIOF->ODR &= ~(1<<9);	// PF9 'L' Buzzer off
}

void DelayMS(unsigned short wMS)
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000);   // 1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
	volatile int Dly = (int)wUS*17;
		for(; Dly; Dly--);
}

void DisplayInitScreen(void)
{
	LCD_Clear(RGB_WHITE);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_GREEN);	// 글자배경색 : Green
	LCD_SetTextColor(RGB_BLACK);	// 글자색 : Black

	LCD_DisplayText(0,0,"TIM4(CH3) PWM MODE");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
	LCD_DisplayText(1,0,"DUTY: 10%");
	LCD_DisplayText(2,0,"DIR :  CW");
}

uint8_t key_flag = 0;
uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{ 
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ?
	if(key == 0xFF00)		// if no key, check key off
	{  	if(key_flag == 0)
        		return key;
      		else
		{	DelayMS(10);
        		key_flag = 0;
        		return key;
        	}
    	}
  	else				// if key input, check continuous key
	{	if(key_flag != 0)	// if continuous key, treat as no key input
        		return 0xFF00;
      		else			// if new key,delay for debounce
		{	key_flag = 1;
			DelayMS(10);
 			return key;
        	}
	}
}
