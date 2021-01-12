//************************************************

// 작성자: 2014132017 석상훈	

// 작성일: 2019.09.24
#include "stm32f4xx.h"
#include "GLCD.h"

typedef struct Digital_Clock{
  
  int ten_hour, hour, ten_min, min, ten_sec, sec, msec;
  int flag,SW_flag;
  
}Clock;
//시계와 스탑워치의 시간 설정이 비슷하므로 묶어서 관리하기위해 구조체로 정의

void Tim5_init(void);
void TIM5_IRQHandler(void);

void Tim3_init(void);
void TIM3_IRQHandler(void);

void _EXTI_Init(void);
void _GPIO_Init(void);
void Display_LCD(void);



void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);

uint8_t	count=0,pm_flag=1; 
//count 인터럽트 발생횟수 체크, pm_flag 오후 인지 오전인지 판단

Clock watch={1,1,5,9,5,5,0,0,0},stop_watch={0, 0, 0, 0, 0, 0, 0, 0,0};
//시계의 초기값과 스탑워치의 초기값 대입


int main(void){
  
  
  _GPIO_Init();			//GPIO 초기화
  _EXTI_Init();			//인터럽트 초기화
  Tim3_init();			//TIM3 초기화
  Tim5_init();			//TIM5 초기화
  LCD_Init();			//LCD초기화
  Display_LCD();		//초기 화면 출력
  GPIOG->ODR |= 0x01;		//LED0 ON
  GPIOG->ODR |= 0x80;		//LED7 ON
  count=0, watch.msec=0;	//count값, 시계의 ms값 초기화
  
  while(1){
    
    switch(watch.SW_flag)//시계 모드 선택
    {
      
    case 1 :																										//SW1 ON
      
      if(watch.flag==1){                    
        GPIOG->ODR |= 0x01;																							//LED0 ON
        GPIOG->ODR &= ~(0x02);	
	//시계의 시간 값들을 0으로 초기화																					//LED1 OFF
        watch.ten_hour=0, watch.hour=0, watch.ten_min=0, watch.min=0, watch.ten_sec=0, watch.sec=0, watch.msec=0;	
        LCD_DisplayText(1, 0, "AM");																				//시계의 모든 시간값을 초기화
        LCD_DisplayText(1, 3, "00:00:00:0");																		//시계의 모든 시간값을 초기화
        BEEP();																										//BEEP on
        watch.SW_flag=0;																							//시계의 스위치 flag 초기화
        watch.flag=0;																								//시계의 정지 기능 OFF
        count=0;																									//count 초기화
        TIM3->CR1 |= (1<<0);																						// Enable the Tim3 Counter 
      }
      
      break;
      
      
    case 2 :				//SW2 -> SW정지모드
      
      GPIOG->ODR &= ~(0x01);		//LED0 OFF
      GPIOG->ODR |= 0x02;		//LED1 ON
      TIM3->CR1 &= ~(1<<0);		// Disable the Tim3 Counter 
      BEEP();				//BEEP on
      watch.SW_flag=0;			//시계의 스위치 flag 초기화
      watch.flag=1;			//시간 초기화
      break;
      
      
    }
    switch(stop_watch.SW_flag){		//스탑워치 모드 선택
      
    case 1 :				//SW6
      
      
      if((stop_watch.flag%2)==1){	//스탑워치 시작모드
        
        GPIOG->ODR |= 0x40;		//LED6 ON
        GPIOG->ODR &= ~(0x80);		//LED7 OFF
      }
      
      else if((stop_watch.flag%2)==0){	//스탑워치 중지 모드
        
        GPIOG->ODR &= ~(0x40);		//LED6 OFF
        GPIOG->ODR &= ~(0x80);		//LED7 OFF
        stop_watch.flag=0;		//스탑워치 모드 선택 초기화
        stop_watch.SW_flag=0;		//스탑워치 SW 초기화
      }
      
      break;				//case1 를 빠져나옴
      
      
    case 2 :																													//SW7
      stop_watch.SW_flag=1;																										//밑에 조건식에 부합하지 않은 경우 스탑워치가 계속 진행되게 설정
      if((stop_watch.flag%2)==0){																								//스탑워치 초기화
        BEEP();																													//BEEP on
        stop_watch.SW_flag=0;																									//스탑워치 SW 초기화
        GPIOG->ODR &= ~(0x40);																									//LED6 OFF
        GPIOG->ODR |= 0x80;	
	//스탑워치의 모든 시간값을 초기화																									//LED7 ON
        stop_watch.hour=0, stop_watch.ten_min=0, stop_watch.min=0, stop_watch.ten_sec=0, stop_watch.sec=0, stop_watch.msec=0;	
        stop_watch.flag=0;																										//스탑워치 모드 초기화 
        LCD_DisplayText(2, 0, "SW 0:00:00:0");																					//초기화된 스탑워치의 시간값 출력                           
      }
      
      break;																													//case2 를 빠져나옴
      
      
    }  
  }
}

