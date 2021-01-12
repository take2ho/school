#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);   //GPIO �ʱ⼳�� �Լ�
void _EXTI_Init(void);   //���ͷ�Ʈ �ʱ⼳�� �Լ�

uint16_t KEY_Scan(void);
void DisplayInitScreen(void);  //�ʱ�ȭ�� ���� �Լ�
void BEEP(void);  //������ �︮�� �Լ�
void BEEP3(void); // ������ 3�� �︮�� �Լ�
void Floor(int floor);  // �� ������ ���� �Լ�
void L_U(void);    //LCD�� L:U  ǥ�� �Լ�
void L_D(void);    //LCD�� L:D ǥ�� �Լ�
void L_S(void);    //LCD�� L:S  ǥ�� �Լ�
void R_U(void);   //LCD�� R:U  ǥ�� �Լ�
void R_D(void);   //LCD�� R:D  ǥ�� �Լ�
void R_S(void);   //LCD�� R:S  ǥ�� �Լ�
void blue_clear(void); // blue elevator bar �׸� clear �Լ�
void red_clear(void);  // red elevator bar �׸� clear �Լ�
void to_ride_up_blue(void);  //blue elevator�� ����� �¿�� ���� �ö󰡴� �Լ�
void to_ride_down_blue(void);  //blue elevator�� ����� �¿�� ���� �������� �Լ�
void after_ride_up_blue(void);   //blue elevator�� ����� �¿� �� ��ǥ������ �ö󰡴� �Լ�
void after_ride_down_blue(void);  //blue elevator�� ����� �¿� �� ��ǥ������ �������� �Լ�
void to_ride_up_red(void);     //red elevator�� ����� �¿�� ���� �ö󰡴� �Լ�
void to_ride_down_red(void);  //red elevator�� ����� �¿�� ���� �������� �Լ�
void after_ride_up_red(void);  //red elevator�� ����� �¿� �� ��ǥ������ �ö󰡴� �Լ�
void after_ride_down_red(void);  //red elevator�� ����� �¿� �� ��ǥ������ �������� �Լ�

void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

uint8_t  SW7_Flag;   //sw7�� ���ͷ�Ʈ�� ����ϱ� ���� ����

int current_floor, goal_floor;  //������� ��ǥ���� ������ ����
int elevator_blue=1, elevator_red=1; // ���������Ͱ� �ִ� ���� ������ ����
int separate=0;  // ����� �Է°� ��ǥ�� �Է��� �����ϱ� ���� ����
int mode=0; // ���  ��ȯ�� ���� ����
int y_blue, height_blue; // LCD�� �׸��� �׸��� ���� �������� y���� ���� ���� (blue elevator)
int y_red, height_red; // LCD�� �׸��� �׸��� ���� �������� y���� ���� ���� (red elevator)

