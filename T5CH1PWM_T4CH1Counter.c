//////////////////////////////////////////////////////////////////////
// PWM Mode & Original Counting(Counter) Mode
// PWM(Pulse & Dir): Timer5 CH1
// COUNTING(Encoder): Timer4 CH1 
// OC1을 통한 PWM Pulse(DC motor 구동) 출력과 Encoder에 의한 모터회전수 측정 
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

void _GPIO_Init(void);
uint16_t KEY_Scan(void);
void DisplayInitScreen(void);

void TIMER4_PWM_Init(void);
void TIMER14_PWM_Init(void);
void TIMER4_COUNTER_Init(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);

uint16_t ENCODER;
uint8_t ENC[5];

int main(void)
{
	_GPIO_Init();
	LCD_Init();		// LCD 구동 함수
	DelayMS(10);		// LCD구동 딜레이
	DisplayInitScreen();	// LCD 초기화면구동 함수
	BEEP();

	TIMER14_PWM_Init();	// TIM5 Init (PWM mode)
        TIMER4_COUNTER_Init(); // TIM4 Init (Count mode)

	GPIOG->ODR &= ~0x00FF;	// 초기값: LED0~7 Off
	GPIOA->ODR |= (1<<3);	// 초기방향: CCW
	GPIOG->ODR |= 0x01; 	// LED0 ON
    
	while(1)
	{
		/* SW0~SW3 Duty 변경*/
		switch(KEY_Scan())
		{
			case SW0_PUSH : 	//SW0
				TIM5->CCR1 = 0;		// DR: 10%	  
				LCD_DisplayText(1,6,"0");
 				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<0);		// LED0 ON 
 			break;

			case SW1_PUSH : 	//SW1
				TIM5->CCR1 = 10;		// DR: 10%	  
				LCD_DisplayText(1,6,"10");
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<1);		// LED1 ON 
 			break;

                        case SW2_PUSH  : 	//SW2
				TIM5->CCR1 = 20;		// DR: 20%	  
				LCD_DisplayText(1,6,"20");
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<2);		// LED2 ON 
 			break;

                        case SW3_PUSH  : 	//SW3
				TIM5->CCR1 = 30;		// DR: 30%	  
				LCD_DisplayText(1,6,"30");
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<3);		// LED3 ON 
 			break;
                        
                        case SW4_PUSH  : 	//SW4
				TIM5->CCR1 = 40;		// DR: 40%	  
				LCD_DisplayText(1,6,"40");
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<4);		// LED4ON 
 			break;
                        
                        case SW5_PUSH  : 	//SW5
				TIM5->CCR1 = 50;		// DR: 30%	  
				LCD_DisplayText(1,6,"50");
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<5);		// LED5 ON 
 			break;
                        case SW7_PUSH  : 	//SW7
				ENCODER = TIM4->CNT;            // TIM4 CNT 값(Encoder 펄스 수) 읽음
                                 ENC[4]= ENCODER/10000;            // 10000 자리
                                  ENC[3]= ENCODER%10000/1000;       // 1000 자리
                                  ENC[2]= ENCODER%1000/100;         // 100 자리
                                ENC[1]= ENCODER%100/10;   // 10 자리
                             ENC[0]= ENCODER%10;       // 1 자리
		 		LCD_DisplayChar(3,5,ENC[4] + 0x30);
				LCD_DisplayChar(3,6,ENC[3] + 0x30);
				LCD_DisplayChar(3,7,ENC[2] + 0x30);
				LCD_DisplayChar(3,8,ENC[1] + 0x30);
				LCD_DisplayChar(3,9,ENC[0] + 0x30);
				GPIOG->ODR |= (1<<7);		// LED7 ON 
 			break;
                 }      
      
		/*Navi.SW로  DIR 변경*/
		if((GPIOI->IDR & 0x0200) == 0)	//NAVI.SW_LEFT (PI_9)
		{
                        GPIOA->ODR |= (1<<3);   //DIR = CCW(HIGH) (PA3)
			LCD_DisplayText(2,6," CCW");	   
		}
		if((GPIOI->IDR & 0x0100) == 0)	//NAVI.SW_RIGHT (PI_8)	
		{
			GPIOA->ODR &= ~(1<<3); 	//DIR = CW(LOW) (PA3)
			LCD_DisplayText(2,6,"  CW");
		}

	}
}