void Tim3_init(void){
  
  RCC->APB1ENR |= 0x02;		// RCC_APB1ENR TIMER3 Enable
  
  NVIC->ISER[0] |= (1<<29); 	// Enable Timer3 global Interrupt
  
  TIM3->PSC = 840-1;		// Prescaler 84,000,000Hz/840 = 100,000 Hz   (1~65536)
  TIM3->ARR = 50000;		// Auto reload  0.01ms * 50k = 500ms
  
  TIM3->CR1 |= (1<<4);		// Downcounter-Mode
  TIM3->CR1 &= ~(3<<8); 	// CKD(Clock division)=1(reset state) 
  TIM3->CR1 &= ~(3<<5);		// CMS(Center-aligned mode Sel): No(reset state)
  
  TIM3->EGR |=(1<<0);		// Update generation  
  
  TIM3->DIER |= (1<<0);		// Enable the Tim3 Update interrupt 
  
  TIM3->CR1 |= (1<<0);		// Enable the Tim3 Counter 
  
  
}

void Tim5_init(void){
  
  RCC->APB1ENR |= (1<<3);	//RCC_APB1ENR TIMER5 Enable
  
  NVIC->ISER[1] |= ( 1 << 18 ); // Enable Timer5 global Interrupt
  
  TIM5->PSC = 8400-1;		// Prescaler 84,000,000Hz/8400 = 10,000 Hz   (1~65536)
  TIM5->ARR = 1000;		// Auto reload  0.1ms * 1k = 100ms
  
  TIM5->CR1 |= (1<<4);		// Downcounter-Mode
  TIM5->CR1 &= ~(3<<8); 	// CKD(Clock division)=1(reset state) 
  TIM5->CR1 &= ~(3<<5); 	// Edge-aligned mode on
  
  TIM5->EGR |=(1<<0);		// Update generation  
  TIM5->EGR |=(1<<4);		// CC4 generation on
  
  
  TIM5->DIER |= (1<<0);		// Enable the Tim5 Update interrupt 
  
  TIM5->CCMR2 &= ~(3<<8);       //CC4 channel를 Output으로 설정
  TIM5->CCMR2 &= ~(0<<10);      //Output compare 4 fast disable
  TIM5->CCMR2 &= ~(0<<11);      //Output compare 4 preload disable
  TIM5->CCMR2 |= (3<<12);       //Output compare 4 mode on
  
  TIM5->CCER &= (1<<12);        //capture/compare 4 output enalbe
  TIM5->CCER &= ~(1<<13);       //capture/compare 4 polarity disable
  
  TIM5->CCR4 = 1000;		//위상차 없이 ARR에의해 UI과 동일하게 CC4 인터럽트 발생
  
  TIM5->DIER |= (1<<4);		// CC4IE: Enable the Tim5 CC4 interrupt
  
  TIM5->CR1 |= (1<<0);		// Enable the Tim5 Counter 
  
  
}


