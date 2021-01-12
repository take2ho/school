#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);
void _EXTI_Init(void);
uint16_t KEY_Scan(void);

void BEEP(void);
void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);



uint8_t	SW0_Flag, SW5_Flag, SW6_Flag, SW7_Flag;


char Total;  //캐릭터 변수로 total:?에 0~9로 넣을 아스키 코드 구현
int coin; //코인 없을시에 커피뽑는 버튼은 돌아가지 않게 하기위한 변수


int main(void)
{
    _GPIO_Init(); // GPIO (LED & SW) 초기화
    LCD_Init();	// LCD 모듈 초기화
    _EXTI_Init();  //exti 초기화
    Total = 0x30;  //total처음에는 0으로 시작
    coin = 0; //코인 없는 상태로 시작
    
    DelayMS(100);
   

    GPIOG->ODR = 0xFF00;	// LED 초기값: LED0~7 Off
    DisplayInitScreen();	// LCD 초기화면
    
    
    while (1)
    {
       if(SW0_Flag && coin==0) //코인이 없을때(있을때는 누르지 못함) 스위치 0번을 눌럴을때
	   {
                 
		GPIOG->ODR ^= 0x01; //0번 led on
                BEEP();  //key 눌렀을때 1번 beep
		
                   LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                   LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                   LCD_SetTextColor(RGB_RED);// 글자색 : Black
                   LCD_DisplayText(1,8,"C");  //Status: C
                   
                   
                   
                   Total= Total+0x01; //코인 한번 오를때마다 total 수가 하나씩 증가
                   
                   if (Total == 0x3A) //total이 9에서는 0으로 변화
                   {
                     Total = 0x30;
                   }
                   
                   LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                   LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                   LCD_SetTextColor(RGB_RED);// 글자색 : Black
                   LCD_DisplayChar(5,7, Total);   //아스키 코드 캐릭터 변수를 써서 0~9순서대로 가는 변수를 LED에 구현
                   coin = 1; //코인이 있는 상태로 변화
                   
                   
                   SW0_Flag = 0; //루프 탈출
                  
       
       
	    }
       if (coin ==0) //코인이 있을때만 스위치 5,6,7번이 작동되게 설정
       {
         SW5_Flag = 0;
       SW6_Flag =0;
       SW7_Flag=0;
    }
       if (coin ==1) //코인이 있을때
       {
      
                    if (SW5_Flag) //5번 스위치 (블랙커피) on
                       {
                            GPIOG->ODR ^= 0x20; //5번 led on
                            BEEP(); //key누를때 beep 한번
                      
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(2,6," cup");   ///BLACK: cup
                     
                     DelayMS(500); //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(2,6," coffee"); //BLACK: coffee
                     
                     DelayMS(500); //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(2,6," water "); // BLACK: water
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(2,6," -     "); //BLACK: -
                     
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                   LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                   LCD_SetTextColor(RGB_RED);// 글자색 : Black
                   LCD_DisplayText(1,8,"S");   //Status: S
                   
                   GPIOG->ODR = 0xFFFFFF00; //led off
                   BEEP();  //커피 완성시 beep 세번
                   BEEP();
                   BEEP();
                   coin = 0; //코인 0개로 초기화
                     SW5_Flag=0; // 루프 빠져나옴
                    
                     
                     
                   }
                   else if (SW6_Flag) //6번 스위치 (설탕커피) on
                   {
                      GPIOG->ODR ^= 0x40; // led 6번 on
                      BEEP();  //key누를때 beep 한번
                      
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(3,6," cup"); //SUGAR: cup
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(3,6," coffee"); //SUGAR: coffee
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(3,6," sugar "); //SUGAR: sugar
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(3,6," water "); //SUGAR: water
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(3,6," -      "); //SUGAR: -
                     
                     
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                   LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                   LCD_SetTextColor(RGB_RED);// 글자색 : Black
                   LCD_DisplayText(1,8,"S");  //Status: -
                   
                   GPIOG->ODR = 0xFFFFFF00; //led off
                   BEEP(); //커피 완성시 beep 세번
                   BEEP();
                   BEEP();
                   coin = 0; //코인 0개로 초기화
                     SW6_Flag=0; //루프 빠져나옴
                     
                     
                
                   }
                   
                   else if (SW7_Flag) //7번 스위치 (믹스커피) on
                   {
                      GPIOG->ODR ^= 0x80; // 7번 led on
                      BEEP();  //key누를때 beep 한번
                      
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(4,6," cup"); //MIXED: cup
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(4,6," coffee"); // MIXED: coffee
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(4,6," sugar "); // MIXED: sugar
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                      LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(4,6," cream "); //MIXED: cream
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(4,6," water ");// MIXED: water
                     
                     DelayMS(500);  //0.5초 딜레이
                     
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                     LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                     LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
                     LCD_DisplayText(4,6," -     "); //MIXED: -
                     
                     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                   LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
                   LCD_SetTextColor(RGB_RED);// 글자색 : Black
                   LCD_DisplayText(1,8,"S");  //Status: S
                   
                   GPIOG->ODR = 0xFFFFFF00; // led off
                   BEEP(); //커피 완성시 beep 세번
                   BEEP();
                   BEEP();
                   coin=0; //코인 0개로 초기화
                     SW7_Flag=0; //루프 빠져나옴
                     
                     
                     
                   }
                   
                   
                  
       }
       
       
                      
    }
  
}

