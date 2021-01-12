//************************************************

// 작성자: 2016130048 김은호

// 작성일: 2019.10.08
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


void _EXTI_Init(void);
void TIMER14_PWM_Init(void);
void TIMER1_COUNTER_Init(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);

uint16_t ENCODER;
uint8_t ENC[5];

int speed =0; //speed 변수 0으로 초기화
int count=0; //led 7번 횟수 카운트용 변수 0으로 초기화
int period; //period 측정용 변수 설정

int main(void)
{
	_GPIO_Init();
	LCD_Init();		// LCD 구동 함수
	DelayMS(10);		// LCD구동 딜레이
	DisplayInitScreen();	// LCD 초기화면구동 함수
	BEEP();

	GPIOG->ODR &= ~0x00FF;	// 초기값: LED0~7 Off
	GPIOA->ODR |= (1<<3);	// 초기방향: CW
	GPIOG->ODR |= 0x01; 	// LED0 ON

	TIMER14_PWM_Init();	// TIM14 Init (PWM mode)
        TIMER1_COUNTER_Init();
        _EXTI_Init();
    
	while(1)
	{
		/* SW0~SW3 Duty 변경*/
		switch(KEY_Scan())
		{
			case SW0_PUSH : 	//SW0
				TIM14->CCR1 = 0;		// DR: 0%	  
				LCD_DisplayChar(4,14,0);  //torque 변수 디스플레이
 				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<0);		// LED0 ON 
                                BEEP();
 			break;

			case SW1_PUSH : 	//SW1
				TIM14->CCR1 = 10;		// DR: 10%	  
				LCD_DisplayText(4,14,"10");  //torque 변수 디스플레이
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<1);		// LED1 ON 
                                BEEP();
 			break;

                        case SW2_PUSH  : 	//SW2
				TIM14->CCR1 = 20;		// DR: 20%	  
				LCD_DisplayText(4,14,"20");  //torque 변수 디스플레이
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<2);		// LED2 ON 
                                BEEP();
 			break;

                        case SW3_PUSH  : 	//SW3
				TIM14->CCR1 = 30;		// DR: 30%	  
				LCD_DisplayText(4,14,"30");  //torque 변수 디스플레이
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<3);		// LED3 ON 
                                BEEP();
 			break;
                        
                         case SW4_PUSH  : 	//SW3
				TIM14->CCR1 = 40;		// DR: 40%	  
				LCD_DisplayText(4,14,"40");  //torque 변수 디스플레이
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<4);		// LED4 ON 
                                BEEP();
                         break;
                         
                         case SW5_PUSH  : 	//SW3
				TIM14->CCR1 = 50;		// DR: 50%	  
				LCD_DisplayText(4,14,"50");  //torque 변수 디스플레이
				GPIOG->ODR &= ~0x00FF;		// All LED Off
				GPIOG->ODR |= (1<<5);		// LED5 ON 
                                BEEP();
                         break;
                         
                        
                         
                       
                                
                        
                         
                                
                                
 			break;
                 }      
      
		
	}
}