int main(void)
{
    DelayMS(100);
    _GPIO_Init();
    LCD_Init();
    _EXTI_Init();

    GPIOG->ODR = 0xFFFFFF01;	// �ʱⰪ: LED0 On, LED1~7 Off 
    
    DisplayInitScreen();   //LCD �ʱ�ȭ�� 
  
    while(1)
    {
	if(SW7_Flag)   // SW7�� ������ �� EXTI  -> ���� ���
	{	  
		GPIOG->ODR |= 0x0080;  //SW7 LED ON
                GPIOG->ODR &= ~0x0001; //SW0 LED OFF
                BEEP();    // SW�� �����Ϳ� ���� ������
		SW7_Flag = 0;  // ���� ���ͷ�Ʈ�� �߻��� �� �ְ� �ϱ����� SW7_Flag�� �ٽ� 0���� �ʱ�ȭ
                mode=3;  // �������߿��� �ߴܸ��� ��ȯ�� �� �ְ� �ϱ� ���� mode�� 3���� ����
                
                LCD_SetTextColor(RGB_BLUE);  //LCD�� �����带 ǥ���ϱ� ���� �Ķ������� FL->EX
                LCD_DisplayText(1,15," EX");
                
                if(abs(current_floor-elevator_blue)>abs(current_floor-elevator_red))  // red elevator�� ����� ��
                {
                         if(current_floor > elevator_red)  //������� red elevator�� �ִ� ������ ���Ƽ� ����� �¿�� ���� �ö�.
                         {
                               R_U();  // LCD�� ������ elevator up ǥ�� �Լ�
                               to_ride_up_red();  //red elevator�� ����� �¿�� ���� �ö󰡴� �Լ�
                               R_S();  // ������� �����Ͽ� ����� �¿�� ���� ��� ����. R:U -> R:S
                               
                               if( current_floor > goal_floor)  //����� �¿� �� ������� ��ǥ������ ���� ���� ��
                               {
                                    R_D();   // R:S -> R:D
                                    after_ride_down_red(); //����� �¿� �� red elevator�� �������� �Լ�
                               }
                               else if(current_floor < goal_floor)  // ����� �¿� �� ��ǥ���� ���������� ���� ���� ��
                               {
                                    R_U();  //R:S -> R:U
                                    after_ride_up_red(); //����� �¿� �� red elevator�� �ö󰡴� �Լ� 
                               }               
                         }
                          else if(current_floor < elevator_red)  //������� red elevator�� �ִ� ������ ���Ƽ� ����� �¿�� ���� ������.
                         {
                               R_D();  // LCD�� ������ elevator down ǥ�� �Լ�
                               to_ride_down_red(); //red elevator�� ����� �¿�� ���� �������� �Լ�
                               R_S();  // ������� �����Ͽ� ����� �¿�� ���� ��� ����. R:U -> R:S
                               
                               if( current_floor > goal_floor)  // �� if���� �ּ� ����
                               {
                                    R_D();
                                    after_ride_down_red();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    R_U();
                                    after_ride_up_red(); 
                               }                      
                         }
                         
                         else if(current_floor == elevator_red) //������� red elevator�� �����ִ� ���� ���� �� 
                        {
                                if( current_floor > goal_floor)  //�� if���� �ּ� ����
                               {
                                    R_D();
                                    after_ride_down_red();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    R_U();
                                    after_ride_up_red(); 
                               }                      
                        }
                        
                        R_S();  // elevator�� ��ǥ���� �����Ͽ� ����. R:S 
                        elevator_red = goal_floor;  // ���� ������ ���� red elevator�� ��ǥ���� ��ġ�� ������.      
                  }   
      
                else if(abs(current_floor-elevator_blue)<=abs(current_floor-elevator_red)) //�Ÿ��� ���ų� blue elevator�� ����� ��
                {
                          if(current_floor > elevator_blue)  //������� blue elevator�� �ִ� ������ ���Ƽ� ����� �¿�� ���� �ö�.
                         {
                               L_U();  // LCD�� ���� elevator up ǥ�� �Լ�
                               to_ride_up_blue();  //blue elevator�� ����� �¿�� ���� �ö󰡴� �Լ�
                               L_S();  // ������� �����Ͽ� ����� �¿�� ���� ��� ����. L:U -> L:S
                               
                               if( current_floor > goal_floor)   //����� �¿� �� ������� ��ǥ������ ���� ���� ��
                               {
                                    L_D();  // L:S -> L:D
                                    after_ride_down_blue();  //����� �¿� �� blue elevator�� �������� �Լ�
                               }
                               else if(current_floor < goal_floor)  // ����� �¿� �� ��ǥ���� ���������� ���� ���� ��
                               {
                                    L_U();  // L:S -> L:U
                                    after_ride_up_blue();   //����� �¿� �� blue elevator�� �ö󰡴� �Լ�
                               }               
                         }
                                                  
                         else if(current_floor < elevator_blue)   //������� blue elevator�� �ִ� ������ ���Ƽ� ����� �¿�� ���� ������.
                         {
                               L_D();  // LCD�� ���� elevator down ǥ�� �Լ�
                               to_ride_down_blue(); //blue elevator�� ����� �¿�� ���� �������� �Լ�
                               L_S();   // ������� �����Ͽ� ����� �¿�� ���� ��� ����. L:D -> L:S
                               
                               if( current_floor > goal_floor)  //�� if���� �ּ� ����
                               {
                                    L_D();
                                    after_ride_down_blue();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    L_U();
                                    after_ride_up_blue(); 
                               }                      
                         }
                         
                         else if(current_floor == elevator_blue) //������� blue elevator�� �����ִ� ���� ���� �� 
                        {
                               if( current_floor > goal_floor)  //�� if���� �ּ� ����
                               {
                                    L_D();
                                    after_ride_down_blue();
                               }
                               else if(current_floor < goal_floor)
                               {
                                    L_U();
                                    after_ride_up_blue(); 
                               }                           
                        }
                       
                        L_S(); // elevator�� ��ǥ���� �����Ͽ� ����. L:S
                        elevator_blue = goal_floor;   // ���� ������ ���� blue elevator�� ��ǥ���� ��ġ�� ������.
                  }
                DelayMS(500);  //0.5�� 
                LCD_SetTextColor(RGB_BLUE);  //�����尡 ����ǰ� �����ø��� �ٲ�. (�Ķ������� EX->FL)
                LCD_DisplayText(1,15," FL");
                BEEP3();  //��ǥ���� ���������� �˸��� ������3��
                GPIOG->ODR &= ~0x0080; //SW7 LED OFF
                GPIOG->ODR |= 0x0001;    //SW0 LED ON
                mode=0;  //���� ������ ���� ������ �ʱ�ȭ ���ش�.
        }

        switch(KEY_Scan())  //���ͷ�Ʈ�� �ƴ� sw1~sw6�� KEY_Scan()�Լ���
        {
        case 0xFD00 : Floor(1); break;  //sw1�� ������ �� Floor�Լ��� 1�� ������.
          
        case 0xFB00 : Floor(2); break;  //sw2�� ������ �� Floor�Լ��� 2�� ������.
          
        case 0xF700 :  Floor(3); break; //sw3�� ������ �� Floor�Լ��� 3�� ������.
          
        case 0xEF00 : Floor(4);  break; //sw4�� ������ �� Floor�Լ��� 4�� ������.
          
        case 0xDF00 : Floor(5);  break; //sw5�� ������ �� Floor�Լ��� 5�� ������.
          
        case 0xBF00 : Floor(6);  break; //sw6�� ������ �� Floor�Լ��� 6�� ������.
        }
                
    }
}