void _GPIO_Init(void)
{
	// LED (GPIO G) 설정
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x000000FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
    
	// SW (GPIO H) 설정 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) 설정 
    	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER 	&= ~0x00000200;	// GPIOF 9 : Push-pull  	
 	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}	

void _EXTI_Init(void)
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock
	
	GPIOH->MODER 	&= 0x0000FFFF;	// GPIOH PIN8~PIN15 Input mode (reset state)				 
	
	SYSCFG->EXTICR[2] |= 0x0007; 	// EXTI8에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
        SYSCFG->EXTICR[3] |= 0x7770; 	// EXTI13,14,15에 대한 소스 입력은 GPIOH로 설정 (EXTICR4) (reset value: 0x0000)	
        
	
	EXTI->FTSR |= 0x00000100;		// Falling Trigger Enable  (EXTI8:PH8) (EXTI13:PH13) (EXTI14:PH14) (EXTI15:PH15)sw누를때 발생
        EXTI->FTSR |= 0x00002000;	
        EXTI->FTSR |= 0x00004000;	
        EXTI->FTSR |= 0x00008000;	
    	EXTI->IMR  |= 0x00000100;  	// EXTI8,13,14,15 인터럽트 mask (Interrupt Enable)
        EXTI->IMR  |= 0x00002000;  
        EXTI->IMR  |= 0x00004000;  
        EXTI->IMR  |= 0x00008000;  
		
	NVIC->ISER[0] |= (1<<23);   // Enable Interrupt EXTI8 Vector table Position 참조  0~31 ISER0, 32~63I SER1......
        NVIC->ISER[1] |= (1<<(8));    // exti  13,14, 15 enable
}

void DisplayInitScreen(void)  //킬때 처음에 나오는 화면 설정
{
    LCD_Clear(RGB_YELLOW);	// 화면 클리어
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
   
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
    LCD_DisplayText(0,1,"MEcha-Coffee");  // Title
    
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_GREEN);// 글자색 : Black
    LCD_DisplayText(1,0,"Status");  
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(1,6,":");  
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_RED);// 글자색 : Black
    LCD_DisplayText(1,8,"S");  
    
    
    
  
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(2,0,"BLACK:");
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
    LCD_DisplayText(2,7,"-");
    
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(3,0,"SUGAR:");  

    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
    LCD_DisplayText(3,7,"-");
     
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(4,0,"MIXED:");  

    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Black
    LCD_DisplayText(4,7,"-");
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(5,0,"TOTAL:");  

    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_RED);// 글자색 : Black
    LCD_DisplayText(5,7,"0");  
    

   

   
   

   
          
   
    
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

void EXTI9_5_IRQHandler(void)		// EXTI 5~9 인터럽트 핸들러
{
    if(EXTI->PR & 0x0100) 		// EXTI8 Interrupt Pending?       8번만 남기고 다지움
    {
        EXTI->PR |= 0x0100; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
	SW0_Flag = 1;
    }
   
      
}