void TIMER14_PWM_Init(void)
{  
// 모터펄스(PWM)핀:PF9(TIM14 CH1), 
// Clock Enable : GPIOF & TIMER14
	RCC->AHB1ENR	|= (1<<5);	// GPIOF Enable
	RCC->APB1ENR 	|= (1<<8);	// TIMER14 Enable 
    						

    
// PF9을 GPIO  출력설정 
	GPIOF->MODER 	|= (1<<18);	// PA3 Output  mode					
	GPIOF->OSPEEDR 	|= (1<<18);	// PA3 Output speed (25MHz High speed)
	GPIOF->OTYPER	&= ~(1<<9);	// PA3 Output type push-pull (reset state)
        GPIOF->AFR[1]	|= (9<<4); 	// Connect TIM14 pins(PF9) to AF9(TIM12..14)
					// PF9 ==> TIM14_CH1
        
// TIM5 Channel 1 : PWM 1 mode
	// Assign 'PWM Pulse Period'
	TIM14->PSC	= 4200-1;	// Prescaler 84,000,000Hz/8400 = 10,000 Hz(0.1ms)  (1~65536)
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
//	TIM5->CCMR1 	&= ~(1<<3); 	// OC1PE=0: Output Compare 1 preload disable
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
void TIMER1_COUNTER_Init(void)
{
        RCC->AHB1ENR	|= (1<<4);	// 0x08, GPIOE Enable
	RCC->APB2ENR 	|= (1<<0);	// 0x04, TIMER1 Enable 

// PE13: TIM1_CH3
// PE13을 출력설정하고 Alternate function(TIM1_CH3)으로 사용 선언
	GPIOE->MODER 	|= (2<<26);	// 0x02000000(MODER.(25,24)=0b10), GPIOD PIN12 Output Alternate function mode 					
	GPIOE->OSPEEDR 	|= (2<<26);	// 0x02000000(OSPEEDER.(25,24)=0b11), GPIOD PIN12 Output speed (50MHz High speed)
        GPIOE->OTYPER	&= ~(1<<13);	// ~0x1000, GPIOD PIN13 Output type push-pull (reset state)
	GPIOE->PUPDR	&= ~(3<<13); 	// GPIOD PIN12 NO Pull-up
  					// PD12 ==> TIM4_CH1
	GPIOE->AFR[1]	|= (1<<20);	//  Connect TIM1 pins(PE13) to AF1(TIM1..2)
  
// Timerbase Mode
	// Setting CR1 : 0x0000 
	TIM1->CR1 &= ~(1<<4);	// DIR=0(Up counter)(reset state)
	TIM1->CR1 &= ~(1<<1);	// UDIS=0(Update event Enabled): By one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM1->CR1 &= ~(1<<2);	// URS=0(Update event source Selection): one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM1->CR1 &= ~(1<<3);	// OPM=0(The counter is NOT stopped at update event) (reset state)
	TIM1->CR1 &= ~(1<<7);	// ARPE=0(ARR is NOT buffered) (reset state)
	TIM1->CR1 &= ~(3<<8); 	// CKD(Clock division)=00(reset state)
	TIM1->CR1 &= ~(3<<5); 	// CMS(Center-aligned mode Sel)=00 (Edge-aligned mode) (reset state)
				// Center-aligned mode: The counter counts Up and DOWN alternatively

	// PSC, ARR
	TIM1->PSC = 84-1;	// Prescaler=84, 84MHz/84 = 1MHz (1us)
	TIM1->ARR = 500000-1;	// Auto reload  : 1us * 500K = 500ms(period) : 인터럽트주기나 출력신호의 주기 결정
  
        
	// Update(Clear) the Counter
	TIM1->EGR |= (1<<0);    // UG: Update generation  , REG's Update (CNT clear) 

// Output Compare Mode
	// CCMR1(Capture/Compare Mode Register 1) : Setting the MODE of Ch1 or Ch2
	TIM1->CCMR1 |= (1<<0); 	// CC1S(CC1 channel) = '0b01' : Input 
	TIM1->CCMR1 &= ~(15<<4); // IC1F='0b0000: No Input Filter 
				
	// CCER(Capture/Compare Enable Register) : Enable "Channel 1" 
	TIM1->CCER &= ~(1<<0);	// CC1E=0: Capture Disable
	TIM1->CCER &= ~(1<<1);	// CC1P=0: TI1FP1 NonInverting / Rising Edge   
	TIM1->CCER &= ~(1<<3);	// CC1NP=0: TI2FP2   

	// SMCR(Slave Mode Control Reg.) : External Clock Enable
	TIM1->SMCR |= (7<<0);	// SMS(Slave Mode Selection)=0b111 : External Clock Mode 1
	TIM1->SMCR |= (4<<4);	// TS(Trigger Selection)=0b100 : TI1F_ED(TI1 Edge Detector 출력신호)

	TIM1->CR1 |= (1<<0);	// CEN: Enable the Tim4 Counter  	
        
        
        TIM1->BDTR |= (1<<15); //timer `1,8에만 해당되는 설정
}
void TIM1_IRQHandler(void)      //RESET: 0
{
	if ((TIM1->SR & 0x01) != RESET)	// Update interrupt flag 
	{
		TIM4->SR &= ~(1<<0);	// Update Interrupt Claer
		GPIOG->ODR |= 0x80;	// LED7 On
	}
    
	if((TIM1->SR & 0x02) != RESET)	// Capture/Compare 1 interrupt flag
	{
		TIM4->SR &= ~(1<<1);	// CC 1 Interrupt Claer
		GPIOG->ODR &= ~0x80;	// LED7 Off
	}
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
{ 	
        
        GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
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


void _EXTI_Init(void)
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock
	
	GPIOH->MODER 	&= 0x0000FFFF;	// GPIOH PIN8~PIN15 Input mode (reset state)				 
	
		
        SYSCFG->EXTICR[3] |= 0x7000; 	// EXTI15에 대한 소스 입력은 GPIOH로 설정 (EXTICR4) (reset value: 0x0000)	
   
        EXTI->FTSR |= 0x00008000;			// Falling Trigger Enable   (EXTI15:PH15)sw누를때 발생
    
    	  	                                                // EXTI15 인터럽트 mask (Interrupt Enable)
        
    
        EXTI->IMR  |= 0x00008000;  // EXTI,15 인터럽트 mask (Interrupt Enable)

		
	
        NVIC->ISER[1] |= (1<<(8));    // exti  15 enable
}

void DisplayInitScreen(void)
{
	LCD_Clear(RGB_YELLOW);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_BLACK);	// 글자배경색 : Green
	LCD_SetTextColor(RGB_WHITE);	// 글자색 : WHITE

	LCD_DisplayText(0,0,"Motor Control");  // Title


	LCD_SetBackColor(RGB_YELLOW);	//글자배경색
        LCD_SetTextColor(RGB_BLUE);	// 글자색 : BLUE
	LCD_DisplayText(1,0,"<Step Motor>");
	LCD_DisplayText(3,0,"<DC Motor>");
        
        
        LCD_SetBackColor(RGB_YELLOW);	//글자배경색
        LCD_SetTextColor(RGB_BLACK);	// 글자색 : BLACK
        LCD_DisplayText(2,0,"*Speed Level:");
	LCD_DisplayText(4,0,"*Torque Level:");
        
        LCD_SetBackColor(RGB_YELLOW);	//글자배경색
        LCD_SetTextColor(RGB_RED);	// 글자색 : RED
        LCD_DisplayChar(2,13,(speed + 0x30));  //speed 변수 디스플레이
        LCD_DisplayText(4,14,"0");  //torque 변수 디스플레이
        
}



void EXTI15_10_IRQHandler(void)		// EXTI 10~15인터럽트 핸들러
{
 
   
     if(EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?       15번만 남기고 다지움
    {
        EXTI->PR |= 0x8000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
      
	speed+=1;  //speed를 누를떄마다 하나씩 늘림
        
        if (speed ==10)  // speed가 9를 넘어가면 다시 0으로 돌아감
        { 
          speed =0;
        }
        
        period = (speed+1)*10000; //주기는 스피드+1에 10을 곱해서 측정
        
        
        
        
        LCD_DisplayChar(2,13,(speed + 0x30));  //speed 변수 디스플레이 
        
        count+=1; //led 점멸용 카운트 1씩 증가
        
        if (count%2 ==0) //카운트가 짝수인경우
        {
          GPIOG->ODR &= ~(1<<7);		// LED7 OFF 
        }
        else if (count%2 ==1) //카운트가 홀수인 경우
        {
          TIM4->CCR1 = period; //주기 펄스 period만큼 발생 
          GPIOG->ODR |= (1<<7);		// LED7 ON 
        }
          
    }
    
    
    
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

