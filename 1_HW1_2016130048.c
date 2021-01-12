// ***************************************************
//작성자: 2016130048 김은호
//작성일: 2019.09.24





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

void _RCC_Init(void);
void _GPIO_Init(void);
void _EXTI_Init(void);
uint16_t KEY_Scan(void);
void TIMER3_Init(void);	// General-purpose Timer 3 (Output Compare mode)

void DisplayInitScreen(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);
uint8_t    SW6_Flag; //sw6 작동용 변수

int hour1=1;
int hour2=1;
int min1=5;                                                                                                                                                                                                                                                                                                                                                                    
int min2=9;
int sec1=5;
int sec2=5;
int halfsec=0; //초기 시간값 11:59:55:0으로 시작

int swhour1=0;
int swhour2=0;
int swmin1=0;                                                                                                                                                                                                                                                                                                                                                                    
int swmin2=0;
int swsec1=0;
int swsec2=0; //sw 초기 시간값 0:00:00으로 시작


int pmcount=0; //pm, am 구분용 변수 선언

int stop=0; //stop 기능용 변수 선언

int main(void)
{
	_RCC_Init();
	_GPIO_Init();
	_EXTI_Init();
	LCD_Init();	
	DelayMS(10);
	BEEP();
       
        
   	DisplayInitScreen();	// LCD 초기화면
	GPIOG->ODR &= 0xFF00;	// 초기값: LED0~7 Off
        GPIOG->ODR |= 0x0080; // LED7 ON	
	GPIOG->ODR &= ~0x0040; //led 6번 off  sw led 초기 화면

	TIMER3_Init();		// TIM3 Init (Output Compare mode: UI & CCI 발생, )         
        
        
 
	while(1)
	{
          if (stop==1) //stop일 경우에는 멈춤
          {
            
          }
           
          else if (stop==0) //stop이 아니면 계속 시간 카운트
          {
            
          DelayMS(500);
          halfsec=5;
          DisplayInitScreen();
          
          
          DelayMS(500);
          halfsec=0;
          sec2+=1;
          if (sec2 ==10)
          {
            sec1+=1;
            sec2=0;
          }
          if (sec1 ==6)
          {
            min2+=1;
            sec1=0;
          }
          if(min2 == 10)
          {
            min1+=1;
            min2=0;
          }
          if(min1 ==6)
          { 
            hour2+=1;
            min1=0;
          }
          if(hour2 == 10 && hour1==0)
          {
            hour1+=1;
            hour2=0;
          }
          if(hour2 ==3 && hour1 ==1)
          {
            hour1=0;
            hour2=0;
            pmcount+=1;   //12넘어갈때마다 pm am 바꿀수 있게 카운트
          }   //12:59:59 다음에 0:00:00을 갈수 있게 조건
         
            
          DisplayInitScreen();
          
          }
          
		
	}
}