void TIMER4_COUNTER_Init(void)
{  
// Encoder 입력(Counting) 핀: PD12 (TIM4 CH1)
// Clock Enable : GPIOD & TIMER4
	RCC->AHB1ENR	|= (1<<3);	// 0x08, GPIOD Enable
	RCC->APB1ENR 	|= (1<<2);	// 0x04, TIMER4 Enable 

// PD12: TIM4_CH1
// PD12을 출력설정하고 Alternate function(TIM4_CH1)으로 사용 선언
	GPIOD->MODER 	|= (2<<24);	// 0x02000000(MODER.(25,24)=0b10), GPIOD PIN12 Output Alternate function mode 					
	GPIOD->OSPEEDR 	|= (2<<24);	// 0x02000000(OSPEEDER.(25,24)=0b11), GPIOD PIN12 Output speed (50MHz High speed)
	GPIOD->PUPDR	&= ~(3<<24); 	// GPIOD PIN12 NO Pull-up
  					// PD12 ==> TIM4_CH1
	GPIOD->AFR[1]	|= (2<<16);	// 0x00020000(AFR[1].(19~16)=0b0010): Connect TIM4 pins(PD12) to AF2(TIM3..5)
  
// Timerbase Mode
	// Setting CR1 : 0x0000 
	TIM4->CR1 &= ~(1<<4);	// DIR=0(Up counter)(reset state)
	TIM4->CR1 &= ~(1<<1);	// UDIS=0(Update event Enabled): By one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM4->CR1 &= ~(1<<2);	// URS=0(Update event source Selection): one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM4->CR1 &= ~(1<<3);	// OPM=0(The counter is NOT stopped at update event) (reset state)
	TIM4->CR1 &= ~(1<<7);	// ARPE=0(ARR is NOT buffered) (reset state)
	TIM4->CR1 &= ~(3<<8); 	// CKD(Clock division)=00(reset state)
	TIM4->CR1 &= ~(3<<5); 	// CMS(Center-aligned mode Sel)=00 (Edge-aligned mode) (reset state)
				// Center-aligned mode: The counter counts Up and DOWN alternatively

	// PSC, ARR
	TIM4->PSC = 1;		// Prescaler=1
	TIM4->ARR = 10000;	// Auto reload  :  count값 범위: 0~10000
        
	// Update(Clear) the Counter
	TIM4->EGR |= (1<<0);    // UG: Update generation  , REG's Update (CNT clear) 

// Output Compare Mode
	// CCMR1(Capture/Compare Mode Register 1) : Setting the MODE of Ch1 or Ch2
	TIM4->CCMR1 |= (1<<0); 	// CC1S(CC1 channel) = '0b01' : Input 
	TIM4->CCMR1 &= ~(15<<4); // IC1F='0b0000: No Input Filter 
				
	// CCER(Capture/Compare Enable Register) : Enable "Channel 1" 
	TIM4->CCER &= ~(1<<0);	// CC1E=0: Capture Disable
	TIM4->CCER &= ~(1<<1);	// CC1P=0: TI1FP1 NonInverting / Rising Edge   
	TIM4->CCER &= ~(1<<3);	// CC1NP=0: TI2FP2   

	// SMCR(Slave Mode Control Reg.) : External Clock Enable
	TIM4->SMCR |= (7<<0);	// SMS(Slave Mode Selection)=0b111 : External Clock Mode 1
	TIM4->SMCR |= (4<<4);	// TS(Trigger Selection)=0b100 : TI1F_ED(TI1 Edge Detector 출력신호)

	TIM4->CR1 |= (1<<0);	// CEN: Enable the Tim4 Counter  	
}

void TIMER14_PWM_Init(void)
{  
// 모터펄스(PWM)핀:PF9(TIM14 CH1), 방향:PF27
// Clock Enable : GPIOF & TIMER14
	RCC->AHB1ENR	|= (1<<5);	// GPIOF Enable
	RCC->APB1ENR 	|= (1<<8);	// TIMER14 Enable 
    						
// PF9을 출력설정하고 Alternate function(TIM14_CH1)으로 사용 선언 : PWM 출력
	GPIOF->MODER 	|= (2<<18);	// PF9 Output Alternate function mode					
	GPIOF->OSPEEDR 	|= (3<<18);	// PF9 Output speed (100MHz High speed)
	GPIOF->OTYPER	&= ~(1<<9);	// PF9 Output type push-pull (reset state)
	GPIOF->AFR[1]	|= (9<<4); 	// 0x00000002	(AFR[0].(3~0)=0b0010): Connect TIM14 pins(PF9) to AF9(TIM12..14)
					// PF9 ==> TIM14_CH1
    
// PA3을 GPIO  출력설정 : Dir (모터방향)
	GPIOF->MODER 	|= (1<<6);	// PA3 Output  mode					
	GPIOF->OSPEEDR 	|= (1<<6);	// PA3 Output speed (25MHz High speed)
	GPIOF->OTYPER	&= ~(1<<3);	// PA3 Output type push-pull (reset state)
        
// TIM5 Channel 1 : PWM 1 mode
	// Assign 'PWM Pulse Period'
	TIM14->PSC	= 8400-1;	// Prescaler 84,000,000Hz/8400 = 10,000 Hz(0.1ms)  (1~65536)
	TIM14->ARR	= 100-1;	// Auto reload  (0.1ms * 100 = 10ms : PWM Period)
    
	// Define the corresponding pin by 'Output'  
	// CCER(Capture/Compare Enable Register) : Enable "Channel 1" 
	TIM14->CCER	|= (1<<0);	// CC1E=1: OC1(TIM5_CH1) Active(Capture/Compare 1 output enable)
					// 해당핀(40번)을 통해 신호출력
	TIM14->CCER	&= ~(1<<1);	// CC1P=0: CC1 output Polarity High (OC1으로 반전없이 출력)

	// Duty Ratio 
	TIM14->CCR1	= 10;		// CCR1 value

	// 'Mode' Selection : Output mode, PWM 1
	// CCMR1(Capture/Compare Mode Register 1) : Setting the MODE of Ch1 or Ch2
	TIM14->CCMR1 	&= ~(3<<0); 	// CC1S(CC1 channel)='0b00' : Output 
	TIM14->CCMR1 	|= (1<<3); 	// OC1PE=1: Output Compare 1 preload Enable

	TIM14->CCMR1	|= (6<<4);	// OC1M: Output compare 1 mode: PWM 1 mode
	TIM14->CCMR1	|= (1<<7);	// OC1CE: Output compare 1 Clear enable

	// CR1 : Up counting & Counter TIM5 enable
	TIM14->CR1 	&= ~(1<<4);	// DIR: Countermode = Upcounter (reset state)
	TIM14->CR1 	&= ~(3<<8);	// CKD: Clock division = 1 (reset state)
	TIM14->CR1 	&= ~(3<<5); 	// CMS(Center-aligned mode Sel): No(reset state)
	TIM14->CR1	|= (1<<7);	// ARPE: Auto-reload preload enable
	TIM14->CR1	|= (1<<0);	// CEN: Counter TIM5 enable
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
	
}	