void TIM3_IRQHandler(void)  	// 500ms Interrupt
{
  
  TIM3->SR &= ~(1<<0);		// Interrupt flag Clear         
  
  count++;			//인터럽트 발생체크
  
  if(count==1){			//최초 0.5초 지난경우
    LCD_DisplayChar(1,12,0x35);	// ms 단위에 5를 출력 
  }
  else if(count>=2){		//최초 1초 지난 경우
    LCD_DisplayChar(1,12,0x30);	// ms 단위에 0을 출력
    count=0;			//count 초기화
    
  }
  
  watch.msec++;//ms 1번씩 증가
  
  switch(((watch.msec%2)==0)&&(watch.msec>=2)){
    //ms가 2번 이상 짝수 인경우만 실행되게 설정->1초가 올라가야 하는 경우를 특정
  case 1:
    
    watch.sec++;			//초단위에 1 증가
    watch.msec=0;			//ms를 초기화
    LCD_DisplayChar(1,10,0x30+(watch.sec%10)); //초단위에 0~9까지만 출력
    
    if(watch.sec>=10){			//초단위가 10초 이상이 된경우
      
      watch.ten_sec++;  		//십초단위 1증가
      LCD_DisplayChar(1,9,0x30+(watch.ten_sec%6));//십초단위에 0~5까지만 출력
      
      watch.sec=0;      		//초 단위 초기화
      
      if(watch.ten_sec>=6){ 		//십초 단위가 6이상이 된 경우
        watch.min++;
        LCD_DisplayChar(1,7,0x30+(watch.min%10));//분단위에 0~9까지만 출력
        
        watch.ten_sec=0; 		//십초 단위 초기화
        
        if(watch.min>=10){      	//분 단위가 10이상이 된 경우
          
          watch.ten_min++;     	 	//십분 단위에 1 증가
          LCD_DisplayChar(1,6,0x30+(watch.ten_min%6));//십분단위에 0~5까지만 출력
          
          watch.min=0;          	//분 단위 초기화
          
          if(watch.ten_min>=6){ 	//십분 단위가 6이상이 된 경우
            watch.hour++;       	//시 단위 1 증가
            
            LCD_DisplayChar(1,4,0x30+(watch.hour%10));//시단위에 0~9까지만 출력
            
            watch.ten_min=0;    	//십분 단위 초기화
            
            if((watch.hour>=10) ||((watch.ten_hour>=1)&&(watch.hour>=2))){ //시 단위가 10이상이거나 12시 이상이 된 경우
              watch.ten_hour++; 	//열시 단위 1 증가
              
              LCD_DisplayChar(1,3,0x30+(watch.ten_hour%2)); //열시단위에 0,1만 출력                    
              
              if((watch.ten_hour>=1)&&(watch.hour>=2)&&(pm_flag==1)){ 
                //열시 단위가 1이고 시 단위가 2 이면서 PM 인경우
                LCD_DisplayChar(1, 3, 0x30);    //열시 단위에 0 출력
                LCD_DisplayChar(1, 4, 0x30);    //시 단위에 0 출력
                LCD_DisplayText(1, 0, "AM");    //PM/AM 출력부에 AM 출력
                
                pm_flag=0;                      //PM_flag=0로 넣어 AM으로 표현
                watch.ten_hour=0;               //열시 단위 0으로 초기화
                watch.hour=0;                   //시 단위 0으로 초기화
              }
              
              else if(((watch.ten_hour>=1)&&(watch.hour>=2)&&(pm_flag==0))||(pm_flag==2)){
                //열시 단위가 1이고 시 단위가 2 이면서 AM 인경우
                
                pm_flag=2;                      //PM_flag=2로 넣어 PM으로 표현(13시를 PM 01시로바꿔야하므로 2로 표현)
                LCD_DisplayText(1, 0, "PM");    //PM/AM 출력부에 PM 출력
                if(pm_flag==2){ 
                  LCD_DisplayChar(1, 3, 0x30);    //열시 단위에 0 출력
                  LCD_DisplayChar(1, 4, 0x31);    //시 단위에 1 출력
                  
                  watch.ten_hour=0;             //열시 단위 0으로 초기화
                  watch.hour=1;                 //시 단위 1로 초기화
                  pm_flag=1;                    //1로 넣어 시간이 지나면 다시 AM으로 바뀔수 있도록 함
                }
              }
            } 
          }
        }
      }
    }
    break;
  }
}


