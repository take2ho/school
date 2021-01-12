/*
 * seg.c
 *
 * Created: 2020-06-01 ¿ÀÈÄ 4:25:48
 * Author: ehkim
 */

#include <mega128.h>

#include <delay.h>

 

unsigned char Port_char[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xd8,0x80,0x90,
                             0x88,0x83,0xc4,0xa1,0x84,0x8e};

unsigned int Port_fnd[] = {0x1f, 0x2f, 0x4f, 0x8f};

 

void init_PORT()

{

 DDRE = 0xf0;

 DDRB = 0xff;

}

 

void main()

{

 unsigned char FND0, FND1, FND2, FND3;

 init_PORT();

 

 while(1)

 {

  for(FND3 =0; FND3<10; FND3++)

   {

     for(FND2 =0; FND2<6; FND2++)

     {

      for(FND1 =0; FND1<10; FND1++)

       {

        for(FND0 =0; FND0<10; FND0++)

         {

PORTE = Port_fnd[0];

PORTB = Port_char[FND0];

delay_ms(100);

 

PORTE = Port_fnd[1];

PORTB = Port_char[FND1];

delay_ms(100);

 

PORTE = Port_fnd[2];

PORTB = Port_char[FND2];

delay_ms(100);

 

PORTE = Port_fnd[3];

PORTB = Port_char[FND3];

delay_ms(100);

          }

        }

     }   

   }

 }

}