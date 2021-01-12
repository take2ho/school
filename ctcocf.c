/*
 * ctcocf.c
 *
 * Created: 2020-06-01 오후 9:05:15
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
unsigned char kim[] = {0x1d, 0x05, 0x05, 0x00, 0x1f, 0x11, 0x11, 0x1f};
int mode;



void Interrupt_init()
{
 EIMSK = 0x0f; //sw0,1,2,3 선택
 EICRA = 0x10; // 폴링 엣지
 DDRB = 0xff; //포트b 출력 
 DDRD = 0x00; //포트d 입력 
 SREG |= 0x80; //전체 인터럽트 허가 

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

void CGRAM_Set()
{
 int i;
 LCD_Comm(0x40);
 LCD_Delay(1);
 for (i=0;i<8;i++)
 {
  LCD_Data(kim[i]);
  LCD_Delay(1);
 }
}

interrupt [EXT_INT0] void ext_int0_isr (void)   //인터럽트마다 각각 다른 메세지 출력 
{
 unsigned char str[] = "eun ho kim"; 
 LCD_Pos(0,0);
 LCD_Str(str);
}
interrupt [EXT_INT1] void ext_int1_isr (void)
{
 unsigned char str1[] = "electronic"; 
 LCD_Pos(0,0);
 LCD_Str(str1);
}
interrupt [EXT_INT2] void ext_int2_isr (void)
{
 unsigned char str2[] = "EH kim"; 
 LCD_Pos(0,0);
 LCD_Str(str2);
}
interrupt [EXT_INT3] void ext_int3_isr (void)
{
 unsigned char str3[] = "Robotics"; 
 LCD_Pos(0,0);
 LCD_Str(str3);
}




void main(void)
{
   
   

 LCD_Init();

 Interrupt_init();  
 ;
while (1)
    {
     
 
    }
}