void TIM5_IRQHandler(void){
  if ((TIM5->SR & 0x01) != RESET)	// Update interrupt flag (50ms)
  {
    TIM5->SR &= ~(1<<0);		// Update Interrupt Claer
  }
  
  if((TIM5->SR & 0x10) != RESET)	// Capture/Compare 4 interrupt flag
  {
    TIM5->SR &= ~(1<<4);		// CC 4 Interrupt Claer
    
    
    if(stop_watch.SW_flag==1){		//스탑워치 실행모드
      stop_watch.msec++;		//CC 4 인터럽트 발생시 ms 1 증가
      LCD_DisplayChar(2,11,0x30+(stop_watch.msec%10));	//증가된 ms를 0~9에서만 출력
      
      if(stop_watch.msec>=10){		//ms가 10번 이상인경우
        
        stop_watch.msec=0;		//ms를 0으로 초기화
        stop_watch.sec++;		//초단위를 1 증가
        LCD_DisplayChar(2,9,0x30+(stop_watch.sec%10));//증가된 초단위를 0~9에서만 출력
        
        if(stop_watch.sec>=10){		//초단위가 10 이상이 된 경우
          	
          stop_watch.sec=0;		//0초로 초기화
          stop_watch.ten_sec++; 	//십초 단위를 1증가
          LCD_DisplayChar(2,8,0x30+(stop_watch.ten_sec%6)); //증가된 십초 단위를 0~5까지에서만 출력
          
          if(stop_watch.ten_sec>=6){	//십초 단위가 6이상인 경우
            
            stop_watch.min++;		//분단위를 1 증가
            stop_watch.ten_sec=0;	//십초 단위를 0으로 초기화
            LCD_DisplayChar(2,6,0x30+(stop_watch.min%10));//분단위를 0~9에서만 출력
            
            if(stop_watch.min>=10){	//분단위가 10 이상이 된 경우
              
              stop_watch.ten_min++;	//십분단위 1 증가
              stop_watch.min=0;		//분단위 0으로 초기화
              LCD_DisplayChar(2,5,0x30+(stop_watch.ten_min%6));	//증가된 10분 단위를 0~5에서만 출력
              
              if(stop_watch.ten_min>=6){//10분 단위가 6이상인 경우
                
                stop_watch.hour++;	//시 단위를 1 증가
                stop_watch.ten_min=0;	//십분 단위를 0으로 초기화
                LCD_DisplayChar(2,3,0x30+(stop_watch.hour));//증가된 시 단위를 출력
                
              }
            }
          }
        }
      }
    }      
  }
}



void _GPIO_Init(void)
{
  // LED (GPIO G) 설정
  RCC->AHB1ENR	|=  0x00000040;		// RCC_AHB1ENR : GPIOG(bit#6) Enable							
  GPIOG->MODER 	|=  0x00005555;		// GPIOG 0~7 : Output mode (0b01)						
  GPIOG->OTYPER	&= ~0x000000FF;		// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
  GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
  
  // SW (GPIO H) 설정 
  RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
  GPIOH->MODER 	&= ~0xFFFF0000;		// GPIOH 8~15 : Input mode (reset state)				
  GPIOH->PUPDR 	&= ~0xFFFF0000;		// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state
  
  // Buzzer (GPIO F) 설정 
  RCC->AHB1ENR	|=  0x00000020;		// RCC_AHB1ENR : GPIOF(bit#5) Enable							
  GPIOF->MODER 	|=  0x00040000;		// GPIOF 9 : Output mode (0b01)						
  GPIOF->OTYPER 	&= ~0x00000200;	// GPIOF 9 : Push-pull  	
  GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}

