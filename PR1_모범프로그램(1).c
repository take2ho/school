#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);
void _EXTI_Init(void);

uint16_t KEY_Scan(void);
void BEEP(void);

void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

void SelectMode(void);          // ������ ��� ���� �Լ�


uint8_t	SW7_Flag = 0;         // SW7�Է� ���� ���� ���� ���� (�ʱⰪ�� 0���� ����)
uint16_t blue_pointy = 88, red_pointy = 88; // ����(�Ķ�)�� ������(����)�� ���¹� y��ǥ ���� ���� (�ʱⰪ�� 88�� ����)
uint16_t blue_flag = 1, red_flag = 1;           // ����(�Ķ�)�� ������(����)�� ���� ���� �����ϱ� ���� ���������� (�ʱⰪ�� 1������ ����)
// LCD�� �簢���� �׸� �� �簢���� ���̸� ����ϱ� ���� ���
uint16_t count = 0;             // �� ���� Ƚ���� Ȧ������ ¦������ �����ϴ� ���� (0�̸� �ʱ����, 1�̸� Ȧ��, 2�̸� ¦��)
uint16_t status = 0;            // ���� �����ϴ� ���������Ͱ� �������� ���������� ���¸� �����ϴ� ���� (0�̸� �Ķ���, 1�̸� ���������� ���ڻ� ����)

char status1 = 'L', status2 = 'S', start = '1', end = '1', left_floor = '1', right_floor = '1';    //  LCD ��� ���� ���� ����
// status1�� ���� ���������Ͱ� �������� ���������� ���� (�����̸� 'L', �������̸� 'R')
// status2�� ���� ���������Ͱ� � ������ �����ϰ� �ִ��� ���� (���������̸� 'S', �ö󰡰� ������ 'U', �������� ������ 'D')
// start�� ������ ����� ����, end�� ������ ��ǥ�� ����
// left_floor, right_floor�� ���ʰ� �������� ���� ���� �����ϱ� ���� ������ ���� (start, end�� ���ϱ� ���� ���)

char *mode = "FL";      // LCD ��� ���ڿ� ���� ����
// ���� ��带 ���� (������ ���� "FL", ���� ���� "EX", �ߴ� ���� "HL" ����,,, �ʱⰪ�� "FL"�� ����)

