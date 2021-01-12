//////////////////////////////////////////////////////////////////////////
// SPI 통신을 이용한 가속도센서 측정
//  NSS pin:  PA8 (PA4(SPI1_CS) 대신에 사용)
//  SCK pin:  PA5 (SPI1_SCK)
//  MOSI pin: PA7 (SPI1_MOSI)
//  MISO pin: PA6 (SPI1_MISO)
// SPI mode: MASTER
// 가속도센서(LIS2HH12, Slave mode) X,Y,Z 값을 250ms마다 읽은후 LCD에 표시 
//////////////////////////////////////////////////////////////////////////
#include "stm32f4xx.h"
#include "GLCD.h"
#include "ACC.h"
#include "FRAM.h"

#define SW0_PUSH        0xFE00  //PH8
#define SW1_PUSH        0xFD00  //PH9
#define SW2_PUSH        0xFB00  //PH10
#define SW3_PUSH        0xF700  //PH11
#define SW4_PUSH        0xEF00  //PH12
#define SW5_PUSH        0xDF00  //PH13
#define SW6_PUSH        0xBF00  //PH14
#define SW7_PUSH        0x7F00  //PH15

void DisplayTitle(void);
void _GPIO_Init(void);
void SPI1_Init(void);
void TIMER1_Init(void);
void TIMER2_Init(void);
void Display_Process(UINT16 *pBuf);
void _EXTI_Init(void);
void _ADC_Init(void);
uint16_t KEY_Scan(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);



//// void SPI1_Process(UINT16 *pBuf);  // ACC.c 
//// void ACC_Init(void) // ACC.c
//// void LCD_Init(void); // GLCD.c

UINT8 bControl;
int SW7_Flag;
uint16_t ADC_Value, Voltage, DIST;
uint8_t str[20];

void ADC_IRQHandler(void)
{
  if(Fram_Read(1126)==1)
  {
	ADC2->SR &= ~(1<<1);		// EOC flag clear

	ADC_Value = ADC2->DR;		// Reading ADC result
     

        

       	sprintf(str,"%4d",ADC_Value);	// ADC result : 12bits (0~4095)
        
        Voltage = ADC_Value * (3.3 * 10) / 4095;   // 3.3 : 4095 =  Volatge : ADC_Value 
                                                    // 10:  소수점아래 첫자리까지 표시하기 위한 값  
      
	DIST= ((Voltage  *10 )+50);   // DISTance 공식
                                                                     
                                                                  
	LCD_DisplayChar(1,17,(DIST/100) + 0x30);
	LCD_DisplayChar(1,18,(DIST%100/10) + 0x30);
        
        if((DIST/100)==0)
        {
          if( (DIST%100/10)<9)
          {
            
	LCD_DisplayText(2,0,"S(DR):10%");
          }
          else
          {
            
	LCD_DisplayText(2,0,"S(DR):20%");
          }
        }
          
	 if((DIST/100)==1)
         {
           if( (DIST%100/10)<3)
           {
             LCD_DisplayText(2,0,"S(DR):20%");
           }
           else if( (DIST%100/10)<7)
           {
             LCD_DisplayText(2,0,"S(DR):30%");
           }
           else
           {
             LCD_DisplayText(2,0,"S(DR):40%");
           }
             
         }
         if((DIST/100)==2)
         {
            if( (DIST%100/10)<1)
            {
              LCD_DisplayText(2,0,"S(DR):40%");
            }
             else if( (DIST%100/10)<5)
             {
              LCD_DisplayText(2,0,"S(DR):50%");
             }
             else if( (DIST%100/10)<9)
            {
              LCD_DisplayText(2,0,"S(DR):60%");
            }
            else
            {
              LCD_DisplayText(2,0,"S(DR):70%");
            }
         }
        if((DIST/100)==3)
        {
           if( (DIST%100/10)<3)
           {
             LCD_DisplayText(2,0,"S(DR):70%");
           }
           else if( (DIST%100/10)<7)
           {
             LCD_DisplayText(2,0,"S(DR):80%");
           }
           else
           {
             LCD_DisplayText(2,0,"S(DR):90%");
           }
           
           }
        
        
        
        
        
        LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(26,13, 100, 13); //사각형 리셋
    
     LCD_SetBrushColor(RGB_RED); //사각형의 색 RED
    LCD_DrawFillRect(26,13,((DIST-5)/10) ,13); //빨간색 사각형의 좌표,  
       
 

 	//Starts conversion of regular channels
	ADC2->CR2 |= ADC_CR2_SWSTART; 	// 0x40000000 (1<<30)
  }
  else if (Fram_Read(1126)==0)
  {
     LCD_DisplayText(1,18,"0");
        LCD_DisplayText(2,18,"S");
        LCD_DisplayText(2,0,"S(DR):00%");
        LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(26,13, 100, 13); //사각형 리셋
  }
}