void TIMER3_Init(void)
{
// PD12: TIM4_CH1
// PD12을 출력설정하고 Alternate function(TIM4_CH1)으로 사용 선언
	RCC->AHB1ENR	|= (1<<3);	// 0x08, RCC_AHB1ENR GPIOD Enable : AHB1ENR.3

	GPIOD->MODER 	|= (2<<24);	// 0x02000000(MODER.(25,24)=0b10), GPIOD PIN12 Output Alternate function mode 					
	GPIOD->OSPEEDR 	|= (3<<24);	// 0x03000000(OSPEEDER.(25,24)=0b11), GPIOD PIN12 Output speed (100MHz High speed)
	GPIOD->OTYPER	&= ~(1<<12);	// ~0x1000, GPIOD PIN12 Output type push-pull (reset state)
	GPIOD->PUPDR	|= (1<<24); 	// 0x01000000, GPIOD PIN12 Pull-up
  					// PD12 ==> TIM4_CH1
	GPIOD->AFR[1]	|= (2<<16);	// 0x00020000(AFR[1].(19~16)=0b0010): Connect TIM4 pins(PD12) to AF2(TIM3..5)
 
// Timerbase Mode
	RCC->APB1ENR |= (1<<2);	// 0x04, RCC_APB1ENR TIMER4 Enable

	// Setting CR1 : 0x0000 
	TIM3->CR1 |= (1<<4);	// DIR=1 (down counter)(reset state)
	TIM3->CR1 &= ~(1<<1);	// UDIS=0(Update event Enabled): By one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM3->CR1 &= ~(1<<2);	// URS=0(Update event source Selection): one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM3->CR1 &= ~(1<<3);	// OPM=0(The counter is NOT stopped at update event) (reset state)
	TIM3->CR1 &= ~(1<<7);	// ARPE=0(ARR is NOT buffered) (reset state)
	TIM3->CR1 &= ~(3<<8); 	// CKD(Clock division)=00(reset state)
	TIM3->CR1 &= ~(3<<5); 	// CMS(Center-aligned mode Sel)=00 (Edge-aligned mode) (reset state)
				// Center-aligned mode: The counter counts Up and DOWN alternatively

	// Deciding the Period
	TIM3->PSC = 840-1;	// Prescaler=840, 840MHz/84 = 10MHz (10us)             840으로 수정 (주기 길어짐)
	TIM3->ARR = 50000-1;	// Auto reload  : 10us * 50K = 500ms(period) : 인터럽트주기나 출력신호의 주기 결정 (

	// Update(Clear) the Counter
	TIM3->EGR |= (1<<0);    // UG: Update generation    

	// Setting an UI(UEV) Interrupt 
	NVIC->ISER[0] 	|= (1<<30);	// Enable Timer4 global Interrupt on NVIC
	TIM3->DIER |= (1<<0);	// UIE: Enable Tim4 Update interrupt
    
// Output Compare Mode
	// CCMR1(Capture/Compare Mode Register 1) : Setting the MODE of Ch1 or Ch2
	TIM3->CCMR1 &= ~(3<<0); // CC1S(CC1 channel) = '0b00' : Output 
        

        
	TIM3->CCMR1 &= ~(1<<2); // OC1FE=0: Output Compare 1 Fast disable 
	TIM3->CCMR1 &= ~(1<<3); // OC1PE=0: Output Compare 1 preload disable(CCR1에 언제든지 새로운 값을 loading 가능) 
	TIM3->CCMR1 |= (3<<4);	// OC1M=0b011 (Output Compare 1 Mode : toggle)
				// OC1REF toggles when CNT = CCR1
				
	// CCER(Capture/Compare Enable Register) : Enable "Channel 1" 
	TIM3->CCER |= (1<<0);	// CC1E=1: CC1 channel Output Enable
				// OC1(TIM4_CH1) Active: 해당핀(100번)을 통해 신호출력
	TIM3->CCER &= ~(1<<1);	// CC1P=0: CC1 channel Output Polarity (OCPolarity_High : OC1으로 반전없이 출력)  

	// CC1I(CC 인터럽트) 인터럽트 발생시각 또는 신호변화(토글)시기 결정: 신호의 위상(phase) 결정
	// 인터럽트 발생시간(10000 펄스)의 10%(1000) 시각에서 compare match 발생
	TIM3->CCR1 = 1000;	// TIM4 CCR1 TIM4_Pulse

	TIM3->DIER |= (1<<1);	// CC1IE: Enable the Tim4 CC1 interrupt

	TIM3->CR1 |= (1<<0);	// CEN: Enable the Tim4 Counter  					
}

void TIM3_IRQHandler(void)      //RESET: 0
{
	int second=0;
    
	TIM3->SR &= ~(1<<0);	// Interrupt flag Clear
    
	second++;								
 	if(second%2 ==1)	// LED3 Toggle 500ms
	{	halfsec = 5;     							   
			
	}
        else
        {
          halfsec = 0;
        }
}

