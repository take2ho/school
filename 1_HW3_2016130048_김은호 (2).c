//************************************************

// �ۼ���: 2016130048 ����ȣ

// �ۼ���: 2019.11.01
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
void DisplayTitle(void);



void _ADC1_Init(void);
void _ADC3_Init(void);
void TIMER14_Init(void);



uint16_t KEY_Scan(void);

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);
void BEEP(void);

uint16_t ADC_Value, Voltage, ADC2_Value, Voltage2, Temperture,Vsense,Temperture2;
uint8_t str[20];
uint8_t	count=0 , sw_flag=0; //���ͷ�Ʈ ī��Ʈ,����ġ �ʱ�ȭ
void ADC_IRQHandler(void)
{
  
    
	if (ADC3->SR &= ~(1<<1))		// EOC flag clear

	{
          ADC_Value = ADC3->DR;		// Reading ADC result

       	 sprintf(str,"%4d",ADC_Value);	// ADC result : 12bits (0~4095)
       
	Voltage = ADC_Value * (3.3 * 100) / 4095;    // 3.3 : 4095 =  Volatge : ADC_Value  
                                                    // 100:  �Ҽ����Ʒ� ���ڸ����� ǥ���ϱ� ���� ��  
	LCD_DisplayChar(0,8,Voltage/100 + 0x30);
	LCD_DisplayChar(0,10,Voltage%100/10 + 0x30);
	LCD_DisplayChar(0,11,Voltage%10 + 0x30);
        
        
        Temperture = ((((Voltage) * (Voltage)) * 3.5) +1)/1000; // Tem = 3.5 V^2 + 1
                                                                         //   
                                                                         // 1000: voltage ���� �������� �Ҽ����Ʒ� ���ڸ����� ǥ���ϱ� ���Ѱ�
        if(Temperture<10)                                       // �µ� �ּ�1��, �ִ� 39.1�� 
        {
          Temperture = 10;
        }
        if(Temperture>391)
        {
          Temperture = 391;
        }
        
        LCD_DisplayChar(1,8,Temperture/100 + 0x30);
	LCD_DisplayChar(1,9,Temperture%100/10 + 0x30);
	LCD_DisplayChar(1,11,Temperture%10 + 0x30);   
        
        LCD_DisplayChar(2,5,Temperture/100 + 0x30);
	LCD_DisplayChar(2,6,Temperture%100/10 + 0x30);
	LCD_DisplayChar(2,7,Temperture%10 + 0x30); 
        LCD_DisplayChar(2,8,0 + 0x30); 
        
        
        LCD_SetBrushColor(RGB_YELLOW); // �簢�� reset
        LCD_DrawFillRect(10,52, 160, 23);       
        
        LCD_SetBrushColor(RGB_BLUE);
        LCD_DrawFillRect(10, 52, ((3.23*Temperture/10)),17); //frequency�� ����� �Ķ��� �簢���� ��ǥ, ũ��
        
        switch(KEY_Scan())
        {
          case SW7_PUSH  : 	//SW7�� ���������� �߻�
             
        
             count+=1; //���������� ī��Ʈ 1�� ����
        
             if ((count%2)==1)
                 {
          
                        GPIOG->ODR |= (1<<7);		// LED7 ON 
                        BEEP();                                    //Ȧ�������� beep
        }
	      else
        {
          
                        GPIOG->ODR &= ~(1<<7);		// LED7 OFF 
        }
            
            
            break;
            
        }

 	//Starts conversion of regular channels
	ADC3->CR2 |= ADC_CR2_SWSTART; 	// 0x40000000 (1<<30)
        
        }
        
        if (ADC1->SR &= ~(1<<1))	// EOC flag clear

	{
          Vsense = ADC1->DR;		// Reading Vsense
          
          sprintf(str,"%4d",Vsense);          // Vsense result : 12bits (0~4095)
          
         Temperture2 = (((((Vsense - 0.76) / 2,5) + 25))*10);  //*10�� �Ҽ��� ù°¥������ ���������� ��
          
          LCD_DisplayChar(5,8,(Temperture2)/100 + 0x30);
          LCD_DisplayChar(5,9,(Temperture2)%100/10 + 0x30);
          LCD_DisplayChar(5,11,(Temperture2)%10 + 0x30);
          
          
       

 	//Starts conversion of regular channels
        ADC1->CR2 |= ADC_CR2_SWSTART; 	// 0x40000000 (1<<30)
	}
}

int main(void)
{
	LCD_Init();	// LCD ���� �Լ�
	DelayMS(10);	// LCD���� ������
 	DisplayTitle();	//LCD �ʱ�ȭ�鱸�� �Լ�
        

        _GPIO_Init();
	
        
        _ADC1_Init();         //adc 1,3����
        _ADC3_Init();
        
        

        
        
        
        
 	
	while(1)
	{
          //Starts conversion of regular channels
       	ADC3->CR2 |= ADC_CR2_SWSTART; // 0x40000000 (1<<30) 
        ADC1->CR2 |= ADC_CR2_SWSTART; // 0x40000000 (1<<30) 
          
        
        
        
        
 	}
}


