/*
 * watch.c
 *
 * Created: 2020-06-17 오전 9:38:05
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
unsigned char almin,alhour;
unsigned char almin1,alhour1;
unsigned char almin2,alhour2;
unsigned char stopmin,stopsec;
unsigned char day,month;
unsigned char AM[] = "AM";
unsigned char PM[] = "PM";      
unsigned char ON[] = "ON ";  
unsigned char OFF[] = "OFF";     
unsigned char SECOND[] = "SEC";        
unsigned char str10[] ="    ";  
unsigned char str[] = "    -  -  ";  
unsigned char str1[] = "     :  :  ";   
int mode,change,settime,set,alarm,alset,stop,swatch,stopset,zero,number;
int alset1,alset2;
int onoff,year;








void Interrupt_init()
{
 EIMSK = 0x3f; // sw 0~5 허가
 EICRA = 0xaa; // sw0~3 폴링 엣지  
 EICRB = 0x0a; // sw 4,5
 DDRB = 0xff; //포트b 출력 
 DDRD = 0x00; //포트d 입력 
 SREG |= 0x80; //전체 인터럽트 허가 

}        

void Init_Timer0(void)
{
 TIMSK = (1<<OCIE0);                //출력비교 인터럽트 
 TCCR0 |= (1<<WGM01) | (1<<CS01);   //CTC 모드 
 OCR0 = 100;                        // 50us마다 비교 
 SREG |= 0x80;                      //모든 인터럽트 허가 
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




void LCD_time()
{
      
      LCD_Pos(1,0);
      LCD_Str(str1);
      LCD_Pos(0,1);            
      LCD_Char((year/1000)+'0');
      LCD_Char((year%1000/100)+'0');
      LCD_Char((year%100/10)+'0');
      LCD_Char((year%10)+'0'); 
      LCD_Char(' ');   
      LCD_Char(' ');     
     
     
     
     LCD_Pos(0,6);
     LCD_Char((month/10)+'0');
     LCD_Char((month%10)+'0');        
     LCD_Char(' ');  
    
     LCD_Pos(0,9);   
     LCD_Char((day/10)+'0');
     LCD_Char((day%10)+'0');     
     
     
     if(hour > 12)
     {
      LCD_Pos(1,0);
      LCD_Str(PM);
      LCD_Char(((hour)/10)+'0');
      LCD_Char(((hour)%10)+'0');
     }
     else {
     LCD_Pos(1,0);
     LCD_Str(AM);
     LCD_Char(((hour)/10)+'0');
     LCD_Char(((hour)%10)+'0');       
     }
     LCD_Pos(1,6);
     LCD_Char((min/10)+'0');
     LCD_Char((min%10)+'0'); 
     LCD_Pos(1,9);   
     LCD_Char((sec/10)+'0');
     LCD_Char((sec%10)+'0');   
     LCD_Pos(1,11);
     LCD_Char(' ');  
}


void LCD_Beep()
{
 unsigned char str8[] ="Beep";  
   
  LCD_Pos(0,11);
 LCD_Str(str8);     
}

void LCD_Alarm()
{
 unsigned char str2[] = "Alarm set"; 
 unsigned char str5[] = "   ";
 LCD_Pos(0,1);
 LCD_Str(str2);     


 if(alhour > 12)
     {
      LCD_Pos(1,1);
      LCD_Str(PM);
      LCD_Char(((alhour)/10)+'0');
      LCD_Char(((alhour)%10)+'0');
     }
     else {
     LCD_Pos(1,1);
     LCD_Str(AM);
     LCD_Char(((alhour)/10)+'0');
     LCD_Char(((alhour)%10)+'0');       
     }
     LCD_Pos(1,6);
     LCD_Char((almin/10)+'0');
     LCD_Char((almin%10)+'0');       
     
     LCD_Pos(1,8);
     LCD_Str(str5);         
     
      if(number == 1)
   {
    LCD_Pos(1,0);
    LCD_Char('1');      
    
   }   
   if(number == 2)
   {
    LCD_Pos(1,0);
    LCD_Char('2'); 
   
   }           
   if(number == 3)
   {
    LCD_Pos(1,0);
    LCD_Char('3');  
  
   }   
     
     
      

}

void LCD_Alarm2()
{
 unsigned char str2[] = "Alarm set"; 
 unsigned char str5[] = "   ";
 LCD_Pos(0,1);
 LCD_Str(str2);     


 if(alhour > 12)
     {
      LCD_Pos(1,1);
      LCD_Str(PM);
      LCD_Char(((alhour2)/10)+'0');
      LCD_Char(((alhour2)%10)+'0');
     }
     else {
     LCD_Pos(1,1);
     LCD_Str(AM);
     LCD_Char(((alhour2)/10)+'0');
     LCD_Char(((alhour2)%10)+'0');       
     }
     LCD_Pos(1,6);
     LCD_Char((almin2/10)+'0');
     LCD_Char((almin2%10)+'0');       
     
     LCD_Pos(1,8);
     LCD_Str(str5);         
     
      if(number == 1)
   {
    LCD_Pos(1,0);
    LCD_Char('1');      
    
   }   
   if(number == 2)
   {
    LCD_Pos(1,0);
    LCD_Char('2'); 
   
   }           
   if(number == 3)
   {
    LCD_Pos(1,0);
    LCD_Char('3');  
  
   }   
     
     
      

}

void LCD_Alarm1()
{
 unsigned char str2[] = "Alarm set"; 
 unsigned char str5[] = "   ";
 LCD_Pos(0,1);
 LCD_Str(str2);     


 if(alhour > 12)
     {
      LCD_Pos(1,1);
      LCD_Str(PM);
      LCD_Char(((alhour1)/10)+'0');
      LCD_Char(((alhour1)%10)+'0');
     }
     else {
     LCD_Pos(1,1);
     LCD_Str(AM);
     LCD_Char(((alhour1)/10)+'0');
     LCD_Char(((alhour1)%10)+'0');       
     }
     LCD_Pos(1,6);
     LCD_Char((almin1/10)+'0');
     LCD_Char((almin1%10)+'0');       
     
     LCD_Pos(1,8);
     LCD_Str(str5);         
     
      if(number == 1)
   {
    LCD_Pos(1,0);
    LCD_Char('1');      
    
   }   
   if(number == 2)
   {
    LCD_Pos(1,0);
    LCD_Char('2'); 
   
   }           
   if(number == 3)
   {
    LCD_Pos(1,0);
    LCD_Char('3');  
  
   }   
     
     
      

}
void LCD_Stop()
{
 unsigned char str3[] = " Stop Watch ";
 unsigned char str6[] = "   ";  
 unsigned char str7[] = "    ";
 LCD_Pos(0,0);
 LCD_Str(str3);
 LCD_Pos(1,0);  
 LCD_Str(str6); 
 LCD_Pos(1,2);
 LCD_Char((stopmin/10)+'0');
 LCD_Char((stopmin%10)+'0'); 
 LCD_Pos(1,5);   
 LCD_Char((stopsec/10)+'0');
 LCD_Char((stopsec%10)+'0'); 
 LCD_Pos(1,7);  
 LCD_Str(str7);
}

 interrupt [TIM0_COMP] void timer0_out_comp (void)
{
 if(mode == 0)
 {
  cnt++;
  if(cnt == 200) //1/100초마다 한번
 {
  cnt = 0;
  sec++;    
  LCD_Pos(1,9);   
  LCD_Char((sec/10)+'0');
  LCD_Char((sec%10)+'0'); 
  if(sec == 60)
  {
   min++; sec =0;      
   LCD_Pos(1,6);
   LCD_Char((min/10)+'0');
   LCD_Char((min%10)+'0');
   LCD_Pos(1,9);   
   LCD_Char((sec/10)+'0');
   LCD_Char((sec%10)+'0'); 
  }               
  if(min==60)
  {
   hour++; min=0; 
    
     
     if(hour > 12)
     {
      LCD_Pos(1,0);
      LCD_Str(PM);
      LCD_Char(((hour)/10)+'0');
      LCD_Char(((hour)%10)+'0');
     }
     else {
     LCD_Pos(1,0);
     LCD_Str(AM);
     LCD_Char(((hour)/10)+'0');
     LCD_Char(((hour)%10)+'0');       
     }
     LCD_Pos(1,6);
     LCD_Char((min/10)+'0');
     LCD_Char((min%10)+'0'); 
     LCD_Pos(1,9);   
     LCD_Char((sec/10)+'0');
     LCD_Char((sec%10)+'0');   
  }              
  if(hour>=24) hour =0;  
  
 }          
 
 
 }

 if (swatch == 1)
 {
  cnt++;
 if(cnt == 200) //1/100초마다 한번
 {
  cnt = 0;
  stopsec--; 
  LCD_Stop();
  if(stopsec > 100)
  {
   stopmin--; stopsec =59;     
   LCD_Stop();
  }               
  if((stopmin == 0) && (stopsec == 0) )
  {              
   LCD_Stop();
   swatch = 0;
  }              
 
 } 
 }    
 }
 
 
 
                  
void LCD_Alarmselect()
{
 if(number ==1)
 {
  LCD_Alarm();
 }            
 else if (number ==2)
 {
  LCD_Alarm1();
 }             
 else if (number ==3)
 {
  LCD_Alarm2();
 }
} 
 
 
       


interrupt [EXT_INT0] void ext_int0_isr (void)   
{
 if((mode == 0) ||(mode ==1))
 {
 set += 1;  
 mode = 1;
 
 }   
 
 if (set ==2)
 {                
  settime = 1;
  set = 0;                 
  mode = 0;
 }
 
 
 
 if(mode ==2)
 {
  number += 1;
  if(number == 4)
  {
   number = 1;
  }  
  LCD_Alarmselect();  
 }
}
interrupt [EXT_INT1] void ext_int1_isr (void)
{
     
 if (mode ==1)
     {
      settime += 1;     
      if(settime == 7)
      {
       settime = 1;
      }
      LCD_time();      
      
      
     }                   
 else if (mode == 2)
     {
      if(number == 1)
      {
      alset += 1;         
      LCD_Pos(1,13);
      LCD_Str(ON);   
      onoff = 1;
     
      
      if ((alset % 2)==0)
       {
        alset = 0;
        LCD_Pos(1,13);
        LCD_Str(OFF);       
        onoff = 0;
       }
       }     
       
         if(number == 2)
      {
      alset1 += 1;         
      LCD_Pos(1,13);
      LCD_Str(ON);      
      onoff = 1;
     
      
      if ((alset1 % 2)==0)
       {
        alset1 = 0;
        LCD_Pos(1,13);
        LCD_Str(OFF); 
        onoff = 0;
       }
       }            
         if(number == 3)
      {
      alset2 += 1;         
      LCD_Pos(1,13);
      LCD_Str(ON);     
      onoff = 1;
     
      
      if ((alset2 % 2)==0)
       {
        alset2 = 0;
        LCD_Pos(1,13);
        LCD_Str(OFF);    
        onoff = 0;
       }
       }
     }       
 else if (mode == 3)
     {
      stopset += 1;
       if ((stopset % 3)==2)
       {    
        swatch = 1;
        stopset = 0; 
       }
     }
 
}
interrupt [EXT_INT2] void ext_int2_isr (void)
{
 
 if ((mode ==1))
 {     
   if (settime ==1)
     {
      year += 1;
     }   
     if (settime ==2)
     {
      month += 1;
     } 
     if(settime ==3)
     {
      day +=1;
     }    
     if(settime ==4)
     {
      hour +=1;
     }    
     if(settime ==5)
     {
      min +=1;
     }      
     if(settime ==6)
     {
      sec +=1;
     }     
     if(settime ==7)
     {
      settime = 1;
     }       
     LCD_time();      
     }       
  if (mode == 2)
  {
   if(number ==1)
   {
   almin += 10;
   }
   else if (number ==2)
   {
    almin1 +=10;
   }            
   else if (number ==3)
   {
    almin2 +=10;
   }
   if(zero==1)
   {
    almin = 0;
    zero =0;
   }
   LCD_Alarmselect();
  
  }   
  
  if ((mode == 3) && (stop ==1))
  {
   stopsec += 1;
   LCD_Stop();
  
  }
     
}
interrupt [EXT_INT3] void ext_int3_isr (void)
{
 if (mode ==1)
 {
 if (settime ==1)
     {
      year -= 1;
     }   
     if  (settime ==2)
     {
      month -= 1;
     } 
     if(settime ==3)
     {
      day -=1;
     }    
     if(settime ==4)
     {
      hour -=1;
     }    
     if(settime ==5)
     {
      min -=1;
     }      
     if(settime ==6)
     {
      sec -=1;
     }    
     if(settime ==7)
     {
      settime = 1;
     }       
     
     LCD_time();  
  }               
  
  if (mode == 2)
  {
  if(number ==1)
  {
   almin -= 10;  
  }
  else if (number ==2)
  {
   almin1 -=10;
  }            
  else if (number ==3)
  {
   almin2 -=10;
  }
   LCD_Alarmselect();
  }     
  
  if ((mode == 3) && (stop ==1))
  {
   stopsec -= 1;
   LCD_Stop();
  
  }
}


interrupt [EXT_INT4] void ext_int4_isr (void)
{
 alarm += 1;
 mode = 2; 
 LCD_Alarmselect();   
 
 
}


interrupt [EXT_INT5] void ext_int5_isr (void)
{
 if((mode == 0) || (mode == 1) || (mode == 3))
 {
 stop +=1;
 mode = 3; 
 LCD_Stop();     
 LCD_Pos(1,13);
 LCD_Str(SECOND);   
 }       
 
}

void main(void)
{

   
 LCD_Init();
 LCD_Pos(0,0);
 LCD_Str(str);              
 LCD_Pos(1,0);
 LCD_Str(str1);     
 LCD_Pos(1,13);
 LCD_Str(OFF);
 Temp =0;
 cnt =0;
 sec = min = stopset =zero = 0;
 hour =0;              
 onoff = 0;
 mode = change = set = alarm = alset = stopsec = stopmin = stop = swatch = 0;    
 almin = alhour = almin1 = alhour1 = almin2 = alhour2 = 0;
 day = month = alset1 = alset2 = 0;  
 number = settime = 1;
 year = 2000;        
 LCD_time();
 Init_Timer0();   
 Interrupt_init();
 
 

while (1)
{
 if(settime == 1)
 {
  LCD_Pos(0,4);
 }   
          
 if(settime == 2)
 {
  LCD_Pos(0,7);
 }
 if(settime == 3)
 {
  LCD_Pos(0,10);
 }             
 if(settime == 4)
 {    
  LCD_Pos(1,3);   
 } 
 if(settime == 5)
 {    
  LCD_Pos(1,7);   
 }           
 if(settime == 6)
 {    
  LCD_Pos(1,10);   
 }           
 
     
     if(sec == 60)
  {
   min+=1; sec =0;
  }               
  if(min==60)
  {
   hour+=1; min=0;
  }              
  if(hour==24) hour =0;  
  
  if(day == 30)
  {
   month+=1; day =0;
  }               
  if(month==12)
  {
   year+=1; month=0;
  }              
  if(year==3000) year =0;          
  
   if(almin>=50)
  {
   alhour+=1; almin= 0; zero =1; 
  }              
  if(alhour==24) alhour =0;     
    if(almin1>=50)
  {
   alhour1+=1; almin1= 0; zero =1; 
  }              
  if(alhour1==24) alhour1 =0;                 
  
    if(almin2>=50)
  {
   alhour2+=1; almin2= 0; zero =1; 
  }              
  if(alhour2==24) alhour2 =0; 
  
   if(stopsec == 60)
  {
   stopmin+=1; stopsec =0;
  } 
   if(stopsec > 100)
  {
   stopmin-=1; stopsec = 59;   
  }       
              
  if ((stop )==2)
  {
   stop = 0;
   mode = 0;   
   LCD_time();
   LCD_Pos(0,5);
   LCD_Char(' ');
   LCD_Pos(0,8); 
   LCD_Char(' ');           
   if(onoff == 0)
   {
    LCD_Pos(1,13);
    LCD_Str(OFF);   
   }           
   if(onoff == 1)
   {
    LCD_Pos(1,13);
    LCD_Str(ON);
   }
  }           
  if ((alarm )==2)
  {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
   alarm = 0;
   mode = 0;   
   LCD_time();
   LCD_Pos(0,5);
   LCD_Char(' ');
   LCD_Pos(0,8); 
   LCD_Char(' ');
  }              
  
  
  if((onoff==1) && (number==1) && (min == almin) && (hour == alhour))
  {
   LCD_Beep();
   delay_ms(3000);    
   
   LCD_Pos(0,11);
   LCD_Str(str10);  
   LCD_time();     
   LCD_Pos(1,13);
   LCD_Str(ON);     
  }         
   if((onoff==1) && (number==2) && (min == almin1) && (hour == alhour1))
  {
   LCD_Beep();
   delay_ms(3000);  
  
   LCD_Pos(0,11);
   LCD_Str(str10);  
   LCD_time(); 
   LCD_Pos(1,13);
   LCD_Str(ON);     
  }    
  
   if((onoff==1) && (number==3) && (min == almin2) && (hour == alhour2))
  {
   LCD_Beep();
   delay_ms(3000);  
    
   LCD_Pos(0,11);
   LCD_Str(str10);       
   LCD_time();    
   LCD_Pos(1,13);
   LCD_Str(ON);     
  }      
  
  
     
}

}






