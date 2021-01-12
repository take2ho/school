/*
 * lcdtim.c
 *
 * Created: 2020-06-10 오전 10:28:17
 * Author: ehkim
 */

#include <mega128.h>
#include <delay.h>

#define LCD_WDATA   PORTB
#define LCD_WINST   PORTB

#define LCD_CTRL    PORTG
#define LCD_EN  0
#define LCD_RW  1
#define LCD_RS  2
#define RIGHT 1
#define LEFT 0
unsigned char Temp;
unsigned int cnt;
unsigned char sec,min,hour;

void Init_Timer0(void)
{
 TIMSK = (1<<OCIE0);                //출력비교 인터럽트 
 TCCR0 |= (1<<WGM01) | (1<<CS01);   //CTC 모드 
 OCR0 = 100;                        // 50us마다 비교 
 SREG |= 0x80;                      //모든 인터럽트 허가 
}      

interrupt [TIM0_COMP] void timer0_out_comp (void)
{
 cnt++;
 if(cnt == 200) //1/100초마다 한번
 {
  cnt = 0;
  sec++;
  if(sec >= 60)
  {
   min++; sec =0;
  }               
  if(min>=60)
  {
   hour++; min=0;
  }              
  if(hour>=24) hour =0;
 }
}

void LCD_PORT_Init()
{
 DDRB = 0xFF;
 DDRG = 0x0f;

}

void LCD_Data(unsigned char ch)
{
 LCD_CTRL |= (1<<LCD_RS);
 LCD_CTRL &= ~(1<<LCD_RW);
 LCD_CTRL |= (1<<LCD_EN);
 delay_us(50);
 LCD_WDATA = ch;
 delay_us(50);
 LCD_CTRL &= ~(1<<LCD_EN);
}

void LCD_Comm(unsigned char command)
{
 LCD_CTRL &= ~(1<<LCD_RS);
 LCD_CTRL &= ~(1<<LCD_RW);
 LCD_CTRL |= (1<<LCD_EN);
 delay_us(50);
 LCD_WINST = command;
 delay_us(50);
 LCD_CTRL &= ~(1<<LCD_EN);
}

void LCD_Delay(unsigned char ms)
{
 delay_ms(ms);
}

void LCD_Char(unsigned char c)
{
 LCD_Data(c);
 delay_ms(1);
}

void LCD_Str(unsigned char *str)
{
 while(*str !=0) {
  LCD_Char(*str);
  str++;
 }
}

void LCD_Pos(unsigned char x, unsigned char y)
{
 LCD_Comm(0x80|(x*0x40+y));
}

void LCD_Clear()
{
 LCD_Comm(0x01);
 LCD_Delay(2);
}

void LCD_Init()
{
 LCD_PORT_Init();
 
 LCD_Comm(0x38);
 LCD_Delay(4); 
 LCD_Comm(0x38);
 LCD_Delay(4);  
 LCD_Comm(0x38);
 LCD_Delay(4);
 LCD_Comm(0x0e);
 LCD_Delay(2);
 LCD_Comm(0x06);
 LCD_Delay(2);
 LCD_Clear();
}

void LCD_Cursor_Home(void)
{
 LCD_Comm(0x02);
 LCD_Delay(2);
}

void LCD_Display_Shift(unsigned char p)
{
 if(p==RIGHT) {
    LCD_Comm(0x1C);
    LCD_Delay(1);
    }
 else if (p==LEFT) {
    LCD_Comm(0x18);
    LCD_Delay(1);
    }   

}

void LCD_Cursor_Shift(unsigned char p)
{
  if(p==RIGHT) {
    LCD_Comm(0x14);
    LCD_Delay(1);
    }
 else if (p==LEFT) {
    LCD_Comm(0x10);
    LCD_Delay(1);
    }   
}


void main(void)
{
 unsigned char str[] = "Current time";  
 unsigned char str1[] = "AM 12:00:00";   
 unsigned char AM[] = "AM";
 unsigned char PM[] = "PM";    
 LCD_Init();
 LCD_Pos(0,0);
 LCD_Str(str);              
 LCD_Pos(1,0);
 LCD_Str(str1);   
 Temp =0;
 cnt =0;
 sec = min =0;
 hour =12;
 Init_Timer0();
 

while (1)
    {
     if(hour > 12)
     {
      LCD_Pos(1,0);
      LCD_Str(PM);
      LCD_Char(((hour-12)/10)+'0');
      LCD_Char(((hour-12)%10)+'0');
     }
     else {
     LCD_Pos(1,0);
     LCD_Str(AM);
     LCD_Char(((hour-12)/10)+'0');
     LCD_Char(((hour-12)%10)+'0');       
     }
     LCD_Pos(1,6);
     LCD_Char((min/10)+'0');
     LCD_Char((min%10)+'0'); 
     LCD_Pos(1,9);   
     LCD_Char((sec/10)+'0');
     LCD_Char((sec%10)+'0'); 
    }
}