void to_ride_up_blue(void)  //blue elevator�� ����� �¿�� ���� �ö󰡴� �Լ�
{
      for(int i=elevator_blue, length=14; i<current_floor;i++)   //LCDȭ�鿡 bar�׸��� ���̿� y���� length��ŭ �÷ȴ� ����.
      { 
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue-length,10,height_blue+length);  //x���� ���� ���� -> y���� ���̰��� ������ ����
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue-length,10,height_blue+length);
             DelayMS(500);  //0.5�� �������� bar�׸��� �پ��� �þ.
                          
             length+=14;  //length�� ���� 14��ŭ ������Ŵ.
       }
        y_blue -= 14*(current_floor - elevator_blue); //bar�� �̵��� ������ bar�� �ٲ� y��ǥ���� y_blue������ ����.
        height_blue += 14*(current_floor - elevator_blue); //bar�� �̵��� ������ bar�� �ٲ� ���̰��� height_blue������ ����.
}

void to_ride_down_blue(void)   //blue elevator�� ����� �¿�� ���� �������� �Լ� 
{
       for(int i=elevator_blue, length=14; i>current_floor;i--)      //�ּ� �� �Լ��� ����
      { 
             blue_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue+length,10,height_blue-length);
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue+length,10,height_blue-length);
             DelayMS(500);
                          
             length+=14;
       }
        y_blue += 14*(elevator_blue-current_floor);
        height_blue -= 14*(elevator_blue-current_floor);
}

void after_ride_up_blue(void)   //blue elevator�� ����� �¿� �� �ö󰡴� �Լ�
{
       for(int i =current_floor,length=14; i<goal_floor;i++)      //�ּ� �� �Լ��� ����
      {
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue-length,10,height_blue+length);
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue-length,10,height_blue+length);
             DelayMS(500);
                          
              length+=14;
       }
       y_blue -= 14*(goal_floor-current_floor);
       height_blue += 14*(goal_floor - current_floor);
}
  
void after_ride_down_blue(void)   //blue elevator�� ����� �¿� �� �������� �Լ�
{
      for(int i=current_floor,length=14;  i>goal_floor;i--)     //�ּ� �� �Լ��� ����
      { 
             blue_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(12,y_blue+length,10,height_blue-length);
             LCD_SetBrushColor(RGB_BLUE);
             LCD_DrawFillRect(12,y_blue+length,10,height_blue-length);
             DelayMS(500);
                                          
              length+=14;
         }
          y_blue += 14*(current_floor-goal_floor);
          height_blue -= 14*(current_floor-goal_floor);
 }

  //***************************************************************************************

 void to_ride_up_red(void)  //red elevator�� ����� �¿�� ���� �ö󰡴� �Լ�
 {
       for(int i=elevator_red, length=14; i<current_floor;i++)   //LCDȭ�鿡 bar�׸��� ���̿� y���� length��ŭ �÷ȴ� ����.
      { 
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red-length,10,height_red+length);   //x���� ���� ���� -> y���� ���̰��� ������ ����
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red-length,10,height_red+length);
             DelayMS(500);  //0.5�� �������� bar�׸��� �پ��� �þ.
                          
             length+=14;   //a�� ���� 14��ŭ ������Ŵ.
       }
        y_red -= 14*(current_floor - elevator_red); //bar�� �̵��� ������ bar�� �ٲ� y��ǥ���� y_red������ ����.
        height_red += 14*(current_floor - elevator_red);  //bar�� �̵��� ������ bar�� �ٲ� ���̰��� height_red������ ����.
}