int main(void)
{
        UINT16 buffer[3];
    
        LCD_Init();		// LCD 구동 함수
        DelayMS(10);		// LCD구동 딜레이
        DisplayTitle();		// LCD 초기화면구동 함수
    
       	_GPIO_Init();		// LED, SW 초기화
        SPI1_Init();        	// SPI1 초기화
        ACC_Init();		
        TIMER1_Init();		
        TIMER2_Init();	
        _EXTI_Init();
        _ADC_Init();
           FRAM_Init();        // FRAM 초기화 
    Fram_Status_Config(); 
        SW7_Flag=0;
   //Starts conversion of regular channels
       	ADC2->CR2 |= ADC_CR2_SWSTART; // 0x40000000 (1<<30) 
        while(1)
        {
          
        
          
          
          
                if(bControl)
                {
                        bControl = FALSE;     
                        SPI1_Process(&buffer[0]);	// SPI통신을 이용하여 가속도센서 측정
                        Display_Process(&buffer[0]);	// 측정값을 LCD에 표시
                }
        }
}

///////////////////////////////////////////////////////
// Master mode, Full-duplex, 8bit frame(MSB first), 
// fPCLK/32 Baud rate, Software slave management EN
void SPI1_Init(void)
{
	/*!< Clock Enable  *********************************************************/
        RCC->APB2ENR 	|= (1<<12);	// 0x1000, SPI1 Clock EN
        RCC->AHB1ENR 	|= (1<<0);	// 0x0001, GPIOA Clock EN		
  
        /*!< SPI1 pins configuration ************************************************/
	
        /*!< SPI1 NSS pin(PA8) configuration : GPIO 핀  */
        GPIOA->MODER 	|= (1<<(2*8));	// 0x00010000, PA8 Output mode
        GPIOA->OTYPER 	&= ~(1<<8); 	// 0x0100, push-pull(reset state)
        GPIOA->OSPEEDR 	|= (3<<(2*8));	// 0x00030000, PA8 Output speed (100MHZ) 
        GPIOA->PUPDR 	&= ~(3<<(2*8));	// 0x00030000, NO Pullup Pulldown(reset state)
    
        /*!< SPI1 SCK pin(PA5) configuration : SPI1_SCK */
        GPIOA->MODER 	|= (2<<(2*5)); 	// 0x00000800, PA5 Alternate function mode
        GPIOA->OTYPER 	&= ~(1<<5); 	// 0020, PA5 Output type push-pull (reset state)
        GPIOA->OSPEEDR 	|= (3<<(2*5));	// 0x00000C00, PA5 Output speed (100MHz)
        GPIOA->PUPDR 	|= (2<<(2*5)); 	// 0x00000800, PA5 Pull-down
        GPIOA->AFR[0] 	|= (5<<(4*5));	// 0x00500000, Connect PA5 to AF5(SPI1)
    
        /*!< SPI1 MOSI pin(PA7) configuration : SPI1_MOSI */    
        GPIOA->MODER 	|= (2<<(2*7));	// 0x00008000, PA7 Alternate function mode
        GPIOA->OTYPER	&= ~(1<<7);	// 0x0080, PA7 Output type push-pull (reset state)
        GPIOA->OSPEEDR 	|= (3<<(2*7));	// 0x0000C000, PA7 Output speed (100MHz)
        GPIOA->PUPDR 	|= (2<<(2*7)); 	// 0x00008000, PA7 Pull-down
        GPIOA->AFR[0] 	|= (5<<(4*7));	// 0x50000000, Connect PA7 to AF5(SPI1)
    
        /*!< SPI1 MISO pin(PA6) configuration : SPI1_MISO */
        GPIOA->MODER 	|= (2<<(2*6));	// 0x00002000, PA6 Alternate function mode
        GPIOA->OTYPER 	&= ~(1<<6);	// 0x0040, PA6 Output type push-pull (reset state)
        GPIOA->OSPEEDR 	|= (3<<(2*6));	// 0x00003000, PA6 Output speed (100MHz)
        GPIOA->PUPDR 	|= (2<<(2*6));	// 0x00002000, PA6 Pull-down
        GPIOA->AFR[0] 	|= (5<<(4*6));	// 0x05000000, Connect PA6 to AF5(SPI1)

       // Init SPI1 Registers 
        SPI1->CR1 |= (1<<2);	// MSTR(Master selection)=1, Master mode
        SPI1->CR1 &= ~(1<<15);	// SPI_Direction_2 Lines_FullDuplex
        SPI1->CR1 &= ~(1<<11);	// SPI_DataSize_8bit
        SPI1->CR1 |= (1<<9);  	// SSM(Software slave management)=1, 
				// NSS 핀 상태가 코딩에 의해 결정
        SPI1->CR1 |= (1<<8);   	// SSI(Internal_slave_select)=1,
				// 현재 MCU를 Master가 이므로 NSS 상태는 'High' 
        SPI1->CR1 &= ~(1<<7);	// LSBFirst=0, MSB transmitted first    
        SPI1->CR1 |= (4<<3);	// BR(BaudRate)=0b100, fPCLK/32 (84MHz/32 = 2.625MHz)
        SPI1->CR1 |= (1<<1);	// CPOL(Clock polarity)=1, CK is 'High' when idle
        SPI1->CR1 |= (1<<0);	// CPHA(Clock phase)=1, 두 번째 edge 에서 데이터가 샘플링
 
        SPI1->CR1 |= (1<<6);	// SPE=1, SPI1 Enable 
}

