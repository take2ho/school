//////////////////////////////////////////////////////////////////////
// USART RX Interrupt 
// USART1: TX pin: PA9, RX pin: PA10 
// TX: Polling 방식, RX: Interrupt 방식 
// 문자를 TX를 통해 PC(Hyper terminal)로 전송하고, 
// PC에서 보내온 문자를 받아 LCD에 표시
//////////////////////////////////////////////////////////////////////
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

void DispayTitle(void);
void _GPIO_Init(void);
void UART5_Init(void);
void USART_BRR_Configuration(uint32_t USART_BaudRate);

void SerialSendChar(uint8_t c);
void SerialSendString(char *s);

uint16_t KEY_Scan(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);


int count=0;

int main(void)
{
	LCD_Init();	// LCD 구동 함수
	DelayMS(100);	// LCD구동 딜레이
    
	_GPIO_Init();
	UART5_Init();
	GPIOG->ODR &= 0x00;	// LED0~7 Off 
          
	DispayTitle();	//LCD 초기화면구동 함수
	BEEP();
    
	while(1)
	{
          
		switch(KEY_Scan())
		{
			case SW0_PUSH : 		//SW0
				GPIOG->ODR |= 0x01;	// LED0 On
				SerialSendChar(count+0x30);
                                LCD_DisplayChar(1,0,count+0x30);
                                count+=1;
                                if(count>9)
                                {
                                  count=0;
                                }
                                  
 			break;
                }      
 	}
}

void UART5_IRQHandler(void)	
{       
	if ( (UART5->SR & USART_SR_RXNE) ) 	// USART_SR_RXNE=(1<<5) 
	{
		char ch;
		ch = (uint16_t)(UART5->DR & (uint16_t)0x01FF);	// 수신된 문자 저장                                                                                                                                                                                                                                                     
	//	SerialSendChar(ch); 		// Echo
                LCD_DisplayChar(2,0,ch); 	// LCD display
	} 
        // DR 을 읽으면 SR.RXNE bit(flag bit)는 clear 된다. 즉 clear 할 필요없음 
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
}

void UART5_Init(void)
{
	// UART5 : TX(Pc12)
	RCC->AHB1ENR	|= (1<<2);	// RCC_AHB1ENR GPIOC Enable
	GPIOC->MODER	|= (2<<2*12);	// GPIOC PIN12 Output Alternate function mode					
	GPIOC->OSPEEDR	|= (3<<2*12);	// GPIOC PIN12 Output speed (100MHz Very High speed)
	GPIOC->AFR[1]	|= (8<<16);	// Connect GPIOC pin12 to AF8(UART5)
    
	// UART5 : RX(PD2)
        RCC->AHB1ENR	|= (1<<3);	// RCC_AHB1ENR GPIOD Enable
	GPIOD->MODER 	|= (2<<2*2);	// GPIOD PIN2 Output Alternate function mode
	GPIOD->OSPEEDR	|= (3<<2*2);	// GPIOD PIN2 Output speed (100MHz Very High speed
	GPIOD->AFR[0]	|= (8<<8);	// Connect GPIOD pin2 to AF8(UART5)

	RCC->APB1ENR	|= (1<<20);	// RCC_APB1ENR UART5 Enable
    
	USART_BRR_Configuration(19200); // USART Baud rate Configuration
    
	UART5->CR1	&= ~(1<<12);	// USART_WordLength 8 Data bit
	UART5->CR1	&= ~(1<<10);	// NO USART_Parity
	UART5->CR1	|= (1<<2);	// 0x0004, USART_Mode_RX Enable
	UART5->CR1	|= (1<<3);	// 0x0008, USART_Mode_Tx Enable
	UART5->CR2	&= ~(3<<12);	// 0b00, USART_StopBits_1
	UART5->CR3	= 0x0000;	// No HardwareFlowControl, No DMA
    
	UART5->CR1 	|= (1<<5);	// 0x0020, RXNE interrupt Enable
	NVIC->ISER[1]	|= (1<<(53-32));// Enable Interrupt UART5 (NVIC 53번)
	UART5->CR1 	|= (1<<13);	//  0x2000, USART1 Enable
}


void SerialSendChar(uint8_t Ch) // 1문자 보내기 함수
{
        while((UART5->SR & USART_SR_TXE) == RESET); // USART_SR_TXE=(1<<7), 송신 가능한 상태까지 대기

	UART5->DR = (Ch & 0x01FF);	// 전송 (최대 9bit 이므로 0x01FF과 masking)
}


void SerialSendString(char *str) // 여러문자 보내기 함수
{
	while (*str != '\0') // 종결문자가 나오기 전까지 구동, 종결문자가 나온후에도 구동시 메모리 오류 발생가능성 있음.
	{
		SerialSendChar(*str);	// 포인터가 가르키는 곳의 데이터를 송신
		str++; 			// 포인터 수치 증가
	}
}

// Baud rate  
void USART_BRR_Configuration(uint32_t USART_BaudRate)
{ 
	uint32_t tmpreg = 0x00;
	uint32_t APB1clock = 42000000;	//PCLK2_Frequency
	uint32_t integerdivider = 0x00;
	uint32_t fractionaldivider = 0x00;

	// Determine the integer part 
	if ((UART5->CR1 & USART_CR1_OVER8) != 0) // USART_CR1_OVER8=(1<<15)
        {                                         // USART1->CR1.OVER8 = 1 (8 oversampling)
		// Computing 'Integer part' when the oversampling mode is 8 Samples 
		integerdivider = ((25 * APB1clock) / (2 * USART_BaudRate));    
	}
	else  // USART1->CR1.OVER8 = 0 (16 oversampling)
	{	// Computing 'Integer part' when the oversampling mode is 16 Samples 
		integerdivider = ((25 * APB1clock) / (4 * USART_BaudRate));    
	}
	tmpreg = (integerdivider / 100) << 4;
  
	// Determine the fractional part 
	fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

	// Implement the fractional part in the register 
	if ((UART5->CR1 & USART_CR1_OVER8) != 0)	// 8 oversampling
	{
		tmpreg |= (((fractionaldivider * 8) + 50) / 100) & (0x07);
	}
	else 						// 16 oversampling
	{
		tmpreg |= (((fractionaldivider * 16) + 50) / 100) & (0x0F);
	}

	// Write to USART BRR register
	UART5->BRR = (uint16_t)tmpreg;
}

void DispayTitle(void)
{	LCD_Clear(RGB_WHITE);
	LCD_SetFont(&Gulim8);
	LCD_SetBackColor(RGB_GREEN);	//배경색
	LCD_SetTextColor(RGB_BLACK);	//글자색
	LCD_DisplayText(0,0,"UART5");
        

	LCD_SetBackColor(RGB_WHITE);	//글자배경색
}
void DelayMS(unsigned short wMS)
{	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000);  // 1000us => 1ms
}
void DelayUS(unsigned short wUS)
{	volatile int Dly = (int)wUS*17;
	for(; Dly; Dly--);
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
void BEEP(void)			// Beep for 20 ms 
{ 	GPIOF->ODR |= (1<<9);	// PF9 'H' Buzzer on
	DelayMS(20);		// Delay 20 ms
	GPIOF->ODR &= ~(1<<9);	// PF9 'L' Buzzer off
}