void to_ride_down_red(void)  //red elevator�� ����� �¿�� ���� �������� �Լ�
{     
      for(int i=elevator_red, length=14; i>current_floor;i--)      //�ּ� �� �Լ��� ����
      {     
             red_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red+length,10,height_red-length);
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red+length,10,height_red-length);
             DelayMS(500);
                          
             length+=14;
       }
        y_red += 14*(elevator_red-current_floor);
        height_red -= 14*(elevator_red-current_floor);
}
                
void after_ride_up_red(void)  //red elevator�� ����� �¿� �� �ö󰡴� �Լ�
{
       for(int i =current_floor,length=14; i<goal_floor;i++)          //�ּ� �� �Լ��� ����
      {
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red-length,10,height_red+length);
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red-length,10,height_red+length);
             DelayMS(500);
                          
             length+=14;
       }
       y_red -= 14*(goal_floor-current_floor);
       height_red += 14*(goal_floor - current_floor);
}
                
void after_ride_down_red(void)  //red elevator�� ����� �¿� �� �������� �Լ�
{ 
      for(int i=current_floor,length=14;  i>goal_floor;i--)     //�ּ� �� �Լ��� ����
      { 
             red_clear();
             LCD_SetPenColor(RGB_GREEN);
             LCD_DrawRectangle(123,y_red+length,10,height_red-length);
             LCD_SetBrushColor(RGB_RED);
             LCD_DrawFillRect(123,y_red+length,10,height_red-length);
             DelayMS(500);
                                          
              length+=14;
         }
          y_red += 14*(current_floor-goal_floor);
          height_red -= 14*(current_floor-goal_floor);
 }

//****************************************************************************************
  
void L_U(void)  //���� elevator�� �ö󰥶� LCD�� "L:U" �� ǥ���ϱ� ���� �Լ�
{
  LCD_SetTextColor(RGB_BLUE);
  LCD_DisplayText(4,6," L");
  LCD_DisplayText(4,8," :");
  LCD_DisplayText(4,10," U");
   DelayMS(500);
}

void L_D(void)  //���� elevator�� �������� LCD�� "L:D" �� ǥ���ϱ� ���� �Լ�
{
   LCD_SetTextColor(RGB_BLUE);
   LCD_DisplayText(4,6," L");
   LCD_DisplayText(4,8," :");
   LCD_DisplayText(4,10," D");
   DelayMS(500);
}

void L_S(void)  //���� elevator�� ���⶧ LCD�� "L:S" �� ǥ���ϱ� ���� �Լ�
{
   LCD_SetTextColor(RGB_BLUE);
   LCD_DisplayText(4,6," L");
   LCD_DisplayText(4,8," :");
   LCD_DisplayText(4,10," S");
    DelayMS(500);
}

void R_U(void) //������ elevator�� �ö󰥶� LCD�� "R:U" �� ǥ���ϱ� ���� �Լ�
{  
    LCD_SetTextColor(RGB_RED);
    LCD_DisplayText(4,6," R");
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," U");
    DelayMS(500);
}

void R_D(void)  //������ elevator�� �������� LCD�� "R:D" �� ǥ���ϱ� ���� �Լ�
{  
    LCD_SetTextColor(RGB_RED);
    LCD_DisplayText(4,6," R");
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," D");
    DelayMS(500);
}

void R_S(void)  //������ elevator�� ���⶧ LCD�� "R:S" �� ǥ���ϱ� ���� �Լ�
{
    LCD_SetTextColor(RGB_RED);
    LCD_DisplayText(4,6," R");
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," S");
    DelayMS(500);
}

//*****************************************************************************************
  
