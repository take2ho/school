/*
 * lcd.c
 *
 * Created: 2020-06-03 ¿ÀÀü 10:14:08
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

void LCD_Comm(unsigned char ch)
{
 LCD_CTRL &= ~(1<<LCD_RS);
 LCD_CTRL &= ~(1<<LCD_RW);
 LCD_CTRL |= (1<<LCD_EN);
 delay_us(50);
 LCD_WDATA = ch;
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

void main(void)
{
 unsigned char str[] = "LCD test..";
 LCD_Init();
 LCD_Pos(0,0);
 LCD_Str(str);
while (1)
    {
    // Please write your application code here

    }
}
