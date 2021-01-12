#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);
uint16_t KEY_Scan(void);
void _EXTI_Init(void);

void BEEP(void);
void DisplayInitScreen(void);
void DelayMS(unsigned short wMS);
void DelayUS(unsigned short wUS);

uint8_t	SW0_Flag, SW7_Flag;
int Mode,Count,  Start, Goal, Start2, Goal2, Blue=1, Red=1; //모드, 출발층도착층 구분 변수, 시작층(정수),시작층(아스키코드)  목표층(정수),목표층(아스키코드), 파랑층, 빨강층 변수선언
uint16_t  i; //for문 변수를 위한 변수 i 선언

int main(void)
{
    _GPIO_Init(); // GPIO (LED & SW) 초기화
    LCD_Init();	// LCD 모듈 초기화
    _EXTI_Init();  
    DelayMS(100);
  BEEP();
     
    Mode = 0;                                                                                                               
    Count = 2;
    Start = 0x31;
    Goal = 0x31;
    Start2 = 1;
    Goal2 = 1;
     //수치 초기화
   
    
    GPIOG->ODR = 0xFF00;	// LED 초기값: LED0~7 Off
    DisplayInitScreen();	// LCD 초기화면

  
    while(1)
    {
      if (SW7_Flag ==1) //7번 버튼을 눌렀을때
      {
        LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(0,15,"EX");  //실행 모드 표시
    DelayMS(500);
    
      if ( (abs(Start2-Blue)) <=   (abs(Start2-Red)) ) //파란색이 빨간색보다 더 가깝거나 같을때
      {
        if (Blue == Start2) //파랑색 현재층과 출발층이 같을때
        {
          
          
          
          if (Start > Goal) //출발>목표 일때
          {
            for (i=Start2; i >Goal2; i--)  //출발점->목표점 내림차순
            {
               LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D");
    
    LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
     Blue= Blue-1; //층이 내려갈때마다 파랑색 하나씩 -1
    
     DelayMS(500);
    
    
            }
            
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop 
     DelayMS(500);
          }
          else if (Start == Goal) //출발=목표 일때
          {
          }
          else // 출발 <목표일때
          {
            for (i=Start2; i< Goal2; i++) //출발점->목표점 오름차순
            {
              LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
              
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U");
    
    LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
    Blue= Blue+1; //층이 오를때마다 파랑색 +1
    
     DelayMS(500);
    
    
            }
            
          }
         
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
        }
        else if (Blue > Start2) //파랑색 현재층이 출발층보다 클때
        {
          for (i=Blue; i>Start2; i--) //파란색-> 출발점 내림차순
          {
            LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
            
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D"); //내려감
    
    LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
    Blue= Blue-1; //내려갈때마다 파란색 하나씩 -1
    
     DelayMS(500);
            
          }
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
          
          
           if (Start > Goal) //출발>목표 일때
          {
            for (i=Start2; i> Goal2; i--) //출발점->목표점 내림차순
            {
             LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
              
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D");
    
LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
    Blue= Blue-1; //내려갈때마다 파랑색 -1
    
     DelayMS(500);
    
    
            }
            
         
            
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
          }
          else if (Start == Goal) //출발=목표 일때
          {
          }
          else // 출발 <목표일때
          {
            for (i=Start2; i< Goal2; i++) //출발점->도착점 오름차순
            {
            LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
              
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감
    
   LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
    Blue= Blue+1; //올라갈때마다 파란색 +1
    
     DelayMS(500);
    
    
            }
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
            
          }
         
          
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
          
          
        }
        
        else if (Blue < Start2) //파랑색 현재층이 출발층보다 작을때
        {
           for (i=Blue; i< Start2; i++) //파란색->출발점 오름차순
          {
            LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
            
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감
    
  LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    Blue= Blue+1;  //올라갈때마다 파란색+1
    
     DelayMS(500);
    
          }
        
          
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
    
     DelayMS(500);
    
     if (Start > Goal) //출발>목표 일때
          {
            for (i=Start2; i> Goal2; i--) //출발점 -> 목표점 내림차순
            {
              LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
              
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D");
    
    LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    Blue= Blue-1; //내려갈때마다 파란색 -1
    
     DelayMS(500);
    
    
    
            }
          
            
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
          }
          else if (Start == Goal) //출발=목표 일때
          {
          }
          else // 출발 <목표일때
          {
            for (i=Start2; i< Goal2; i++) //출발점->목표점 오름차순
            {
              LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(0,13, 13, 78); //사각형 리셋
              
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감
    
 LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
    Blue= Blue+1; //올라갈때마다 파란색 +1
    
     DelayMS(500);
    
    
            }
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
             
          }
     
    
       
        }
        
        
        
        
    
     
    GPIOG->ODR ^= 0x01; //led 0 on
    GPIOG->ODR &= ~0x0080; //led 7번 off
     BEEP(); //beep 세번
      DelayMS(500);
     BEEP();
      DelayMS(500);
     BEEP();
     
     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Red 
    LCD_DisplayChar(9,0, (Blue+0x30)); //파란색 현재 층수 표시
     
     
     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(0,15,"FL");  //층선택 모드 복귀
     
     Mode =0; //층선택 복귀
    
      }
     
      else //빨간색이 더 가까울때
      {
        LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red 
    LCD_DisplayText(3,6,"L:S"); //stop
    
     DelayMS(500);
    
     if (Red == Start2) //파랑색 현재층과 출발층이 같을때
        {
          
          
          
          if (Start > Goal) //출발>목표 일때
          {
            for (i=Start2; i> Goal2; i--) //출발점->목표점 내림차순
            {
             LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D"); //내려감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
     Red= Red-1; //내려갈때마다 빨간색 -1
    
     DelayMS(500);
    
    
            }
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
     DelayMS(500);
          }
          else if (Start == Goal) //출발=목표 일때
          {
          }
          else // 출발 <목표일때
          {
            for (i=Start2; i< Goal2; i++) //출발점->목표점 오름차순
            {
              
             LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104, 13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
     Red= Red+1; //올라갈때마다 빨간색 +1
    
     DelayMS(500);
    
    
            }
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red 
    LCD_DisplayText(3,6,"L:S"); //stop
          }
          
        }
        else if (Red > Start2) //파랑색 현재층이 출발층보다 클때
        {
          for (i=Red; i>Start; i--) //빨간색 -> 출발점 내림차순
          {
            LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D"); //내려감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
     Red= Red-1; //내려갈때마다 빨간색 -1
    
     DelayMS(500);
    
            
          }
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red 
    LCD_DisplayText(3,6,"L:S"); //stop
          
           if (Start > Goal) //출발>목표 일때
          {
            for (i=Start2; i> Goal2; i--) //출발점->목표점 내림차순
            {
             LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D"); //내려감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
     Red= Red-1; //내려갈 때마다 빨간색 -1
    
     DelayMS(500);
    
            }
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
    
     DelayMS(500);
          }
          else if (Start == Goal) //출발=목표 일때
          {
          }
          else // 출발 <목표일때
          {
            for (i=Start2; i< Goal2; i++) //출발점-> 목표점 오름차순
            {
              LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감 
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
     Red= Red+1; //올라갈때마다 빨간색 +1
    
     DelayMS(500);
    
    
            }
          }
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
          
     DelayMS(500);
          
        }
        
        else if (Red < Start2) //파랑색 현재층이 출발층보다 작을때
        {
           for (i=Red; i< Start2; i++) //빨간색 -> 출발점 오름차순
          {
           LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
     Red= Red+1; //올라갈떄마다 빨간색 +1
    
     DelayMS(500);
    
    
          }
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop
    
     DelayMS(500);
    
     if (Start > Goal) //출발>목표 일때
          {
            for (i=Start2; i> Goal2; i--) //출발점->목표점 내림차순
            {
            LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:D"); //내려감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)-13)); //파란색 사각형의 좌표,  
    
     Red= Red-1; //내려갈 때마다 빨간색 -1
    
     DelayMS(500);
    
    
    
            }
            LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop 
     DelayMS(500);
    
          }
          else if (Start == Goal) //출발=목표 일때
          {
          }
          else // 출발 <목표일때
          {
            for (i=Start2; i< Goal2; i++) //출발점->목표점 오름차순
            {
              LCD_SetBrushColor(RGB_YELLOW); // 색 reset
    LCD_DrawFillRect(104,13, 13, 78); //사각형 리셋
    
               LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:U"); //올라감
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Blue 
    LCD_DrawFillRect(104,13, 13,((13*i)+13)); //파란색 사각형의 좌표,  
    
     Red= Red+1; //올라갈 때마다 빨간색 +1
    
     DelayMS(500);
    
    
            }
          }
          LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S"); //stop 
    
     DelayMS(500);
    
        }
        
        LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red 
    LCD_DisplayChar(8,0, (Red+0x30)); //빨간색 층수 표시
        GPIOG->ODR ^= 0x01; //led 0 on
    GPIOG->ODR &= ~0x0080; //led 7번 off
    
     LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(0,15,"FL");  //층선택 모드 복귀
    
     BEEP();  //beep 세번
      DelayMS(500);
     BEEP();
      DelayMS(500);
     BEEP();

  
    
      }
        
      
      
      SW7_Flag=0; //15번 인터럽트 초기화
      
    }
      
      
    
    else
    {
      
      
        GPIOG->ODR ^= 0x01; //led 0 on
        
                                                   
        if((GPIOH->IDR & 0x0200) == 0 )	// SW1를 눌렸을 때(SW1 = LOW) (GPIOH->IDR.10 = 'L' ?) 
	{	
          if ( (Count%2) ==0 && Goal !=0x31) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Start = 0x31;	// 출발은 1층
                Start2 = 1;
               BEEP();
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,6,Start); //화면에 시작값 입력
                
                
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x02; //LED 1번 on,나머지 off
           
             Count = Count+1; //목표값 입력으로 넘어감
            
          }  
          
           else if (((Count%2)==1) &&  (Start2 != 1)) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
           Goal = 0x31;	// 출발은 1층
                Goal2 = 1;
               BEEP();
              
                
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,9,Goal); //화면에 목표값 입력
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x02; ///LED 1번 on,나머지 off
           
            Count = Count+1; //출발값 입력으로 넘어감
           
            
          }  
          
                
    //           Mode = 1;
                
            }
        else if((GPIOH->IDR & 0x0400) == 0  )	// SW2를 눌렸을 때(SW2 = LOW) (GPIOH->IDR.10 = 'L' ?) 
          {	
            if ( (Count%2) ==0 && Goal !=0x32)  //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Start = 0x32;	// 출발은 1층
                Start2 = 2;
                BEEP();
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,6,Start); //화면에 시작값 입력
                
               
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x04; //LED 2번 on,나머지 off
                
                
            Count = Count+1; //목표값 입력으로 넘어감
           
          }  
          
           else if (((Count%2)==1)  &&  (Start2 != 2)) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Goal = 0x32;	// 출발은 1층
                Goal2 = 2;
               BEEP();
              
                
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,9,Goal); //화면에 목표값 입력
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x04; //LED 2번 on,나머지 off
                
               
            Count = Count+1; //출발값 입력으로 넘어감
           
          } 
              
      
  // Mode = 1;
      
                
                
            }
        else if( (GPIOH->IDR & 0x0800) == 0 )	// SW3를 눌렸을 때(SW3= LOW) (GPIOH->IDR.10 = 'L' ?) 
          {	
            if ( (Count%2) ==0 && Goal !=0x33)  //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Start = 0x33;	// 출발은 1층
                Start2 = 3;
                BEEP();
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,6,Start); //화면에 시작값 입력
                
               
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x08; //LED 3번 on,나머지 off
                
                Count = Count+1; //목표값 입력으로 넘어감
          }  
          
           else if (((Count%2)==1) &&  (Start2 != 3)) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Goal = 0x33;	// 출발은 1층
                Goal2 = 3;
                BEEP();
               
                
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,9,Goal); //화면에 목표값 입력
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x08; //LED 3번 on,나머지 off
               
                 Count =   Count+1; //출발값 입력으로 넘어감
        
          }  
                
           
                