void blue_clear(void)  //lblue elevator�� �������� bar�� �׸��� clear�ϱ� ���� �Լ�
{
    LCD_SetBrushColor(RGB_YELLOW);
    LCD_DrawFillRect(12,25,12,130);
}

void red_clear(void)   //red elevator�� �������� bar�� �׸��� clear�ϱ� ���� �Լ�
{
     LCD_SetBrushColor(RGB_YELLOW);
     LCD_DrawFillRect(123,25,12,130);
}
 
//****************************************************************************************

void Floor(int floor)  //������ �Լ�
{         
          BEEP(); //sw�� �����Ϳ� ���� ������
          
          if(separate == 0)   //separate������ 0�̸� ������� floor���� ����
          {
            current_floor = floor; 
            separate+=1; // ���� �Է��� ��ǥ������ �����ϱ� ���� separate���� 1������Ŵ.
            LCD_SetTextColor(RGB_BLACK);  //���������� LCD�� ����� ǥ��
            LCD_DisplayChar(5,7,floor+0x30); 
            GPIOG->ODR &= ~0x007E;  //������� ��ǥ���� �ٽ� �Է¹��� �� ���� ������ LED�� ��� ��,
            mode = 1;  //��� ���c�� ���� ���� mode�� 1�� ����
          }
          
          else   //seperate������ 0�� �ƴϸ� ��ǥ���� floor���� ����
          {
            goal_floor = floor; 
            GPIOG->ODR &= ~0x007E;  //����� led off
            
            if(current_floor != goal_floor) //������� ��ǥ���� �ٸ����� �����Է����� �ٽ� ������� �Է¹����� �ְ� LCD�� ��ǥ�� ǥ�ð���.
            {
                 separate-=1;  // ���� �Է��� ��������� �����ϱ� ���� separate���� 1���ҽ�Ŵ.
                 LCD_SetTextColor(RGB_BLACK);  //���������� LCD�� ��ǥ�� ǥ��
                 LCD_DisplayChar(5,11,floor+0x30);
                 mode = 2;  //��� ���c�� ���� ���� mode�� 2�� ����
            }  
          }       
   
          if(floor==1)
          {
              GPIOG->ODR |= 0x0002; //LED1 ON
          }
          else if(floor==2)
          {
              GPIOG->ODR |= 0x0004; // LED2 ON
          }
          else if(floor==3)
          {
              GPIOG->ODR |= 0x0008; // LED3 ON
          }
          else if(floor==4)
          {
              GPIOG->ODR |= 0x0010; // LED4 ON
          }
          else if(floor==5)
          {
              GPIOG->ODR |= 0x0020; // LED5 ON
          }
          else if(floor==6)
          {
              GPIOG->ODR |= 0x0040; // LED6 ON
          }
}

void DisplayInitScreen(void)  //LCD�ʱ�ȭ�� ���� �Լ�
{
    LCD_Clear(RGB_YELLOW);	// ȭ�� Ŭ����
    LCD_SetFont(&Gulim8);	// ��Ʈ : ���� 8

    LCD_SetBackColor(RGB_YELLOW);  //����� ��濡 ������ �۾��� "Mecha_Elevator"ǥ��.
    LCD_SetTextColor(RGB_BLACK);
    LCD_DisplayText(1,0," Mecha-Elevator");
    
    LCD_SetTextColor(RGB_BLUE);  //�Ķ��� �۾��� "FL" ǥ��.
    LCD_DisplayText(1,15," FL");
    
    LCD_DisplayText(2,3," 6");  //�Ķ��� �۾��� y���� �ٲ㰡�� ���� elevator �� ǥ��.
    LCD_DisplayText(3,3," 5");
    LCD_DisplayText(4,3," 4");
    LCD_DisplayText(5,3," 3");
    LCD_DisplayText(6,3," 2");
    LCD_DisplayText(7,3," 1");
    
    LCD_DisplayText(4,6," L");  //�Ķ��� �۾��� "L:S" ǥ��.
    LCD_DisplayText(4,8," :");
    LCD_DisplayText(4,10," S");
    
    LCD_SetTextColor(RGB_BLACK);  //������ �۾��� "1>1" ǥ��.
    LCD_DisplayChar(5,7,1+0x30);
    LCD_DisplayText(5,8," >");
    LCD_DisplayChar(5,11,1+0x30);
    
    
    LCD_SetTextColor(RGB_RED);  //������ �۾��� y���� �ٲ㰡�� ������ elevator �� ǥ��.
    LCD_DisplayText(2,13," 6");
    LCD_DisplayText(3,13," 5");
    LCD_DisplayText(4,13," 4");
    LCD_DisplayText(5,13," 3");
    LCD_DisplayText(6,13," 2");
    LCD_DisplayText(7,13," 1");
    
    LCD_SetPenColor(RGB_GREEN);  //�ʷϻ������� ���� elevator�� ������ elevator bar�׸��� �׵θ��� �׸�
    LCD_DrawRectangle(12,95,10,7);
    LCD_DrawRectangle(123,95,10,7);
    
    LCD_SetBrushColor(RGB_BLUE);  //�Ķ����귯���� ���� elevator bar�׸��� �簢�� ���θ� ä��.
    LCD_DrawFillRect(12,95,10,7);
    
    LCD_SetBrushColor(RGB_RED);  //�������귯���� ������ elevator bar�׸��� �簢�� ���θ� ä��.
    LCD_DrawFillRect(123,95,10,7);
    
    y_blue = 95;  //�ʱ� ���� elevator y���� 95�� ����.
    y_red = 95;   //�ʱ� ������ elevator y���� 95�� ����.
    height_blue = 7;   //�ʱ� ���� elevator ���̸� 7�� ����.
    height_red = 7;    //�ʱ� ������ elevator ���̸� 7�� ����.
}

