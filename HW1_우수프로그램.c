//************************************************

// �ۼ���: 2014132017 ������	

// �ۼ���: 2019.09.24
#include "stm32f4xx.h"
#include "GLCD.h"

typedef struct Digital_Clock{
  
  int ten_hour, hour, ten_min, min, ten_sec, sec, msec;
  int flag,SW_flag;
  
}Clock;
//�ð�� ��ž��ġ�� �ð� ������ ����ϹǷ� ��� �����ϱ����� ����ü�� ����

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
//count ���ͷ�Ʈ �߻�Ƚ�� üũ, pm_flag ���� ���� �������� �Ǵ�

Clock watch={1,1,5,9,5,5,0,0,0},stop_watch={0, 0, 0, 0, 0, 0, 0, 0,0};
//�ð��� �ʱⰪ�� ��ž��ġ�� �ʱⰪ ����


int main(void){
  
  
  _GPIO_Init();			//GPIO �ʱ�ȭ
  _EXTI_Init();			//���ͷ�Ʈ �ʱ�ȭ
  Tim3_init();			//TIM3 �ʱ�ȭ
  Tim5_init();			//TIM5 �ʱ�ȭ
  LCD_Init();			//LCD�ʱ�ȭ
  Display_LCD();		//�ʱ� ȭ�� ���
  GPIOG->ODR |= 0x01;		//LED0 ON
  GPIOG->ODR |= 0x80;		//LED7 ON
  count=0, watch.msec=0;	//count��, �ð��� ms�� �ʱ�ȭ
  
  while(1){
    
    switch(watch.SW_flag)//�ð� ��� ����
    {
      
    case 1 :																										//SW1 ON
      
      if(watch.flag==1){                    
        GPIOG->ODR |= 0x01;																							//LED0 ON
        GPIOG->ODR &= ~(0x02);	
	//�ð��� �ð� ������ 0���� �ʱ�ȭ																					//LED1 OFF
        watch.ten_hour=0, watch.hour=0, watch.ten_min=0, watch.min=0, watch.ten_sec=0, watch.sec=0, watch.msec=0;	
        LCD_DisplayText(1, 0, "AM");																				//�ð��� ��� �ð����� �ʱ�ȭ
        LCD_DisplayText(1, 3, "00:00:00:0");																		//�ð��� ��� �ð����� �ʱ�ȭ
        BEEP();																										//BEEP on
        watch.SW_flag=0;																							//�ð��� ����ġ flag �ʱ�ȭ
        watch.flag=0;																								//�ð��� ���� ��� OFF
        count=0;																									//count �ʱ�ȭ
        TIM3->CR1 |= (1<<0);																						// Enable the Tim3 Counter 
      }
      
      break;
      
      
    case 2 :				//SW2 -> SW�������
      
      GPIOG->ODR &= ~(0x01);		//LED0 OFF
      GPIOG->ODR |= 0x02;		//LED1 ON
      TIM3->CR1 &= ~(1<<0);		// Disable the Tim3 Counter 
      BEEP();				//BEEP on
      watch.SW_flag=0;			//�ð��� ����ġ flag �ʱ�ȭ
      watch.flag=1;			//�ð� �ʱ�ȭ
      break;
      
      
    }
    switch(stop_watch.SW_flag){		//��ž��ġ ��� ����
      
    case 1 :				//SW6
      
      
      if((stop_watch.flag%2)==1){	//��ž��ġ ���۸��
        
        GPIOG->ODR |= 0x40;		//LED6 ON
        GPIOG->ODR &= ~(0x80);		//LED7 OFF
      }
      
      else if((stop_watch.flag%2)==0){	//��ž��ġ ���� ���
        
        GPIOG->ODR &= ~(0x40);		//LED6 OFF
        GPIOG->ODR &= ~(0x80);		//LED7 OFF
        stop_watch.flag=0;		//��ž��ġ ��� ���� �ʱ�ȭ
        stop_watch.SW_flag=0;		//��ž��ġ SW �ʱ�ȭ
      }
      
      break;				//case1 �� ��������
      
      
    case 2 :																													//SW7
      stop_watch.SW_flag=1;																										//�ؿ� ���ǽĿ� �������� ���� ��� ��ž��ġ�� ��� ����ǰ� ����
      if((stop_watch.flag%2)==0){																								//��ž��ġ �ʱ�ȭ
        BEEP();																													//BEEP on
        stop_watch.SW_flag=0;																									//��ž��ġ SW �ʱ�ȭ
        GPIOG->ODR &= ~(0x40);																									//LED6 OFF
        GPIOG->ODR |= 0x80;	
	//��ž��ġ�� ��� �ð����� �ʱ�ȭ																									//LED7 ON
        stop_watch.hour=0, stop_watch.ten_min=0, stop_watch.min=0, stop_watch.ten_sec=0, stop_watch.sec=0, stop_watch.msec=0;	
        stop_watch.flag=0;																										//��ž��ġ ��� �ʱ�ȭ 
        LCD_DisplayText(2, 0, "SW 0:00:00:0");																					//�ʱ�ȭ�� ��ž��ġ�� �ð��� ���                           
      }
      
      break;																													//case2 �� ��������
      
      
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
  
  TIM5->CCMR2 &= ~(3<<8);       //CC4 channel�� Output���� ����
  TIM5->CCMR2 &= ~(0<<10);      //Output compare 4 fast disable
  TIM5->CCMR2 &= ~(0<<11);      //Output compare 4 preload disable
  TIM5->CCMR2 |= (3<<12);       //Output compare 4 mode on
  
  TIM5->CCER &= (1<<12);        //capture/compare 4 output enalbe
  TIM5->CCER &= ~(1<<13);       //capture/compare 4 polarity disable
  
  TIM5->CCR4 = 1000;		//������ ���� ARR������ UI�� �����ϰ� CC4 ���ͷ�Ʈ �߻�
  
  TIM5->DIER |= (1<<4);		// CC4IE: Enable the Tim5 CC4 interrupt
  
  TIM5->CR1 |= (1<<0);		// Enable the Tim5 Counter 
  
  
}


void TIM3_IRQHandler(void)  	// 500ms Interrupt
{
  
  TIM3->SR &= ~(1<<0);		// Interrupt flag Clear         
  
  count++;			//���ͷ�Ʈ �߻�üũ
  
  if(count==1){			//���� 0.5�� �������
    LCD_DisplayChar(1,12,0x35);	// ms ������ 5�� ��� 
  }
  else if(count>=2){		//���� 1�� ���� ���
    LCD_DisplayChar(1,12,0x30);	// ms ������ 0�� ���
    count=0;			//count �ʱ�ȭ
    
  }
  
  watch.msec++;//ms 1���� ����
  
  switch(((watch.msec%2)==0)&&(watch.msec>=2)){
    //ms�� 2�� �̻� ¦�� �ΰ�츸 ����ǰ� ����->1�ʰ� �ö󰡾� �ϴ� ��츦 Ư��
  case 1:
    
    watch.sec++;			//�ʴ����� 1 ����
    watch.msec=0;			//ms�� �ʱ�ȭ
    LCD_DisplayChar(1,10,0x30+(watch.sec%10)); //�ʴ����� 0~9������ ���
    
    if(watch.sec>=10){			//�ʴ����� 10�� �̻��� �Ȱ��
      
      watch.ten_sec++;  		//���ʴ��� 1����
      LCD_DisplayChar(1,9,0x30+(watch.ten_sec%6));//���ʴ����� 0~5������ ���
      
      watch.sec=0;      		//�� ���� �ʱ�ȭ
      
      if(watch.ten_sec>=6){ 		//���� ������ 6�̻��� �� ���
        watch.min++;
        LCD_DisplayChar(1,7,0x30+(watch.min%10));//�д����� 0~9������ ���
        
        watch.ten_sec=0; 		//���� ���� �ʱ�ȭ
        
        if(watch.min>=10){      	//�� ������ 10�̻��� �� ���
          
          watch.ten_min++;     	 	//�ʺ� ������ 1 ����
          LCD_DisplayChar(1,6,0x30+(watch.ten_min%6));//�ʺд����� 0~5������ ���
          
          watch.min=0;          	//�� ���� �ʱ�ȭ
          
          if(watch.ten_min>=6){ 	//�ʺ� ������ 6�̻��� �� ���
            watch.hour++;       	//�� ���� 1 ����
            
            LCD_DisplayChar(1,4,0x30+(watch.hour%10));//�ô����� 0~9������ ���
            
            watch.ten_min=0;    	//�ʺ� ���� �ʱ�ȭ
            
            if((watch.hour>=10) ||((watch.ten_hour>=1)&&(watch.hour>=2))){ //�� ������ 10�̻��̰ų� 12�� �̻��� �� ���
              watch.ten_hour++; 	//���� ���� 1 ����
              
              LCD_DisplayChar(1,3,0x30+(watch.ten_hour%2)); //���ô����� 0,1�� ���                    
              
              if((watch.ten_hour>=1)&&(watch.hour>=2)&&(pm_flag==1)){ 
                //���� ������ 1�̰� �� ������ 2 �̸鼭 PM �ΰ��
                LCD_DisplayChar(1, 3, 0x30);    //���� ������ 0 ���
                LCD_DisplayChar(1, 4, 0x30);    //�� ������ 0 ���
                LCD_DisplayText(1, 0, "AM");    //PM/AM ��ºο� AM ���
                
                pm_flag=0;                      //PM_flag=0�� �־� AM���� ǥ��
                watch.ten_hour=0;               //���� ���� 0���� �ʱ�ȭ
                watch.hour=0;                   //�� ���� 0���� �ʱ�ȭ
              }
              
              else if(((watch.ten_hour>=1)&&(watch.hour>=2)&&(pm_flag==0))||(pm_flag==2)){
                //���� ������ 1�̰� �� ������ 2 �̸鼭 AM �ΰ��
                
                pm_flag=2;                      //PM_flag=2�� �־� PM���� ǥ��(13�ø� PM 01�÷ιٲ���ϹǷ� 2�� ǥ��)
                LCD_DisplayText(1, 0, "PM");    //PM/AM ��ºο� PM ���
                if(pm_flag==2){ 
                  LCD_DisplayChar(1, 3, 0x30);    //���� ������ 0 ���
                  LCD_DisplayChar(1, 4, 0x31);    //�� ������ 1 ���
                  
                  watch.ten_hour=0;             //���� ���� 0���� �ʱ�ȭ
                  watch.hour=1;                 //�� ���� 1�� �ʱ�ȭ
                  pm_flag=1;                    //1�� �־� �ð��� ������ �ٽ� AM���� �ٲ�� �ֵ��� ��
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
    
    
    if(stop_watch.SW_flag==1){		//��ž��ġ ������
      stop_watch.msec++;		//CC 4 ���ͷ�Ʈ �߻��� ms 1 ����
      LCD_DisplayChar(2,11,0x30+(stop_watch.msec%10));	//������ ms�� 0~9������ ���
      
      if(stop_watch.msec>=10){		//ms�� 10�� �̻��ΰ��
        
        stop_watch.msec=0;		//ms�� 0���� �ʱ�ȭ
        stop_watch.sec++;		//�ʴ����� 1 ����
        LCD_DisplayChar(2,9,0x30+(stop_watch.sec%10));//������ �ʴ����� 0~9������ ���
        
        if(stop_watch.sec>=10){		//�ʴ����� 10 �̻��� �� ���
          	
          stop_watch.sec=0;		//0�ʷ� �ʱ�ȭ
          stop_watch.ten_sec++; 	//���� ������ 1����
          LCD_DisplayChar(2,8,0x30+(stop_watch.ten_sec%6)); //������ ���� ������ 0~5���������� ���
          
          if(stop_watch.ten_sec>=6){	//���� ������ 6�̻��� ���
            
            stop_watch.min++;		//�д����� 1 ����
            stop_watch.ten_sec=0;	//���� ������ 0���� �ʱ�ȭ
            LCD_DisplayChar(2,6,0x30+(stop_watch.min%10));//�д����� 0~9������ ���
            
            if(stop_watch.min>=10){	//�д����� 10 �̻��� �� ���
              
              stop_watch.ten_min++;	//�ʺд��� 1 ����
              stop_watch.min=0;		//�д��� 0���� �ʱ�ȭ
              LCD_DisplayChar(2,5,0x30+(stop_watch.ten_min%6));	//������ 10�� ������ 0~5������ ���
              
              if(stop_watch.ten_min>=6){//10�� ������ 6�̻��� ���
                
                stop_watch.hour++;	//�� ������ 1 ����
                stop_watch.ten_min=0;	//�ʺ� ������ 0���� �ʱ�ȭ
                LCD_DisplayChar(2,3,0x30+(stop_watch.hour));//������ �� ������ ���
                
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
  // LED (GPIO G) ����
  RCC->AHB1ENR	|=  0x00000040;		// RCC_AHB1ENR : GPIOG(bit#6) Enable							
  GPIOG->MODER 	|=  0x00005555;		// GPIOG 0~7 : Output mode (0b01)						
  GPIOG->OTYPER	&= ~0x000000FF;		// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
  GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
  
  // SW (GPIO H) ���� 
  RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
  GPIOH->MODER 	&= ~0xFFFF0000;		// GPIOH 8~15 : Input mode (reset state)				
  GPIOH->PUPDR 	&= ~0xFFFF0000;		// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state
  
  // Buzzer (GPIO F) ���� 
  RCC->AHB1ENR	|=  0x00000020;		// RCC_AHB1ENR : GPIOF(bit#5) Enable							
  GPIOF->MODER 	|=  0x00040000;		// GPIOF 9 : Output mode (0b01)						
  GPIOF->OTYPER 	&= ~0x00000200;	// GPIOF 9 : Push-pull  	
  GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
}

void Display_LCD(void){
  
  
  LCD_Clear(RGB_YELLOW);		// ȭ�� Ŭ����
  LCD_SetFont(&Gulim8);			// ��Ʈ : ���� 8
  LCD_SetBackColor(RGB_YELLOW);		// ���ڹ��� : Yellow
  LCD_SetTextColor(RGB_BLACK);		// ���ڻ� : Black
  LCD_DisplayText(0, 0, "Digital Clock");// Title
  
  
  LCD_DisplayText(1, 0, "PM");		//�ʱ� �ð��� AM/PM�� ���
  LCD_DisplayText(1, 3, "11:59:55:0");	//�ʱ� �ð��� �ð��� ���
  LCD_DisplayText(2, 0, "SW 0:00:00:0");//�ʱ� ��ž��ġ�� �ð��� ���
}


void EXTI15_10_IRQHandler(void){
  
  
  if (EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?
  {
    EXTI->PR |= 0x8000; 		// Pending bit Clear
    
    
    stop_watch.SW_flag = 2;         	//��ž��ġ �ʱ�ȭ��� ���԰�
    
    
  }
  else if (EXTI->PR & 0x4000) 		// EXTI14 Interrupt Pending?
  {
    EXTI->PR |= 0x4000; 		// Pending bit Clear
    BEEP();                         	//BEEP on
    stop_watch.SW_flag=1;           	//��ž��ġ �������/������ ���԰�
    stop_watch.flag++;              	//������� ������ �� 1���� ���۵ǰ� ����� ��
    
  }
  
}

void EXTI9_5_IRQHandler(void)		// EXTI 5~9 ���ͷ�Ʈ �ڵ鷯 �ҽ��� 5���ε� ���⼱ 2���� �����ؼ� ��.
{
  if(EXTI->PR & 0x0100) 		// EXTI8 Interrupt Pending?
  {
    EXTI->PR |= 0x0100; 		// Pending bit Clear
    watch.SW_flag = 1;			//�ð� �ʱ�ȭ��� ���԰�
  }
  else if(EXTI->PR & 0x0200) 		// EXTI9 Interrupt Pending?
  {
    EXTI->PR |= 0x0200; 		// Pending bit Clear �ַ� 0���� ����� �� �ܺ� ���ͷ�Ʈ�� 1�� ��� ������.
    watch.SW_flag = 2;			//�ð� ������� ���԰�
  }
}

void _EXTI_Init(void)
{
  RCC->AHB1ENR 	|= 0x0080;		// RCC_AHB1ENR GPIOH Enable
  RCC->APB2ENR 	|= 0x4000;		// Enable System Configuration Controller Clock
  
  GPIOH->MODER 	&= 0x0000FFFF;		// GPIOH PIN8~PIN15 Input mode (reset state)				 
  
  SYSCFG->EXTICR[2] |= 0x0077; 		// EXTI8,9�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR3) (reset value: 0x0000)	
  SYSCFG->EXTICR[3] |= (7<<12);		// EXTI15�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR3) (reset value: 0x0000)	
  SYSCFG->EXTICR[3] |= (7<<8);		// EXTI14�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR3) (reset value: 0x0000)	
  
  
  EXTI->RTSR |= 0x000300;		// Rising Trigger  Enable  (EXTI9,8:PH9,8) 
  EXTI->RTSR |= (3<<14);		// Rising Trigger  Enable  (EXTI14,15:PH14,15) 
  
  EXTI->IMR  |= 0x000300;		// EXTI8,9 ���ͷ�Ʈ mask (Interrupt Enable)
  EXTI->IMR |= (3<<14);			// EXTI14,15 ���ͷ�Ʈ mask (Interrupt Enable)
  
  NVIC->ISER[0] |= ( 1 << 23 );		// Enable Interrupt EXTI8,9 Vector table Position ����
  
  NVIC->ISER[1] |= ( 1 << 8 );		//Enable Interrupt EXTI15_10 Vector table Position ����
  
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