void BEEP(void)			// Beep for 20 ms 
{ 	GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
	DelayMS(20);		// Delay 20 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
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

	LCD_DisplayText(0,0,"PWM  & Encoder" );  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색
	LCD_DisplayText(1,0,"DUTY: 10%");
	LCD_DisplayText(2,0,"DIR :  CCW");
	LCD_DisplayText(3,0,"ENC :00000 ");
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

/******************************************************************************/
/*     RCC Set up                                                             */
/******************************************************************************/
void _RCC_Init(void)
{
    // PLL (clocked by HSE) used as System clock source                    

    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    // Enable HSE : 외부 입력 클락주파수: 5MHz(회로도 참조)
    RCC->CR |= 0x00010000;	// Set bit#16 of CR
 
    // Wait till HSE is ready and if Time out is reached exit 
    do
    {
	HSEStatus = RCC->CR & 0x00020000;	// CHECK bit#17 of CR (HSE RDY?) 
	StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & 0x00020000) != RESET)	// CHECK bit#17 of CR (HSE RDY?) // RESET is 0
    {
	HSEStatus = 0x01;	// HSE is Ready!
    }
    else
    {
	HSEStatus = 0x00;	// HSE is NOT Ready!
    }

    if (HSEStatus == 0x01)	// HSE clock Enable
    {
	// HCLK = SYSCLK / 1 (HCLK = 168MHz)
	RCC->CFGR |= 0x00000000;
 
	// PCLK2 = HCLK / 2  (PCLK2 = 84MHz)
	RCC->CFGR |= 0x00008000;	// PPRE2: APB(Advanced Peripheral Bus)(APB2) High-speed prescaler
					// 100: AHB clock divided by 2 

	// PCLK1 = HCLK / 4  (PCLK1 = 42MHz)
	RCC->CFGR |= 0x00001400;	// PPRE1: APB(Advanced Peripheral Bus)(APB1) Low-speed prescaler
					// 101: AHB clock divided by 4 

    	// Configure the main PLL 
	// Reset vlaue: 0x2400 3010 (PPLQ:4, PLLSR:0, PLL_M:16, PLL_N:192, PLL_P: 2(00))
        RCC->PLLCFGR = 0;
	RCC->PLLCFGR |= 8;		// PLL_M(6bits): 8(0b001000): /8
	RCC->PLLCFGR |= (336 << 6);	// PLL_N(9bits): 336 : *336
	RCC->PLLCFGR |= (0<<16);	// PLL_P(2bits): (2 >> 1) -1=0b00 : 2 : /2 
	RCC->PLLCFGR |= 0x00400000; 	// PLL_SR(1bit): 1 : HSE oscillator clock selected as PLL and PLLI2S clock
//      RCC->PLLCFGR = 0x24405408;
	// SYSCLK 계산 (HSE 입력클럭주파수: 8MHz)
	// SYSCLK = 8M * 336(N) /8(M) /2(P) = 168MHz	
    
	// Enable the main PLL 
	RCC->CR |= 0x01000000;	// Set bit#24 of CR : PLL ON

	// Wait till the main PLL is ready 
	while((RCC->CR & 0x02000000) == 0)	// CHECK bit#25 : PLL RDY?  
	{}
   
	// Configure Flash prefetch, Instruction cache, Data cache and wait state 
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

	// Select the main PLL as system clock source 
	// Reset value of RCC->CFGR: 0x0000 0000
	RCC->CFGR &= ~0x00000003;	// clear, (Reset value: HSI) 
	RCC->CFGR |= 0x00000002;	// PLL

	// Wait till the main PLL is used as system clock source 
	while ((RCC->CFGR & 0x0000000C ) != 0x00000008);	// CHECK bit#2~3 : PLL as system clock is RDY?  
	{}
    }
    else
    { // If HSE fails to start-up, the application will have wrong clock
        // configuration. User can add here some code to deal with this error 
    }
}