void _EXTI_Init(void)
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock
	
	GPIOH->MODER 	&= 0x0000FFFF;	// GPIOH PIN8~PIN15 Input mode (reset state)				 
	
	SYSCFG->EXTICR[2] |= 0x0007; 	// EXTI8에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
        SYSCFG->EXTICR[3] |= 0x7000; 	// EXTI15에 대한 소스 입력은 GPIOH로 설정 (EXTICR4) (reset value: 0x0000)	
        
	
	EXTI->FTSR |= 0x00000100;		// Falling Trigger Enable  (EXTI8:PH8) (EXTI13:PH13) (EXTI14:PH14) (EXTI15:PH15)sw누를때 발생
   
        EXTI->FTSR |= 0x00008000;			// Falling Trigger Enable  (EXTI8:PH8) (EXTI13:PH13) (EXTI14:PH14) (EXTI15:PH15)sw누를때 발생
    
    	  	// EXTI8,15 인터럽트 mask (Interrupt Enable)
        
     EXTI->IMR  |= 0x00000100;  	// EXTI8,,15 인터럽트 mask (Interrupt Enable)

        EXTI->IMR  |= 0x00008000;  
		
	NVIC->ISER[0] |= (1<<23);   // Enable Interrupt EXTI8 Vector table Position 참조  0~31 ISER0, 32~63I SER1......
        NVIC->ISER[1] |= (1<<(8));    // exti  15 enable
}

