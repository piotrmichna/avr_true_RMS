/*
 * AVR-RMS-MOD.c
 *
 * Created: 30.08.2020 20:06:27
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "macr.h"
#include "speed_test.h"
#include "uart/uart328pb.h"

#include "mod/adc_m328pb.h"

void main_event(void);
uint16_t val_adc[2];

int main(void){
    //timer1 mode CTC
    TCCR1B |= (1<<WGM12);				// tryb CTC
    //TCCR1B |= (1<<CS10) | (1<<CS12);	// prescaler 1024
	//TCCR1B |= (1<<CS12);				// prescaler 256
	TCCR1B |= (1<<CS10)|(1<<CS11);		// prescaler 64
    OCR1A= 2880;						//przerwanie co 10ms
	
	USART_Init( __UBRR);
	sei();
	
	uart_clear();
	uart_puts("START\n\r");
	DDRD |= (1<<PD5);
	PORTD &= ~(1<<PD5);
	uint8_t x=0,n=0;
	
    while (1){
		if(TIFR1 & (1<<OCF1A)){
			TIFR1 |= (1<<OCF1A);
			#if SPEED_ENABLE==1
			spt_start();
			
			if(x<3) n=0; else n=1;
			if(x==5) x=0; else x++;
						
			val_adc[n]=adc_get(n);
			adc_stop();
			spt_stop();
			#endif
			main_event();
		}
    }
}

void main_event(void){
	static uint8_t cnt=99,n=0;
	
	if(cnt){
		cnt--;
	}else{
		cnt=99;
		if(n%10==0){
			PORTD ^= (1<<PD5);
		}
		n++;
		uart_clear();
		uart_puts("n=");
		uart_putint(n,10);
		
#if SPEED_ENABLE==1
		uart_puts("\n\rspt_tim=");
		uart_putint(spt_get_tim(),10);		
#endif
		uart_puts("\n\rADC0=");
		uart_putint(val_adc[0],10);
		uart_puts("\n\rADC1=");
		uart_putint(val_adc[1],10);
	}
}