void Display_LCD(void){
  
  
  LCD_Clear(RGB_YELLOW);		// 화면 클리어
  LCD_SetFont(&Gulim8);			// 폰트 : 굴림 8
  LCD_SetBackColor(RGB_YELLOW);		// 글자배경색 : Yellow
  LCD_SetTextColor(RGB_BLACK);		// 글자색 : Black
  LCD_DisplayText(0, 0, "Digital Clock");// Title
  
  
  LCD_DisplayText(1, 0, "PM");		//초기 시계의 AM/PM값 출력
  LCD_DisplayText(1, 3, "11:59:55:0");	//초기 시계의 시간값 출력
  LCD_DisplayText(2, 0, "SW 0:00:00:0");//초기 스탑워치의 시간값 출력
}


void EXTI15_10_IRQHandler(void){
  
  
  if (EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?
  {
    EXTI->PR |= 0x8000; 		// Pending bit Clear
    
    
    stop_watch.SW_flag = 2;         	//스탑워치 초기화모드 진입값
    
    
  }
  else if (EXTI->PR & 0x4000) 		// EXTI14 Interrupt Pending?
  {
    EXTI->PR |= 0x4000; 		// Pending bit Clear
    BEEP();                         	//BEEP on
    stop_watch.SW_flag=1;           	//스탑워치 정지모드/실행모드 진입값
    stop_watch.flag++;              	//정지모드 실행모드 중 1개만 시작되게 만드는 값
    
  }
  
}

void EXTI9_5_IRQHandler(void)		// EXTI 5~9 인터럽트 핸들러 소스는 5개인데 여기선 2개만 선택해서 씀.
{
  if(EXTI->PR & 0x0100) 		// EXTI8 Interrupt Pending?
  {
    EXTI->PR |= 0x0100; 		// Pending bit Clear
    watch.SW_flag = 1;			//시계 초기화모드 진입값
  }
  else if(EXTI->PR & 0x0200) 		// EXTI9 Interrupt Pending?
  {
    EXTI->PR |= 0x0200; 		// Pending bit Clear 주로 0으로 지우는 데 외부 인터럽트는 1을 써야 지워짐.
    watch.SW_flag = 2;			//시계 정지모드 진입값
  }
}

void _EXTI_Init(void)
{
  RCC->AHB1ENR 	|= 0x0080;		// RCC_AHB1ENR GPIOH Enable
  RCC->APB2ENR 	|= 0x4000;		// Enable System Configuration Controller Clock
  
  GPIOH->MODER 	&= 0x0000FFFF;		// GPIOH PIN8~PIN15 Input mode (reset state)				 
  
  SYSCFG->EXTICR[2] |= 0x0077; 		// EXTI8,9에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
  SYSCFG->EXTICR[3] |= (7<<12);		// EXTI15에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
  SYSCFG->EXTICR[3] |= (7<<8);		// EXTI14에 대한 소스 입력은 GPIOH로 설정 (EXTICR3) (reset value: 0x0000)	
  
  
  EXTI->RTSR |= 0x000300;		// Rising Trigger  Enable  (EXTI9,8:PH9,8) 
  EXTI->RTSR |= (3<<14);		// Rising Trigger  Enable  (EXTI14,15:PH14,15) 
  
  EXTI->IMR  |= 0x000300;		// EXTI8,9 인터럽트 mask (Interrupt Enable)
  EXTI->IMR |= (3<<14);			// EXTI14,15 인터럽트 mask (Interrupt Enable)
  
  NVIC->ISER[0] |= ( 1 << 23 );		// Enable Interrupt EXTI8,9 Vector table Position 참조
  
  NVIC->ISER[1] |= ( 1 << 8 );		//Enable Interrupt EXTI15_10 Vector table Position 참조
  
}

void BEEP(void)			/* beep for 30 ms */
{ 	GPIOF->ODR |= 0x0200;	// PF9 'H' Buzzer on
DelayMS(30);			// Delay 30 ms
GPIOF->ODR &= ~0x0200;		// PF9 'L' Buzzer off
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