void TIMER5_Init(void)
{
// PD12: TIM5_CH1
// PD12을 출력설정하고 Alternate function(TIM5_CH1)으로 사용 선언
	RCC->AHB1ENR	|= (1<<3);	// 0x08, RCC_AHB1ENR GPIOD Enable : AHB1ENR.3

	GPIOD->MODER 	|= (2<<24);	// 0x02000000(MODER.(25,24)=0b10), GPIOD PIN12 Output Alternate function mode 					
	GPIOD->OSPEEDR 	|= (3<<24);	// 0x03000000(OSPEEDER.(25,24)=0b11), GPIOD PIN12 Output speed (100MHz High speed)
	GPIOD->OTYPER	&= ~(1<<12);	// ~0x1000, GPIOD PIN12 Output type push-pull (reset state)
	GPIOD->PUPDR	|= (1<<24); 	// 0x01000000, GPIOD PIN12 Pull-up
  					// PD12 ==> TIM4_CH1
	GPIOD->AFR[1]	|= (2<<16);	// 0x00020000(AFR[1].(19~16)=0b0010): Connect TIM4 pins(PD12) to AF2(TIM3..5)
 
// Timerbase Mode
	RCC->APB1ENR |= (1<<2);	// 0x04, RCC_APB1ENR TIMER4 Enable

	// Setting CR1 : 0x0000 
	TIM3->CR1 |= (1<<3);	// OPM=1 (output compare)(reset state)
	TIM3->CR1 &= ~(1<<1);	// UDIS=0(Update event Enabled): By one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM3->CR1 &= ~(1<<2);	// URS=0(Update event source Selection): one of following events
				//	- Counter Overflow/Underflow, 
				// 	- Setting the UG bit Set,
				//	- Update Generation through the slave mode controller 
	TIM3->CR1 &= ~(1<<3);	// OPM=0(The counter is NOT stopped at update event) (reset state)
	TIM3->CR1 &= ~(1<<7);	// ARPE=0(ARR is NOT buffered) (reset state)
	TIM3->CR1 &= ~(3<<8); 	// CKD(Clock division)=00(reset state)
	TIM3->CR1 &= ~(3<<5); 	// CMS(Center-aligned mode Sel)=00 (Edge-aligned mode) (reset state)
				// Center-aligned mode: The counter counts Up and DOWN alternatively

	// Deciding the Period
	TIM3->PSC = 8400-1;	// Prescaler=8400, 8400MHz/84 = 100MHz (100us)          
	TIM3->ARR = 10000-1;	// Auto reload  : 100us * 10K = 100ms(period) : 인터럽트주기나 출력신호의 주기 결정 (

	// Update(Clear) the Counter
	TIM3->EGR |= (1<<0);    // UG: Update generation    

	// Setting an UI(UEV) Interrupt 
	NVIC->ISER[0] 	|= (1<<30);	// Enable Timer5 global Interrupt on NVIC
	TIM5->DIER |= (1<<0);	// UIE: Enable Tim5 Update interrupt
    
// Output Compare Mode
	// CCMR1(Capture/Compare Mode Register 1) : Setting the MODE of Ch1 or Ch2
	TIM5->CCMR1 &= ~(3<<0); // CC1S(CC1 channel) = '0b00' : Output 
        

        
	TIM5->CCMR1 &= ~(1<<2); // OC1FE=0: Output Compare 1 Fast disable 
	TIM5->CCMR1 &= ~(1<<3); // OC1PE=0: Output Compare 1 preload disable(CCR1에 언제든지 새로운 값을 loading 가능) 
	TIM5->CCMR1 |= (3<<4);	// OC1M=0b011 (Output Compare 1 Mode : toggle)
				// OC1REF toggles when CNT = CCR1
				
	// CCER(Capture/Compare Enable Register) : Enable "Channel 1" 
	TIM5->CCER |= (1<<0);	// CC1E=1: CC1 channel Output Enable
				// OC1(TIM5_CH1) Active: 해당핀(100번)을 통해 신호출력
	TIM5->CCER &= ~(1<<1);	// CC1P=0: CC1 channel Output Polarity (OCPolarity_High : OC1으로 반전없이 출력)  

	// CC1I(CC 인터럽트) 인터럽트 발생시각 또는 신호변화(토글)시기 결정: 신호의 위상(phase) 결정
	// 인터럽트 발생시간(10000 펄스)의 10%(1000) 시각에서 compare match 발생
	TIM5->CCR1 = 1000;	// TIM45 CCR1 TIM5_Pulse

	TIM5->DIER |= (1<<1);	// CC1IE: Enable the Tim4 CC1 interrupt

	TIM5->CR1 |= (1<<0);	// CEN: Enable the Tim4 Counter  					
}