void _GPIO_Init(void)  //GPIO�� �ʱ� ����
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

void _EXTI_Init(void)  //SW0�� SW7�� ���� ���ͷ�Ʈ ����
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable H->Switch
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock		 
	
	SYSCFG->EXTICR[2] |=  0x00000007;// EXTI8�� ���� �ҽ� �Է��� GPIOH�� ���� 
        SYSCFG->EXTICR[3] |= 0x00007000; // EXIT15�� ���� �ҽ� �Է��� GPIOH�� ����	
	
	EXTI->FTSR |= 0x00008100;		// Falling Trigger Enable  (EXTI8:PH8)(EXTI15:PH15)
    	EXTI->IMR  |= 0x00008100;  	// EXTI8,15 ���ͷ�Ʈ mask (Interrupt Enable)
		
	NVIC->ISER[0] |= ( 1<<23  );   // Enable Interrupt EXTI8 Vector table Position ����
        NVIC->ISER[1] |= ( 1<<8  );    // Enable Interrupt EXTI15 Vector table Position ����
}

void EXTI9_5_IRQHandler(void)	// SW0�� ���� EXTI 5~9 ���ͷ�Ʈ �ڵ鷯
{
    if(EXTI->PR & 0x0100)	//SW0�� ���Ȱ� mode�� 2�̸� �ߴܸ�� ����.
    {
        EXTI->PR |= 0x0100; 		// Pending bit Clear
        if(mode==3)
       {
            GPIOG->ODR &= ~0x0081; //�ߴܸ�忡�� LED0,7 OFF
            BEEP();  //SW�� �����Ϳ� ���� ������
                
            LCD_SetTextColor(RGB_BLUE);  //�Ķ��� �۾��� LCD�� EX->HL �ߴܸ�� ǥ��.
            LCD_DisplayText(1,15," HL");
            GPIOG->ODR &= ~0x0081;  //SW0�� ������ SW1~SW6 LED�� �״��, SW0�� SW7 LED�� OFF
                
            for(int i=0; i<=9;i++)  //�ߴܸ�尡 ���õǸ� 0.5�� �������� 5�ʰ� ������ �߻�.
           {
              BEEP();
              DelayMS(500);
           }
           GPIOG->ODR &= ~0x0001; //led0 off
           GPIOG->ODR |= 0x0080;  // led7 on
       } 
    }
}

void EXTI15_10_IRQHandler(void)  //SW7�� ���� EXTI 15~10 ���ͷ�Ʈ �ڵ鷯
{
     if(EXTI->PR & 0x8000)    //SW7�� ��������
    {
        EXTI ->PR |= 0x8000;  // Pending bit Clear
        if(mode==2)  //��ǥ������ �� �Է��� ���� �Ŀ��� ������� ��ȯ ����
        {
            SW7_Flag = 1;	  //main�������� sw7�� ���� if���� �����Ű�� ���� 1�� ����
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

void BEEP(void)			/* beep for 30 ms */
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void BEEP3(void)  //������ 3�� �︮�� �Լ�
{
   BEEP();
   DelayMS(50);
   BEEP();
   DelayMS(50);
   BEEP();
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