//     Mode = 1;
           
                
                
            }
        else if( (GPIOH->IDR & 0x1000) == 0 )	// SW4를 눌렸을 때(SW4= LOW) (GPIOH->IDR.10 = 'L' ?) 
          {	
            if ( (Count%2) ==0 && Goal !=0x34)  //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Start = 0x34;	// 출발은 1층
                Start2 = 4;
                BEEP();
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,6,Start); //화면에 시작값 입력
                
            
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x10; //LED 4번 on,나머지 off
                
                Count = Count+1; //목표값 입력으로 넘어감
          }  
          
           else if (((Count%2)==1) &&  (Start2 != 4)) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Goal = 0x34;	// 출발은 1층
                Goal2 = 4;
               BEEP();
           
                
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,9,Goal); //화면에 목표값 입력
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x10; //LED 4번 on,나머지 off
         
                 Count = Count+1; //출발값 입력으로 넘어감
          }  
                
                
       // Mode = 1;

               
                
            }
        else if( (GPIOH->IDR & 0x2000) == 0)	// SW5를 눌렸을 때(SW5= LOW) (GPIOH->IDR.10 = 'L' ?) 
          {	if ( (Count%2) ==0 && Goal !=0x35)  //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Start = 0x35;	// 출발은 1층
                Start2 = 5;
               BEEP();
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,6,Start);  //화면에 시작값 입력
                

                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x20; //LED 5번 on,나머지 off
                
                Count = Count+1; //목표값 입력으로 넘어감
          }  
          
           else if (((Count%2)==1) &&  (Start2 != 5)) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Goal = 0x35;	// 출발은 1층
                Goal2 = 5;
                BEEP();
               
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,9,Goal); //화면에 목표값 입력
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x20; //LED 5번 on,나머지 off
              
                 Count = Count+1; //출발값 입력으로 넘어감
          }  
                
                
                
        //  Mode = 1;
                
            
                
            }
        else if( (GPIOH->IDR & 0x4000) == 0)	// SW6를 눌렸을 때(SW6= LOW) (GPIOH->IDR.10 = 'L' ?) 
          {	
            if ( (Count%2) ==0 && Goal !=0x36)  //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Start = 0x36;	// 출발은 1층
                Start2 = 6;
                BEEP();
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,6,Start);  //화면에 시작값 입력
                
               
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x40; //LED 6번 on,나머지 off
                
                Count = Count+1; //목표값 입력으로 넘어감
          }  
          
           else if (((Count%2)==1) &&  (Start2 != 6)) //출발값->도착값->출발값 순서 유지, 층 입력 중복 방지 조건문
          {
            Goal = 0x36;	// 출발은 1층
                Goal2 = 6;
               BEEP();
             
                
                LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
                LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
                LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
                LCD_DisplayChar(4,9,Goal); //화면에 목표값 입력
               
                GPIOG->ODR = 0xFF00;
                GPIOG->ODR ^= 0x01;
                GPIOG->ODR ^= 0x40; //LED 6번 on,나머지 off
          
                 Count = Count+1; //출발값 입력으로 넘어감
          }  
                
               
               
             //  Mode = 1;
                
                
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
        SYSCFG->EXTICR[3] |= 0x7000; 	// EXTI15에 대한 소스 입력은 GPIOH로 설정 (EXTICR4) (reset value: 0x0000)	
        
	
	EXTI->FTSR |= 0x00000100;		// Falling Trigger Enable  (EXTI8:PH8) (EXTI13:PH13) (EXTI14:PH14) (EXTI15:PH15)sw누를때 발생
   
        EXTI->FTSR |= 0x00008000;			// Falling Trigger Enable  (EXTI8:PH8) (EXTI13:PH13) (EXTI14:PH14) (EXTI15:PH15)sw누를때 발생
    
    	  	// EXTI8,15 인터럽트 mask (Interrupt Enable)
        
     EXTI->IMR  |= 0x00000100;  	// EXTI8,,15 인터럽트 mask (Interrupt Enable)

        EXTI->IMR  |= 0x00008000;  
		
	NVIC->ISER[0] |= (1<<23);   // Enable Interrupt EXTI8 Vector table Position 참조  0~31 ISER0, 32~63I SER1......
        NVIC->ISER[1] |= (1<<(8));    // exti  15 enable
}