void _ADC1_Init(void)
{	// ADC1: PF5(pin 21)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	// (1<<0) ENABLE GPIOF CLK (stm32f4xx.h ����)
	GPIOF->MODER |= (3<<2*5);		// CONFIG GPIOA PIN5(PF5) TO ANALOG IN MODE
						
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;	// (1<<9) ENABLE ADC2 CLK (stm32f4xx.h ����)

	ADC->CCR &= ~(0X1F<<0);		// MULTI[4:0]: ADC_Mode_Independent  ADC->�� �������� ����
	ADC->CCR |=  (1<<16); 		// 0x00010000 ADCPRE:ADC_Prescaler_Div4 (ADC MAX Clock 36MHz, 84Mhz(APB2)/4 = 21MHz)
        ADC->CCR |=  (1<<23); 	        // TSVREFE bit =1 to wake up the temperature sensor
        
	ADC1->CR1 &= ~(3<<24);		// RES[1:0]=0b00 : 12bit Resolution
	ADC1->CR1 &= ~(1<<8);		// SCAN=0 : ADC_ScanCovMode Disable
	ADC1->CR1 |=  (1<<5);		// EOCIE=1: Interrupt enable for EOC

	ADC1->CR2 &= ~(1<<1);		// CONT=0: ADC_Continuous ConvMode Disable
	ADC1->CR2 &= ~(3<<28);		// EXTEN[1:0]=0b00: ADC_ExternalTrigConvEdge_None
	ADC1->CR2 &= ~(1<<11);		// ALIGN=0: ADC_DataAlign_Right
	ADC1->CR2 &= ~(1<<10);		// EOCS=0: The EOC bit is set at the end of each sequence of regular conversions

	ADC1->SQR1 &= ~(0xF<<20);	// L[3:0]=0b0000: ADC Regular channel sequece length 
					// 0b0000:1 conversion ä�� 1���� �ٲ�)
    
        ADC1->SMPR2 |= (0x7<<(3*0));	// ADC2_CH0 Sample TIme_480Cycles (3*Channel_1)
 	//Channel selection, The Conversion Sequence of PIN1(ADC2_CH1) is first, Config sequence Range is possible from 0 to 17
	// ADC2->SQR3 |= (1<<0);		// SQ1[4:0]=0b0001 : CH1 //
        ADC1->SQR3 &= (0xF<<0);  //CH0 ���
        

	NVIC->ISER[0] |= (1<<18);	// Enable ADC global Interrupt

	ADC1->CR2 |= 1<<0;		// ADON: ADC ON


}

void _ADC3_Init(void)
{	// ADC3: PA1(pin 41)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// (1<<0) ENABLE GPIOA CLK (stm32f4xx.h ����)
	GPIOA->MODER |= (3<<2*1);		// CONFIG GPIOA PIN1(PA1) TO ANALOG IN MODE
						
	RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;	// (1<<8) ENABLE ADC1 CLK (stm32f4xx.h ����)

	ADC->CCR &= ~(0X1F<<0);		// MULTI[4:0]: ADC_Mode_Independent
	ADC->CCR |=  (1<<16); 		// 0x00010000 ADCPRE:ADC_Prescaler_Div4 (ADC MAX Clock 36MHz, 84Mhz(APB2)/4 = 21MHz)
        
	ADC3->CR1 &= ~(3<<24);		// RES[1:0]=0b00 : 12bit Resolution
	ADC3->CR1 &= ~(1<<8);		// SCAN=0 : ADC_ScanCovMode Disable
	ADC3->CR1 |=  (1<<5);		// EOCIE=1: Interrupt enable for EOC

	ADC3->CR2 &= ~(1<<1);		// CONT=0: ADC_Continuous ConvMode Disable
	ADC3->CR2 &= ~(3<<28);		// EXTEN[1:0]=0b00: ADC_ExternalTrigConvEdge_None
	ADC3->CR2 &= ~(1<<11);		// ALIGN=0: ADC_DataAlign_Right
	ADC3->CR2 &= ~(1<<10);		// EOCS=0: The EOC bit is set at the end of each sequence of regular conversions

	ADC3->SQR1 &= ~(0xF<<20);	// L[3:0]=0b0000: ADC Regular channel sequece length 
					// 0b0000:1 conversion)
    
        ADC3->SMPR2 |= (0x7<<(3*1));	// ADC1_CH1 Sample TIme_480Cycles (3*Channel_1)
 	//Channel selection, The Conversion Sequence of PIN1(ADC1_CH1) is first, Config sequence Range is possible from 0 to 17
	ADC3->SQR3 |= (1<<0);		// SQ1[4:0]=0b0001 : CH1

	NVIC->ISER[0] |= (1<<18);	// Enable ADC global Interrupt

	ADC3->CR2 |= (1<<0);		// ADON: ADC ON

}


void _GPIO_Init(void)
{
	// LED (GPIO G) ����
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
    
	// SW (GPIO H) ���� 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) ���� 
    	RCC->AHB1ENR	|=  0x00000020; // RCC_AHB1ENR : GPIOF(bit#5) Enable							
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						
	GPIOF->OTYPER 	&= ~0x0200;	// GPIOF 9 : Push-pull  	
 	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 
	
	//NAVI.SW(PORT I) ����
	RCC->AHB1ENR 	|= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER 	= 0x00000000;	// GPIOI PIN8~PIN15 Input mode (reset state)
	GPIOI->PUPDR    = 0x00000000;	// GPIOI PIN8~PIN15 Floating input (No Pull-up, pull-down) (reset state)
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

void DisplayTitle(void)           //�ʱ� ȭ��
{
	LCD_Clear(RGB_YELLOW);
	LCD_SetFont(&Gulim10);		//��Ʈ 
	LCD_SetBackColor(RGB_YELLOW);	//���ڹ���
	LCD_SetTextColor(RGB_BLACK);	//���ڻ�
       	LCD_DisplayText(0,0,"EXT VOL:3.30V");
      	LCD_DisplayText(1,0,"EXT TEM:39.1C");
        LCD_DisplayText(2,0,"FREQ:3910Hz");
        LCD_DisplayText(5,0,"INT TEM:  . C");
        LCD_SetBrushColor(RGB_BLUE); //�簢���� �� Blue 
        

	LCD_SetBackColor(RGB_YELLOW);	//���ڹ���
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