void EXTI15_10_IRQHandler(void)		// EXTI 10~15인터럽트 핸들러
{
   if(EXTI->PR & 0x2000) 		// EXTI13 Interrupt Pending?       13번만 남기고 다지움
    {
        EXTI->PR |= 0x2000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생

	SW5_Flag = 1;
    }
    
    else if(EXTI->PR & 0x4000) 		// EXTI14 Interrupt Pending?       14번만 남기고 다지움
    {
        EXTI->PR |= 0x4000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
	SW6_Flag = 1;
    }
   
     else if(EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?       15번만 남기고 다지움
    {
        EXTI->PR |= 0x8000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
	SW7_Flag = 1;
    }
    
}



void BEEP(void)			/* beep for 30 ms */
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void DelayMS(unsigned short wMS)
{
    register unsigned short i;
    for (i=0; i<wMS; i++)
        DelayUS(1000);         		// 1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
    volatile int Dly = (int)wUS*17;
    for(; Dly; Dly--);
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

typedef struct
{
  __IO uint32_t CR;            // RCC clock control register, Address offset: 0x00  
  __IO uint32_t PLLCFGR;       // RCC PLL configuration register, Address offset: 0x04  
  __IO uint32_t CFGR;          // RCC clock configuration register, Address offset: 0x08  
  __IO uint32_t CIR;           // RCC clock interrupt register, Address offset: 0x0C  
  __IO uint32_t AHB1RSTR;      // RCC AHB1 peripheral reset register, Address offset: 0x10  
  __IO uint32_t AHB2RSTR;      // RCC AHB2 peripheral reset register, Address offset: 0x14  
  __IO uint32_t AHB3RSTR;      // RCC AHB3 peripheral reset register, Address offset: 0x18  
  uint32_t      RESERVED0;     // Reserved, 0x1C                                                                    
  __IO uint32_t APB1RSTR;      // RCC APB1 peripheral reset register, Address offset: 0x20  
  __IO uint32_t APB2RSTR;      // RCC APB2 peripheral reset register, Address offset: 0x24  
  uint32_t      RESERVED1[2];  // Reserved, 0x28-0x2C                                                                
  __IO uint32_t AHB1ENR;       // RCC AHB1 peripheral clock register, Address offset: 0x30  
  __IO uint32_t AHB2ENR;       // RCC AHB2 peripheral clock register, Address offset: 0x34  
  __IO uint32_t AHB3ENR;       // RCC AHB3 peripheral clock register, Address offset: 0x38  
  uint32_t      RESERVED2;     // Reserved, 0x3C                                                                     
  __IO uint32_t APB1ENR;       // RCC APB1 peripheral clock enable register, Address offset: 0x40  
  __IO uint32_t APB2ENR;       // RCC APB2 peripheral clock enable register, Address offset: 0x44  
  uint32_t      RESERVED3[2];  // Reserved, 0x48-0x4C                                                                
  __IO uint32_t AHB1LPENR;     // RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50  
  __IO uint32_t AHB2LPENR;     // RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54  
  __IO uint32_t AHB3LPENR;     // RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58  
  uint32_t      RESERVED4;     // Reserved, 0x5C                                                                     
  __IO uint32_t APB1LPENR;     // RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60  
  __IO uint32_t APB2LPENR;     // RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64  
  uint32_t      RESERVED5[2];  // Reserved, 0x68-0x6C                                                                
  __IO uint32_t BDCR;          // RCC Backup domain control register, Address offset: 0x70  
  __IO uint32_t CSR;           // RCC clock control & status register, Address offset: 0x74  
  uint32_t      RESERVED6[2];  // Reserved, 0x78-0x7C                                                                
  __IO uint32_t SSCGR;         // RCC spread spectrum clock generation register, Address offset: 0x80  
  __IO uint32_t PLLI2SCFGR;    // RCC PLLI2S configuration register, Address offset: 0x84  
  __IO uint32_t PLLSAICFGR;    // RCC PLLSAI configuration register, Address offset: 0x88  
  __IO uint32_t DCKCFGR;       // RCC Dedicated Clocks configuration register, Address offset: 0x8C  
} RCC_TypeDef;
	

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

typedef struct
{
  __IO uint32_t IMR;    // EXTI Interrupt mask register, Address offset: 0x00 
  __IO uint32_t EMR;    // EXTI Event mask register, Address offset: 0x04 
  __IO uint32_t RTSR;   // EXTI Rising trigger selection register,  Address offset: 0x08
  __IO uint32_t FTSR;   // EXTI Falling trigger selection register, Address offset: 0x0C
  __IO uint32_t SWIER;  // EXTI Software interrupt event register,  Address offset: 0x10 
  __IO uint32_t PR;     // EXTI Pending register, Address offset: 0x14 
} EXTI_TypeDef;

typedef struct
{
  __IO uint32_t MEMRMP;       // SYSCFG memory remap register, Address offset: 0x00 
  __IO uint32_t PMC;          // SYSCFG peripheral mode configuration register, Address offset: 0x04
  __IO uint32_t EXTICR[4];    // SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 
  uint32_t      RESERVED[2];  // Reserved, 0x18-0x1C     
  __IO uint32_t CMPCR;        // SYSCFG Compensation cell control register,Address offset: 0x20

} SYSCFG_TypeDef;

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

#define CRC             ((CRC_TypeDef *) CRC_BASE)
#define RCC             ((RCC_TypeDef *) RCC_BASE)
#define FLASH           ((FLASH_TypeDef *) FLASH_R_BASE)

#define SYSCFG          ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI            ((EXTI_TypeDef *) EXTI_BASE)

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
       uint32_t RESERVED0[24];                                   
  __IO uint32_t ICER[8];  // Offset: 0x080 Interrupt Clear Enable Register  
       uint32_t RSERVED1[24];                                    
  __IO uint32_t ISPR[8];  // Offset: 0x100 Interrupt Set Pending Register   
       uint32_t RESERVED2[24];                                   
  __IO uint32_t ICPR[8];  // Offset: 0x180 Interrupt Clear Pending Register
       uint32_t RESERVED3[24];                                   
  __IO uint32_t IABR[8];  // Offset: 0x200 Interrupt Active bit Register      
       uint32_t RESERVED4[56];                                   
  __IO uint8_t  IP[240];  // Offset: 0x300 Interrupt Priority Register (8Bit) 
       uint32_t RESERVED5[644];                                  
  __O  uint32_t STIR;  // Offset: 0xE00 Software Trigger Interrupt Register    
}  NVIC_Type; 
     
// Memory mapping of Cortex-M4 Hardware 
#define SCS_BASE     (0xE000E000)    // System Control Space Base Address 
#define NVIC_BASE   (SCS_BASE +  0x0100)  // NVIC Base Address  
#define NVIC        ((NVIC_Type *)  NVIC_BASE) // NVIC configuration struct                                           

*/ 