void DisplayInitScreen(void)
{
    LCD_Clear(RGB_YELLOW);	// 화면 클리어
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(0,0,"Mecha-Elevator");  // Title

    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(0,15,"FL"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(6,4,"6"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(5,4,"5"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);  // 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE); // 글자색 : Blue
    LCD_DisplayText(4,4,"4");
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,4,"3");
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 :YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(2,4,"2");
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(1,4,"1"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red
    LCD_DisplayText(6,10,"6"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red
    LCD_DisplayText(5,10,"5"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red
    LCD_DisplayText(4,10,"4"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red
    LCD_DisplayText(3,10,"3"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red
    LCD_DisplayText(2,10,"2"); 
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_RED);// 글자색 : Red
    LCD_DisplayText(1,10,"1"); 
    
    
    LCD_SetBrushColor(RGB_BLUE); //사각형의 색 Blue 
    LCD_DrawFillRect(0,13, 13,13); //파란색 사각형의 좌표, 크기
    
    LCD_SetBrushColor(RGB_RED); //사각형의 색 Red 
    LCD_DrawFillRect(104,13, 13,13); //빨간색 사각형의 좌표, 크기
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(3,6,"L:S");
    
    LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW);// 글자배경색 : YELLOw
    LCD_SetTextColor(RGB_BLACK);// 글자색 : Black
    LCD_DisplayText(4,6,"1->1");
  
                    
    
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
       if (SW7_Flag == 1) //실행모드시에만 전환 가능
       {
          GPIOG->ODR &= ~0x0001; //led 0번 off
          GPIOG->ODR &= ~0x0080; //led 7번 off
           LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(0,15,"HL");  //중단 모드 표시
    
    DelayMS(500); //5초간 0,5초마다 beep 실행
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
  DelayMS(500);
  BEEP();
          
  LCD_SetFont(&Gulim8);	// 폰트 : 굴림 8
    LCD_SetBackColor(RGB_YELLOW); // 글자배경색 : Yellow
    LCD_SetTextColor(RGB_BLUE);// 글자색 : Blue
    LCD_DisplayText(0,15,"EX");  //실행 모드 표시
       }
    }
    
    
   
      
}


void EXTI15_10_IRQHandler(void)		// EXTI 10~15인터럽트 핸들러
{
 
   
     if(EXTI->PR & 0x8000) 		// EXTI15 Interrupt Pending?       15번만 남기고 다지움
    {
        EXTI->PR |= 0x8000; 		// Pending bit Clear       지우지 않을경우 int. 또 발생
        GPIOG->ODR = 0xFF00;
	SW7_Flag = 1;
       
    }
    
    
    
}


void BEEP(void)			/* beep for 30 ms */
{ 	
	//  GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	//  DelayMS(30);		// Delay 30 ms
	//  GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
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