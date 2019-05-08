/*
 * SonarAtmega.c
 *
 * Created: 5/7/2019 4:10:34 PM
 * Author : Lucas
 */ 

#include "def_principais.h"
#include "LCD.h"
#include <avr/io.h>
#include <avr/interrupt.h>
float x;
int y = 0;
int Ts = 0;
int Td = 0;
int Tf = 0;
int ToBuzz=0;
int CompBuzz = 300;

unsigned char digits[4];
unsigned char segValue [16] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};
	
ISR(TIMER0_COMPA_vect) //interrupção do TC1
{
	y++;
	ident_num(x, digits);
	switch (y)
	{
		case 1:
		PORTC = 0b1111110;
		PORTD = segValue[digits[0]- '0'];
		break;

		case 2:
		PORTC = 0b1111101;
		PORTD = segValue[digits[1]- '0'];
		break;
		
		case 3:
		PORTC = 0b1111011;
		PORTD = segValue[digits[2]- '0'];
		break;
		
		case 4:
		PORTC = 0b1110111;
		PORTD = segValue[digits[3]- '0'];
		y = 0;
		break;	
	}
	
}

ISR(TIMER1_CAPT_vect) //interrupção do T1
{
	if (tst_bit(TCCR1B, ICES1))
	{
		clr_bit(TCCR1B, ICES1);
		Ts = ICR1;
	} 
	else
	{
		set_bit(TCCR1B, ICES1);
		Td = ICR1;
		Tf = Td - Ts;
		//Tf = 1024/(16*(Tf+1));
		x = Tf * 1.1;
	}
	
}

ISR(TIMER2_COMPA_vect) //interrupção do TC1
{
	if (ToBuzz >= CompBuzz)
	{
		cpl_bit(PORTB, PB2);
		ToBuzz = 0;
	}
	ToBuzz++;
}

void CheckBuzz()
{
	if (x>100)
	{
		CompBuzz = 300;
	}
	if (x<=100 && x>=50)
	{
		CompBuzz = 150;
	}
	if (x<=50 && x>=10)
	{
		CompBuzz = 75;
	}
	if (x<=10 && x>=0)
	{
		CompBuzz = 40;
	}
};



int main(void)
{
	DDRD  = 0xFF;	//Conigura como saida
	DDRC  = 0xFF;	//Conigura como saida
	DDRB  = 0b11111110;	//Configura como entrada
	PORTB = 0b11111001;	//Habilita pull up's
	
	
	TCNT0 = 0;
	OCR0A = 77;
	TCCR0B = (1<<CS10) | (1<<CS12) | (1<<WGM12);//TC1 com prescaler de 1024
	TIMSK0 = (1<<OCIE0A); //habilita a interrupção do T1
	
	TCNT1 = 0;									 							 
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<ICES1); 
	TIMSK1 = (1<<ICIE1);
	sei();
	
	TCNT2 = 0;
	TCCR2B = (1<<CS10) | (1<<CS12) | (1<<WGM12);//TC1 com prescaler de 1024
	TIMSK2 = (1<<OCIE2A); //habilita a interrupção do T1
	
    while (1) 
    {
	set_bit(PORTB, PB1);
	_delay_us(10);
	clr_bit(PORTB, PB1);
	_delay_ms(500);
	CheckBuzz();
	Ts = 0;
	Td = 0;
    }
}