int main(void)
{
    _GPIO_Init(); // GPIO (LED & SW) �ʱ�ȭ
    _EXTI_Init();   // EXTI(�ܺ� ���ͷ�Ʈ) �ʱ�ȭ
    LCD_Init();	// LCD ��� �ʱ�ȭ
    DelayMS(100);

    GPIOG->ODR |= 0x0001;	// �ʱⰪ: LED0ON
    DisplayInitScreen();	// LCD �ʱ�ȭ��
 
   while(1)
    {
        if(SW7_Flag == 0)                       // SW7 �Է��� ���� ���¶��
              SelectMode();                     //  �����ø�� ���� �Լ� ȣ�� �� ���� �ݺ�
        
        if(SW7_Flag)    // SW7�� �ԷµǸ� ����
        {
              GPIOG->ODR &= ~0x0001;    // LED0 OFF
              GPIOG->ODR |= 0x0080;     // LED7 ON
              mode = "EX";      // MODE�� ������ "EX"�� ����
              if(abs(left_floor - start) > abs(right_floor - start))              // ������ ���������Ͱ� ���� ���������� ���� ������� �� ����� ��� ����
              {
                      status = 1;       // ���ڻ��� ���������� �����ϱ� ���� 1 ����
                      status1 = 'R';    // ���� ���������ʹ� ������'R' ���
                      if(right_floor < start)   // ������������ ���� ���� ��������� ���� ��� ����
                      {
                              status2 = 'U';    // ���������� ������ ��� 'U' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; right_floor < start; right_floor++)         // ������������ ���� ���� ��������� ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �ö󰡴� ����
                                      red_pointy -= 11;        // ���� ������������ ���¹� 1�� ��� (LCD�� ��ǥ�� ���� ������ ���ڰ� �پ���)
                                      red_flag++;               // ���� ���������� 1�� ���
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                              status2 = 'S';    // ���������� ������ ���� 'S' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                      }
                      
                      else if(right_floor > start)   // ������������ ���� ���� ��������� ���� ��� ����
                      {
                              status2 = 'D';    // ���������� ������ �ϰ� 'D' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; right_floor > start; right_floor--)         // ������������ ���� ���� ��������� ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �������� ����
                                      red_pointy += 11;        // ���� ������������ ���¹� 1�� �ϰ� (LCD�� ��ǥ�� �Ʒ��� ������ ���ڰ� �þ��)
                                      red_flag--;               // ���� ���������� 1�� �ϰ�
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                              status2 = 'S';    // ���������� ������ ���� 'S' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                      }
                      
                      if(right_floor < end)   // ������������ ���� ���� ��ǥ������ ���� ��� ����
                      {
                              status2 = 'U';    // ���������� ������ ��� 'U' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; right_floor < end; right_floor++)         // ������������ ���� ���� ��ǥ������ ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �ö󰡴� ����
                                      red_pointy -= 11;        // ���� ������������ ���¹� 1�� ��� (LCD�� ��ǥ�� ���� ������ ���ڰ� �پ���)
                                      red_flag++;               // ���� ���������� 1�� ���
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                      }
                      
                      else if(right_floor > end)     // ������������ ���� ���� ��ǥ������ ���� ��� ����
                      {
                              status2 = 'D';    // ���������� ������ �ϰ� 'D' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; right_floor > end; right_floor--)           // ������������ ���� ���� ��ǥ������ ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �������� ����
                                      red_pointy += 11;        // ���� ������������ ���¹� 1�� �ϰ� (LCD�� ��ǥ�� �Ʒ��� ������ ���ڰ� �þ��)
                                      red_flag--;               // ���� ���������� 1�� �ϰ�
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                      }
              }
              
              else                            // �� �� ��� ��, ���� ���������Ͱ� ������ ���������� ���� ������� �� �����ų� �� ������������ �Ÿ��� ���� ��� ����
              {
                      status = 0;       // ���ڻ��� �Ķ������� �����ϱ� ���� 0 ����
                      status1 = 'L'; // ���� ���������ʹ� ������'R' ���
                      if(left_floor < start)    // ������������ ���� ���� ��������� ���� ��� ����
                      {
                              status2 = 'U';    // ���������� ������ ��� 'U' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; left_floor < start; left_floor++)           // ������������ ���� ���� ��������� ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �ö󰡴� ����
                                      blue_pointy -= 11;        // ������ ������������ ���¹� 1�� ��� (LCD�� ��ǥ�� ���� ������ ���ڰ� �پ���)
                                      blue_flag++;               // ������ ���������� 1�� ���
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                              status2 = 'S';    // ���������� ������ ���� 'S' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                      }
                      
                      else if(left_floor > start)   // ������������ ���� ���� ��������� ���� ��� ����
                      {
                              status2 = 'D';    // ���������� ������ �ϰ� 'D' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; left_floor > start; left_floor--)         // ������������ ���� ���� ��������� ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �������� ����
                                      blue_pointy += 11;        // ������ ������������ ���¹� 1�� �ϰ� (LCD�� ��ǥ�� �Ʒ��� ������ ���ڰ� �þ��)
                                      blue_flag--;               // ������ ���������� 1�� �ϰ�
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                              status2 = 'S';    // ���������� ������ ���� 'S' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                      }
                      
                      if(left_floor < end)   // ������������ ���� ���� ��ǥ������ ���� ��� ����
                      {
                              status2 = 'U';    // ���������� ������ ��� 'U' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; left_floor < end; left_floor++)           // ������������ ���� ���� ��ǥ������ ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �ö󰡴� ����
                                      blue_pointy -= 11;        // ������ ������������ ���¹� 1�� ��� (LCD�� ��ǥ�� ���� ������ ���ڰ� �پ���)
                                      blue_flag++;               // ������ ���������� 1�� ���
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                      }
                      
                      else if(left_floor > end)   // ������������ ���� ���� ��ǥ������ ���� ��� ����
                      {
                              status2 = 'D';    // ���������� ������ �ϰ� 'D' ���
                              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                              DelayMS(500);     // 0.5�� ����
                              for(; left_floor > end; left_floor--)         // ������������ ���� ���� ��ǥ������ ���� ���� �ݺ�, �������� �ݺ� Ż��
                              {                                                         // ���� �������� ����
                                      blue_pointy += 11;        // ������ ������������ ���¹� 1�� �ϰ� (LCD�� ��ǥ�� �Ʒ��� ������ ���ڰ� �þ��)
                                      blue_flag--;               // ������ ���������� 1�� �ϰ�
                                      DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                                      DelayMS(500);     // 0.5�� ����
                              }
                      }
              }
              
              status2 = 'S';    // ���������� ������ ���� 'S' ���
              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
              DelayMS(500);     // 0.5�� ����
              for(int i = 0; i < 3; i++)        // ���� 3�� �︮�� ���� �ݺ���
              {
                      BEEP();
                      DelayMS(100);
              }
              
              mode = "FL";      // MODE�� ������ ��� "FL"�� ����
              DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
              GPIOG->ODR |= 0x0001;     // LED0 ON
              GPIOG->ODR &= ~0x00FE;    // LED0�� ������ ��� LED OFF
              SW7_Flag = 0;             // SW7 �Է� ���� �ʱ�ȭ (0���� ����)
        }
    }
}

void SelectMode(void)           // ������ ��� ���� �Լ�
{
        switch(KEY_Scan())
        {
        	case 0xFD00 : 	//SW1�� ������ break���� ����
                        GPIOG->ODR &= ~0x007E;  // LED0�� ������ ��� LED OFF
                        GPIOG->ODR |= 0x0002;   // ������ ���� LED ON (LED1 ON)
                        
                        if((count == 0) || (count == 2))        // �ʱ���� �̰ų�(count == 0) ¦����° �Է��� �����̸�(count == 2) ����
                        {
                            start = '1';        // ������� '1'����
                            count = 1;          // Ȧ����° �Է� ����
                        }
                        else    // �� �� ��� ��� (Ȧ�� ��° �Է��� ���(count == 1))
                        {
                            end = '1';          // ��ǥ���� '1'����
                            if(start == end)    // �̶� ���� ������� ��ǥ���� ���ٸ�
                                break;          // ����ġ�� ������ ����ġ ���Է�
                            count = 2;          // ¦����° �Է� ����
                        }
                        
                        DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                        BEEP();     // ���� 1�� �︲
                        
          	break;
                
        	case 0xFB00 : 	//SW2�� ������ break���� ����
                        GPIOG->ODR &= ~0x007E;   // LED0�� ������ ��� LED OFF
                        GPIOG->ODR |= 0x0004;   // ������ ���� LED ON (LED2 ON)
                        
                        if((count == 0) || (count == 2))        // �ʱ���� �̰ų�(count == 0) ¦����° �Է��� �����̸�(count == 2) ����
                        {
                            start = '2';        // ������� '2'����
                            count = 1;          // Ȧ����° �Է� ����
                        }
                        else
                        {
                            end = '2';          // ��ǥ���� '2'����
                            if(start == end)    // �̶� ���� ������� ��ǥ���� ���ٸ�
                                break;          // ����ġ�� ������ ����ġ ���Է�
                            count = 2;          // ¦����° �Է� ����
                        }
                        
                        DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                        BEEP();     // ���� 1�� �︲
                        
          	break;
                
        	case 0xF700 : 	//SW3�� ������ break���� ����
                        GPIOG->ODR &= ~0x007E;   // LED0�� ������ ��� LED OFF
                        GPIOG->ODR |= 0x08;   // ������ ���� LED ON (LED3 ON)
                        
                        if((count == 0) || (count == 2))        // �ʱ���� �̰ų�(count == 0) ¦����° �Է��� �����̸�(count == 2) ����
                        {
                            start = '3';        // ������� '3'����
                            count = 1;          // Ȧ����° �Է� ����
                        }
                        else
                        {
                            end = '3';          // ��ǥ���� '3'����
                            if(start == end)    // �̶� ���� ������� ��ǥ���� ���ٸ�
                                break;          // ����ġ�� ������ ����ġ ���Է�
                            count = 2;          // ¦����° �Է� ����
                        }
                        
                        DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                        BEEP();     // ���� 1�� �︲
                        
          	break;
                
        	case 0xEF00 : 	//SW4�� ������ break���� ����
                        GPIOG->ODR &= ~0x007E;   // LED0�� ������ ��� LED OFF
                        GPIOG->ODR |= 0x10;   // ������ ���� LED ON (LED4 ON)
                        
                        if((count == 0) || (count == 2))        // �ʱ���� �̰ų�(count == 0) ¦����° �Է��� �����̸�(count == 2) ����
                        {
                            start = '4';        // ������� '4'����
                            count = 1;          // Ȧ����° �Է� ����
                        }
                        else
                        {
                            end = '4';          // ��ǥ���� '4'����
                            if(start == end)    // �̶� ���� ������� ��ǥ���� ���ٸ�
                                break;          // ����ġ�� ������ ����ġ ���Է�
                            count = 2;          // ¦����° �Է� ����
                        }
                        
                        DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                        BEEP();     // ���� 1�� �︲
                        
          	break;
                
        	case 0xDF00 : 	//SW5�� ������ break���� ����
                        GPIOG->ODR &= ~0x007E;   // LED0�� ������ ��� LED OFF
                        GPIOG->ODR |= 0x20;   // ������ ���� LED ON (LED5 ON)
                        
                        if((count == 0) || (count == 2))        // �ʱ���� �̰ų�(count == 0) ¦����° �Է��� �����̸�(count == 2) ����
                        {
                            start = '5';        // ������� '5'����
                            count = 1;          // Ȧ����° �Է� ����
                        }
                        else
                        {
                            end = '5';          // ��ǥ���� '5'����
                            if(start == end)    // �̶� ���� ������� ��ǥ���� ���ٸ�
                                break;          // ����ġ�� ������ ����ġ ���Է�
                            count = 2;          // ¦����° �Է� ����
                        }
                        
                        DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                        BEEP();     // ���� 1�� �︲
                        
          	break;
                
        	case 0xBF00 : 	//SW6�� ������ break���� ����
                        GPIOG->ODR &= ~0x007E;          // LED0�� ������ ��� LED OFF   
                        GPIOG->ODR |= 0x40;   // ������ ���� LED ON (LED6 ON)
                        
                        if((count == 0) || (count == 2))        // �ʱ���� �̰ų�(count == 0) ¦����° �Է��� �����̸�(count == 2) ����
                        {
                            start = '6';        // ������� '6'����
                            count = 1;          // Ȧ����° �Է� ����
                        }
                        else
                        {
                            end = '6';          // ��ǥ���� '6'����
                            if(start == end)    // �̶� ���� ������� ��ǥ���� ���ٸ�
                                break;          // ����ġ�� ������ ����ġ ���Է�
                            count = 2;          // ¦����° �Է� ����
                        }
                        
                        DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
                        BEEP();     // ���� 1�� �︲
                        
          	break;
        }
}

void _GPIO_Init(void)
{
	// LED (GPIO G) ����
    	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						
	GPIOG->OTYPER	&= ~0x000000FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	
 	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
    
	// SW (GPIO H) ���� 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) ���� 
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
        
	SYSCFG->EXTICR[2] |= 0x0007; 	// EXTI8�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR3) (reset value: 0x0000)	
        SYSCFG->EXTICR[3] |= 0x7000; 	// EXTI15�� ���� �ҽ� �Է��� GPIOH�� ���� (EXTICR4) (reset value: 0x0000)
	
	EXTI->FTSR |= 0x00008100;		// Falling Trigger Enable  (EXTI8:PH8 || EXTI15:PH15)
    	EXTI->IMR  |= 0x00008100;  	// EXTI8,15 ���ͷ�Ʈ mask (Interrupt Enable)
		
	NVIC->ISER[0] |= ( 1 << 23  );   // Enable Interrupt EXTI8 Vector table Position ����
        NVIC->ISER[1] |= ( 1 << 8  );   // Enable Interrupt EXTI15 Vector table Position ����
}

void EXTI9_5_IRQHandler(void)		// EXTI 5~9 ���ͷ�Ʈ �ڵ鷯
{
    if(SW7_Flag)
    {
        if(EXTI->PR & 0x0100) 		// EXTI8 Interrupt Pending?
        {
            EXTI->PR |= 0x0100; 		// Pending bit Clear
            mode = "HL";      // MODE�� �ߴ� ��� "HL"�� ����
            DisplayInitScreen();      // ����� ������ LCD �ʱ�ȭ �� �����
            GPIOG->ODR &= ~0x0081;      // LED0, LED7 OFF, ������ LED�� ���� ����
            BEEP();
            
            for(int i = 0; i < 10; i++)          // 5�� ������ ���� �ݺ���, 0.5�ʰ������� ���� �︲
            {
                  DelayMS(500);     // 0.5�� ����
                  BEEP();       
            }
            
            DelayMS(500);     // 0.5�� ����
            mode = "EX";      // MODE�� ���� ��� "EX"�� ����
        }
    }
}

void EXTI15_10_IRQHandler(void)		// EXTI 10~15 ���ͷ�Ʈ �ڵ鷯
{
        if(EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?
        {
            EXTI->PR |= 0x8000; 		// Pending bit Clear            bit �ڸ��� 1�� �־�� clear �ȴ�.
            SW7_Flag = 1;	                // SW7�� ������ SW7_Flag�� 1���� (SW7�� ���ȴٴ� ���� ����)
            BEEP();     // ���� 1�� �︲
        }
}

int y_point = 2;
void DisplayInitScreen(void)    // LCD ȭ�� ��� �Լ�
{
    LCD_Clear(RGB_YELLOW);	// ȭ�� Ŭ���� (���� : Yellow)
    LCD_SetFont(&Gulim8);	// ��Ʈ : ���� 8
    LCD_SetBackColor(RGB_YELLOW);       // ���� : Yellow
    
    // ���ڿ� �Ǵ� ���� ��� ����
    LCD_SetTextColor(RGB_GRAY);// ���ڻ� : Gray
    LCD_DisplayText(0,0," Mecha-Elevator");  // 0���� Title : Mecha-Elevator

    //  ���� ���������� �� ���
    LCD_SetTextColor(RGB_BLUE);// ���ڻ� : Blue
    LCD_DisplayText(0,16, mode);
    y_point = 2;
    for(int i = 6; i >0; i--)
    {
          LCD_DisplayChar(y_point, 4, i + 0x30);
          y_point++;
    }
    /////////////////////////////////
    
    if(status == 0)     // status�� 0�̸� ����
          LCD_SetTextColor(RGB_BLUE);// ���ڻ� : Blue
    
    else        // �� ���� ��� ��, status�� 1�̸� ����
          LCD_SetTextColor(RGB_RED);// ���ڻ� : Red
  
    // ���� ���������� ��� ���
    // �ʱⰪ�� L:S
    LCD_DisplayChar(4, 8, status1);
    LCD_DisplayChar(4, 9, ':');
    LCD_DisplayChar(4, 10, status2);
    /////////////////////////////////
    
    // ������ ���������� �� ���
    LCD_SetTextColor(RGB_RED);// ���ڻ� : Red
    y_point = 2;
    for(int i = 6; i > 0; i--)
    {
          LCD_DisplayChar(y_point, 14, i + 0x30);
          y_point++;
    }
    /////////////////////////////////
    
    // ���� �Էµ� ������� ��ǥ�� ���
    // �ʱⰪ�� 1->1
    LCD_SetTextColor(RGB_BLACK);// ���ڻ� : Black
    LCD_DisplayChar(5, 8, start);
    LCD_DisplayText(5, 9, "->");
    LCD_DisplayChar(5, 11, end);
    
    // ���������� ���¹� ���� �� ä��� ����
    // ����
    LCD_SetBrushColor(RGB_BLUE);
    LCD_DrawFillRect(15, blue_pointy, 10, 11 * blue_flag);
    
    // ������
    LCD_SetBrushColor(RGB_RED);
    LCD_DrawFillRect(125, red_pointy, 10, 11 * red_flag);
    ////////////////////////////////////////////////////////////
    
    // ���������� ���¹� �׵θ� ���
    LCD_SetPenColor(RGB_GREEN);
    LCD_DrawRectangle(15, blue_pointy, 10, 11 * blue_flag);     // ����
    LCD_DrawRectangle(125, red_pointy, 10, 11 * red_flag);      // ������
    ////////////////////////////////////////////////////////////////
    
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