void EXTI15_10_IRQHandler(void)		// EXTI 10~15인터럽트 핸들러
{
 
   
     if(EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?       15번만 남기고 다지움
    {
        
      
        EXTI->PR |= 0x8000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
        LCD_DisplayText(1,18,"0");
        LCD_DisplayText(2,18,"S");
        LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(26,13, 100, 13); //사각형 리셋
        Fram_Write(1126,0);  // FRAM (0~8191)1126번지에 0 저장 
	
       
    }
    
    
    
}
void EXTI9_5_IRQHandler(void)		// EXTI 5~9 인터럽트 핸들러
{
    if(EXTI->PR & 0x0100) 		// EXTI8 Interrupt Pending?       8번만 남기고 다지움
    {
        EXTI->PR |= 0x0100; 
         LCD_DisplayText(2,18,"M");
         Fram_Write(1126,1);  // FRAM (0~8191)1126번지에 1 저장 
         
    }
}


void TIMER1_Init(void)	// 주기 생성: 300ms
{
        RCC->APB2ENR 	|= (1<<0);	// TIMER1 Clock Enable
     
        TIM1->PSC = 840-1;	// Prescaler 84MHz/840 = 0.1MHz (10us)
	TIM1->ARR = 30000-1;	// Auto reload  : 10us * 50K = 300ms(period)

        TIM1->CR1	&= ~(1<<4);	// Countermode = Upcounter (reset state)
        TIM1->CR1 	&= ~(3<<8);	// Clock division = 1 (reset state)
        TIM1->EGR 	|=(1<<0);	// Update Event generation    

        TIM1->DIER 	|= (1<<0);	// Enable Tim1 Update interrupt
        NVIC->ISER[0] 	|= ( 1 << 27 );	// Enable Timer1 global Interrupt
        TIM1->CR1 	|= (1<<0);	// Enable Tim1 Counter    
        
        // Define the corresponding pin by 'Output'  
	TIM1->CCER |= (1<<8);	// CC3E=1: CC3 channel Output Enable
				// OC3(TIM5_CH3) Active: 해당핀을 통해 신호출력
	TIM1->CCER &= ~(1<<9);	// CC3P=0: CC3 channel Output Polarity (OCPolarity_High : OC3으로 반전없이 출력)  

	// 'Mode' Selection : Output mode, toggle  
	TIM1->CCMR2 &= ~(3<<0); // CC3S(CC3 channel) = '0b00' : Output 
	TIM1->CCMR2 &= ~(1<<3); // OC3P=0: Output Compare 3 preload disable
	TIM1->CCMR2 |= (3<<4);	// OC3M=0b011: Output Compare 3 Mode : toggle
				// OC3REF toggles when CNT = CCR3

 	TIM1->CCR3 = 30000;	// TIM1 CCR3 TIM5_Pulse

	////////////////////////////////
	// Disable Tim5 CC3 interrupt
      
	TIM1->CR1 |= (1<<0);	// CEN: Enable the Tim5 Counter  					
}


void TIM1_IRQHandler(void)	// 250ms int
{
        static UINT16 LED_cnt = 0;
    
        TIM1->SR &= ~(1<<0);		//Interrupt flag Clear

	bControl = TRUE;		// 250ms마다 센서 측정
	GPIOG->ODR ^= 0x02;		// Test LED
    
        LED_cnt++;								
        if(LED_cnt >= 2)		// 작동 확인용
        {
                LED_cnt = 0;     		
                GPIOG->ODR ^= 0x01;	// LED0 Toggle 500ms
        }
}

void TIMER2_Init(void)	// 가속도센서 측정 주기 생성: 250ms
{
        RCC->APB1ENR 	|= (1<<1);	// TIMER3 Clock Enable
     
        TIM2->PSC 	= 8400-1;	// Prescaler 84MHz/8400 = 10KHz (0.1ms)  
        TIM2->ARR 	= 2500-1;	// Auto reload  0.1ms * 2500 = 250ms

        TIM2->CR1	&= ~(1<<4);	// Countermode = Upcounter (reset state)
        TIM2->CR1 	&= ~(3<<8);	// Clock division = 1 (reset state)
        TIM2->EGR 	|=(1<<0);	// Update Event generation    

        TIM2->DIER 	|= (1<<0);	// Enable Tim3 Update interrupt
        NVIC->ISER[0] 	|= ( 1 << 29 );	// Enable Timer3 global Interrupt
        TIM2->CR1 	|= (1<<0);	// Enable Tim3 Counter    
}
void TIM2_IRQHandler(void)	// 250ms int
{
        static UINT16 LED_cnt = 0;
    
        TIM2->SR &= ~(1<<0);		//Interrupt flag Clear

	bControl = TRUE;		// 250ms마다 센서 측정
	GPIOG->ODR ^= 0x02;		// Test LED
    
        LED_cnt++;								
        if(LED_cnt >= 2)		// 작동 확인용
        {
                LED_cnt = 0;     		
                GPIOG->ODR ^= 0x01;	// LED0 Toggle 500ms
        }
}

void _ADC_Init(void)
{	// ADC1: PA1(pin 41)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// (1<<0) ENABLE GPIOA CLK (stm32f4xx.h 참조)
	GPIOA->MODER |= (3<<2*1);		// CONFIG GPIOA PIN1(PA1) TO ANALOG IN MODE
						
	RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;	// (1<<8) ENABLE ADC1 CLK (stm32f4xx.h 참조)

	ADC->CCR &= ~(0X1F<<0);		// MULTI[4:0]: ADC_Mode_Independent
	ADC->CCR |=  (1<<16); 		// 0x00010000 ADCPRE:ADC_Prescaler_Div4 (ADC MAX Clock 36MHz, 84Mhz(APB2)/4 = 21MHz)
        
	ADC2->CR1 &= ~(3<<24);		// RES[1:0]=0b00 : 12bit Resolution
	ADC2->CR1 &= ~(1<<8);		// SCAN=0 : ADC_ScanCovMode Disable
	ADC2->CR1 |=  (1<<5);		// EOCIE=1: Interrupt enable for EOC

	ADC2->CR2 &= ~(1<<1);		// CONT=0: ADC_Continuous ConvMode Disable
	ADC2->CR2 &= ~(3<<28);		// EXTEN[1:0]=0b00: ADC_ExternalTrigConvEdge_None
        ADC2->CR2 |= (0x02<<24);	// EXTSEL[3:0]: ADC_ExternalTrig (TIM1_CC3)
	ADC2->CR2 &= ~(1<<11);		// ALIGN=0: ADC_DataAlign_Right
	ADC2->CR2 &= ~(1<<10);		// EOCS=0: The EOC bit is set at the end of each sequence of regular conversions

	ADC2->SQR1 &= ~(0xF<<20);	// L[3:0]=0b0000: ADC Regular channel sequece length 
					// 0b0000:1 conversion)
    
        ADC2->SMPR2 |= (0x7<<(3*1));	// ADC2_CH1Sample TIme_480Cycles (3*Channel_1)
 	//Channel selection, The Conversion Sequence of PIN1(ADC1_CH1) is first, Config sequence Range is possible from 0 to 17
	ADC2->SQR3 |= (1<<0);		// SQ1[4:0]=0b0001 : CH1

	NVIC->ISER[0] |= (1<<18);	// Enable ADC global Interrupt

	ADC2->CR2 |= (1<<0);		// ADON: ADC ON

}


void Display_Process(UINT16 *pBuf)
{
          
        
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

        GPIOG->ODR &= 0x00;	// LED0~7 Off 
}

void DelayMS(unsigned short wMS)
{
        register unsigned short i;

        for (i=0; i<wMS; i++)
                DelayUS(1000);		//1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
        volatile int Dly = (int)wUS*17;
         for(; Dly; Dly--);
}

void DisplayTitle(void)
{
        LCD_Clear(RGB_YELLOW);
        LCD_SetFont(&Gulim8);		//폰트 
        LCD_SetBackColor(RGB_YELLOW);
        LCD_SetTextColor(RGB_BLACK);    //글자색
        LCD_DisplayText(0,0,"Tracking Car");  // Title
		
        LCD_SetBackColor(RGB_YELLOW);    //글자배경색

	LCD_DisplayText(1,0,"D:");	
        LCD_DisplayText(1,18,"0");	
	LCD_DisplayText(2,0,"S(DR):00%");
	LCD_DisplayText(2,18,"M");	

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