void TIM5_IRQHandler(void)      //RESET: 0
{
	int second=0;
    
	TIM3->SR &= ~(1<<0);	// Interrupt flag Clear
    
	second++;								
 	if(second%2 ==1)	// LED3 Toggle 500ms
	{	halfsec = 5;     							   
			
	}
        else
        {
          halfsec = 0;
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
}	

void _EXTI_Init(void)
{
    	RCC->AHB1ENR 	|= 0x0080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR 	|= 0x4000;	// Enable System Configuration Controller Clock
	
	GPIOH->MODER 	&= 0x0000FFFF;	// GPIOH PIN8~PIN15 Input mode (reset state)				 
	
	SYSCFG->EXTICR[2] |= 0x0077; 	// EXTI8,9에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
         SYSCFG->EXTICR[3] |= 0x7700; 	// EXTI14, 15에 대한 소스 입력은 GPIOH로 설정 (EXTICR4) (reset value: 0x0000)
	
	EXTI->FTSR |= 0x000100;		// Falling Trigger Enable  (EXTI8:PH8)
	EXTI->RTSR |= 0x000200;		// Rising Trigger  Enable  (EXTI9:PH9) 
        EXTI->FTSR |= 0x008000;		// Falling Trigger Enable  (EXTI15:PH15)
        EXTI->FTSR |= 0x004000;		// Falling Trigger Enable  (EXTI14:PH14)
    	EXTI->IMR  |= 0x000300;  	// EXTI8,9,15,14 인터럽트 mask (Interrupt Enable)
        EXTI->IMR  |= 0x00008000;  
        EXTI->IMR  |= 0x00004000;  
		
	NVIC->ISER[0] |= (1<<23);   	// Enable Interrupt EXTI8,9 Vector table Position 참조
        NVIC->ISER[1] |= (1<<(8));        // enable interrupt exti 14,15
}

void EXTI9_5_IRQHandler(void)		// EXTI 5~9 인터럽트 핸들러
{
	if(EXTI->PR & 0x0100) 		// EXTI8 nterrupt Pending?
	{
		EXTI->PR |= 0x0100; 	// Pending bit Clear
                
                GPIOG->ODR |= 0x0001; // LED0 ON	
                GPIOG->ODR &= ~0x0002; // LED1 OFF
                
                 if (stop==1) //stop 기능이 걸려있는 경우 stop을 풀어줌
                {
                  stop=0; 
                }
                
                
                hour1=0;
                hour2=0;
                min1=0;
                min2=0;
                sec1=0;
                sec2=0;
                halfsec=0;   //모든 시간값 다 0으로 초기화 (clear키)
                
               
                BEEP();  //bz on
		
	}
	else if(EXTI->PR & 0x0200) 	// EXTI9 Interrupt Pending?
	{
		EXTI->PR |= 0x0200; 	// Pending bit Clear
                
                GPIOG->ODR |= 0x0002; // LED1 ON	
                GPIOG->ODR &= ~0x0001; // LED0 OFF
                
                stop=1;
		
                BEEP();  //bz on
	}
}

void EXTI15_10_IRQHandler(void)		// EXTI 10~15인터럽트 핸들러
{
 
      
   
     if(EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?       15번만 남기고 다지움
    {
        EXTI->PR |= 0x8000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
        
        GPIOG->ODR |= 0x0080; // LED7 ON	
	GPIOG->ODR &= ~0x0040; //led 6번 off
        
        int swhour1=0;
        int swhour2=0;
        int swmin1=0;                                                                                                                                                                                                                                                                                                                                                                    
        int swmin2=0;
        int swsec1=0;
        int swsec2=0; //sw 초기화
      
	
       
    }
    else if(EXTI->PR & 0x4000) 		// EXTI14 Interrupt Pending?       14번만 남기고 다지움
    {
      do{                                         //7번 스위치 눌릴떄까지 sw 시간 카운트
         EXTI->PR |= 0x4000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
        
        
        
        GPIOG->ODR |= 0x0040; // LED6 ON	
	GPIOG->ODR &= ~0x0080; //led 7번 off
        
        if (SW6_PUSH  ==1) //6번 스위치 눌리면 stop
        {
       
          GPIOG->ODR &= ~0x0040; //led 6번 off
          GPIOG->ODR &= ~0x0080; //led 7번 off
          do{
          }
          while (SW7_PUSH ==1);
          
        
        }
        
        DelayMS(1000);
          swsec2+=1;
          DisplayInitScreen();
          
          
          
          
          if (swsec2 ==10)
          {
            swsec1+=1;
            swsec2=0;
          }
          if (swsec1 ==6)
          {
            swmin2+=1;
            swsec1=0;
          }
          if(swmin2 == 10)
          {
            swmin1+=1;
            swmin2=0;
          }
          if(swmin1 ==6)
          { 
            swhour2+=1;
            swmin1=0;
          }
          if(swhour2 == 10 && swhour1==0)
          {
            swhour1+=1;
            swhour2=0;
          }
          if(swhour2 ==3 && swhour1 ==1)
          {
            swhour1=0;
            swhour2=0;
            
          }              //12:59:59 다음에 0:00:00을 갈수 있게 조건
         
            
          DisplayInitScreen();
        
        }while(SW6_PUSH  ==1);
    }
    
    
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
	LCD_Clear(RGB_YELLOW);		// 화면 클리어
	LCD_SetFont(&Gulim8);		// 폰트 : 굴림 8
	LCD_SetBackColor(RGB_YELLOW);	// 글자배경색 : yellow
	LCD_SetTextColor(RGB_BLACK);	// 글자색 : Black

	LCD_DisplayText(0,0,"Digital Clock");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
         if (pmcount %2 ==0)
          {
            LCD_DisplayText(1,0,"PM");  // Title

	    LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
            if (hour1==0)
            {
               LCD_DisplayText(1,0,"AM");  // Title

	       LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
            }
          }
          if (pmcount %2 ==1)
          {
            LCD_DisplayText(1,0,"AM");  // Title

	    LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
            
            if (hour1==12)
            {
               LCD_DisplayText(1,0,"PM");  // Title

	       LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
            }
          }
        

        
        LCD_DisplayChar(1,3,(hour1+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(1,4,(hour2+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayText(1,5,":");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(1,6,(min1+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(1,7,(min2+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayText(1,8,":");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(1,9,(sec1+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(1,10,(sec2+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayText(1,11,":");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(1,12,(halfsec+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayText(2,0,"SW");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
         LCD_DisplayChar(2,3,(swhour1+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(2,4,(swhour2+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayText(2,5,":");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(2,6,(swmin1+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(2,7,(swmin2+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayText(2,8,":");  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(2,9,(swsec1+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
        
        LCD_DisplayChar(2,10,(swsec2+0x30));  // Title

	LCD_SetBackColor(RGB_YELLOW);	//글자배경색 : Yellow
}   //시간이 아스키코드로 lcd에 나올수 있게 설정

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

/**************************************************************************
// 보충 설명자료
// 다음은 stm32f4xx.h에 있는 RCC관련 주요 선언문임 
#define HSE_STARTUP_TIMEOUT    ((uint16_t)0x05000)   // Time out for HSE start up 
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

#define FLASH_BASE            ((uint32_t)0x08000000) // FLASH(up to 1 MB) base address in the alias region                          
#define CCMDATARAM_BASE       ((uint32_t)0x10000000) // CCM(core coupled memory) data RAM(64 KB) base address in the alias region   
#define SRAM1_BASE            ((uint32_t)0x20000000) // SRAM1(112 KB) base address in the alias region                              

#if defined(STM32F40_41xxx) 
#define SRAM2_BASE            ((uint32_t)0x2001C000) // SRAM2(16 KB) base address in the alias region                               
#define SRAM3_BASE            ((uint32_t)0x20020000) // SRAM3(64 KB) base address in the alias region                               
#endif   

#define PERIPH_BASE           ((uint32_t)0x40000000) // Peripheral base address in the alias region                                 
#define BKPSRAM_BASE          ((uint32_t)0x40024000) // Backup SRAM(4 KB) base address in the alias region                          

// Peripheral memory map  
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000)

// AHB1 peripherals  
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000)
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)

// APB1 peripherals address
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800)
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00)
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000)

// APB2 peripherals address
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800)

// RCC Structue
typedef struct
{
  __IO uint32_t CR;            // RCC clock control register, Address offset: 0x00  
  __IO uint32_t PLLCFGR;       // RCC PLL configuration register, Address offset: 0x04  
  __IO uint32_t CFGR;          // RCC clock configuration register, Address offset: 0x08  
  __IO uint32_t CIR;           // RCC clock interrupt register, Address offset: 0x0C  
  __IO uint32_t AHB1RSTR;      // RCC AHB1 peripheral reset register, Address offset: 0x10  
  __IO uint32_t AHB2RSTR;      // RCC AHB2 peripheral reset register, Address offset: 0x14  
  __IO uint32_t AHB3RSTR;      // RCC AHB3 peripheral reset register, Address offset: 0x18  
  __IO uint32_t APB1RSTR;      // RCC APB1 peripheral reset register, Address offset: 0x20  
  __IO uint32_t APB2RSTR;      // RCC APB2 peripheral reset register, Address offset: 0x24  
  __IO uint32_t AHB1ENR;       // RCC AHB1 peripheral clock register, Address offset: 0x30  
  __IO uint32_t AHB2ENR;       // RCC AHB2 peripheral clock register, Address offset: 0x34  
  __IO uint32_t AHB3ENR;       // RCC AHB3 peripheral clock register, Address offset: 0x38  
  __IO uint32_t APB1ENR;       // RCC APB1 peripheral clock enable register, Address offset: 0x40  
  __IO uint32_t APB2ENR;       // RCC APB2 peripheral clock enable register, Address offset: 0x44  
  __IO uint32_t AHB1LPENR;     // RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50  
  __IO uint32_t AHB2LPENR;     // RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54  
  __IO uint32_t AHB3LPENR;     // RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58  
  __IO uint32_t APB1LPENR;     // RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60  
  __IO uint32_t APB2LPENR;     // RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64  
  __IO uint32_t BDCR;          // RCC Backup domain control register, Address offset: 0x70  
  __IO uint32_t CSR;           // RCC clock control & status register, Address offset: 0x74  
  __IO uint32_t SSCGR;         // RCC spread spectrum clock generation register, Address offset: 0x80  
  __IO uint32_t PLLI2SCFGR;    // RCC PLLI2S configuration register, Address offset: 0x84  
  __IO uint32_t PLLSAICFGR;    // RCC PLLSAI configuration register, Address offset: 0x88  
  __IO uint32_t DCKCFGR;       // RCC Dedicated Clocks configuration register, Address offset: 0x8C  
} RCC_TypeDef;
	
// FLASH Structue 
typedef struct
{
  __IO uint32_t ACR;      // FLASH access control register,   Address offset: 0x00  
  __IO uint32_t KEYR;     // FLASH key register,              Address offset: 0x04  
  __IO uint32_t OPTKEYR;  // FLASH option key register,       Address offset: 0x08  
  __IO uint32_t SR;       // FLASH status register,           Address offset: 0x0C  
  __IO uint32_t CR;       // FLASH control register,          Address offset: 0x10  
  __IO uint32_t OPTCR;    // FLASH option control register ,  Address offset: 0x14  
  __IO uint32_t OPTCR1;   // FLASH option control register 1, Address offset: 0x18  
} FLASH_TypeDef;

// GPIO Structue 
typedef struct
{
  __IO uint32_t MODER;    // GPIO port mode register,               Address offset: 0x00       
  __IO uint32_t OTYPER;   // GPIO port output type register,        Address offset: 0x04       
  __IO uint32_t OSPEEDR;  // GPIO port output speed register,       Address offset: 0x08       
  __IO uint32_t PUPDR;    // GPIO port pull-up/pull-down register,  Address offset: 0x0C       
  __IO uint32_t IDR;      // GPIO port input data register,         Address offset: 0x10       
  __IO uint32_t ODR;      // GPIO port output data register,        Address offset: 0x14       
  __IO uint16_t BSRRL;    // GPIO port bit set/reset low register,  Address offset: 0x18       
  __IO uint16_t BSRRH;    // GPIO port bit set/reset high register, Address offset: 0x1A       
  __IO uint32_t LCKR;     // GPIO port configuration lock register, Address offset: 0x1C       
  __IO uint32_t AFR[2];   // GPIO alternate function registers,     Address offset: 0x20-0x24  
} GPIO_TypeDef;

// EXTI Structue 
typedef struct
{
  __IO uint32_t IMR;    // EXTI Interrupt mask register, Address offset: 0x00 
  __IO uint32_t EMR;    // EXTI Event mask register, Address offset: 0x04 
  __IO uint32_t RTSR;   // EXTI Rising trigger selection register,  Address offset: 0x08
  __IO uint32_t FTSR;   // EXTI Falling trigger selection register, Address offset: 0x0C
  __IO uint32_t SWIER;  // EXTI Software interrupt event register,  Address offset: 0x10 
  __IO uint32_t PR;     // EXTI Pending register, Address offset: 0x14 
} EXTI_TypeDef;

// SYSCFG Structue 
typedef struct
{
  __IO uint32_t MEMRMP;       // SYSCFG memory remap register, Address offset: 0x00 
  __IO uint32_t PMC;          // SYSCFG peripheral mode configuration register, Address offset: 0x04
  __IO uint32_t EXTICR[4];    // SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 
  __IO uint32_t CMPCR;        // SYSCFG Compensation cell control register,Address offset: 0x20

} SYSCFG_TypeDef;

// Timer Structue 
typedef struct
{
  __IO uint16_t CR1;         // TIM control register 1, Address offset: 0x00
  __IO uint16_t CR2;         // TIM control register 2, 0x04 
  __IO uint16_t SMCR;        // TIM slave mode control register, 0x08
  __IO uint16_t DIER;        // TIM DMA/interrupt enable register, 0x0C 
  __IO uint16_t SR;          // TIM status register, 0x10 
  __IO uint16_t EGR;         // TIM event generation register, 0x14 
  __IO uint16_t CCMR1;       // TIM capture/compare mode register 1, 0x18 
  __IO uint16_t CCMR2;       // TIM capture/compare mode register 2, 0x1C 
  __IO uint16_t CCER;        // TIM capture/compare enable register, 0x20 
  __IO uint32_t CNT;         // TIM counter register, 0x24 
  __IO uint16_t PSC;         // TIM prescaler, 0x28 
  __IO uint32_t ARR;         // TIM auto-reload register, 0x2C 
  __IO uint16_t RCR;         // TIM repetition counter register, 0x30 
  __IO uint32_t CCR1;        // TIM capture/compare register 1, 0x34 
  __IO uint32_t CCR2;        // TIM capture/compare register 2, 0x38 
  __IO uint32_t CCR3;        // TIM capture/compare register 3, 0x3C 
  __IO uint32_t CCR4;        // TIM capture/compare register 4, 0x40 
  __IO uint16_t BDTR;        // TIM break and dead-time register, 0x44 
  __IO uint16_t DCR;         // TIM DMA control register, 0x48 
  __IO uint16_t DMAR;        // TIM DMA address for full transfer, 0x4C 
  __IO uint16_t OR;          // TIM option register, 0x50 
} TIM_TypeDef;

// 각 주변장치 모듈 선언
#define GPIOA 	((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB	((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE  	((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF   ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG   ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH   ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI   ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ   ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK   ((GPIO_TypeDef *) GPIOK_BASE)

#define CRC     ((CRC_TypeDef *) CRC_BASE)
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define FLASH   ((FLASH_TypeDef *) FLASH_R_BASE)

#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI    ((EXTI_TypeDef *) EXTI_BASE)

#define TIM2    ((TIM_TypeDef *) TIM2_BASE)
#define TIM3    ((TIM_TypeDef *) TIM3_BASE)
#define TIM4    ((TIM_TypeDef *) TIM4_BASE)
#define TIM5    ((TIM_TypeDef *) TIM5_BASE)
#define TIM6    ((TIM_TypeDef *) TIM6_BASE)
#define TIM7    ((TIM_TypeDef *) TIM7_BASE)
#define TIM12   ((TIM_TypeDef *) TIM12_BASE)
#define TIM13   ((TIM_TypeDef *) TIM13_BASE)

#define TIM1    ((TIM_TypeDef *) TIM1_BASE)
#define TIM8    ((TIM_TypeDef *) TIM8_BASE)
#define TIM9    ((TIM_TypeDef *) TIM9_BASE)
#define TIM10   ((TIM_TypeDef *) TIM10_BASE)
#define TIM11   ((TIM_TypeDef *) TIM11_BASE)

#define FLASH_ACR_PRFTEN             ((uint32_t)0x00000100)
#define FLASH_ACR_ICEN               ((uint32_t)0x00000200)
#define FLASH_ACR_DCEN               ((uint32_t)0x00000400)
#define FLASH_ACR_ICRST              ((uint32_t)0x00000800)
#define FLASH_ACR_DCRST              ((uint32_t)0x00001000)
#define FLASH_ACR_BYTE0_ADDRESS      ((uint32_t)0x40023C00)
#define FLASH_ACR_BYTE2_ADDRESS      ((uint32_t)0x40023C03)

#define FLASH_ACR_LATENCY_5WS        ((uint32_t)0x00000005)

typedef struct {
  __IO uint32_t ISER[8];  // Offset: 0x000 Interrupt Set Enable Register    
  __IO uint32_t ICER[8];  // Offset: 0x080 Interrupt Clear Enable Register  
  __IO uint32_t ISPR[8];  // Offset: 0x100 Interrupt Set Pending Register   
  __IO uint32_t ICPR[8];  // Offset: 0x180 Interrupt Clear Pending Register
  __IO uint32_t IABR[8];  // Offset: 0x200 Interrupt Active bit Register      
  __IO uint8_t  IP[240];  // Offset: 0x300 Interrupt Priority Register (8Bit) 
  __O  uint32_t STIR;  // Offset: 0xE00 Software Trigger Interrupt Register    
}  NVIC_Type; 
     
// Memory mapping of Cortex-M4 Hardware 
#define SCS_BASE     (0xE000E000)    // System Control Space Base Address 
#define NVIC_BASE   (SCS_BASE +  0x0100)  // NVIC Base Address  
#define NVIC        ((NVIC_Type *)  NVIC_BASE) // NVIC configuration struct                                           

*/ 