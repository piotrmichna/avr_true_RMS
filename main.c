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
	
	uint8_t cnt=99,n=0;
	
    while (1){
		if(TIFR1 & (1<<OCF1A)){
			TIFR1 |= (1<<OCF1A);
			if(cnt){
				cnt--;
			}else{
				cnt=99;
				n++;				
				uart_clear();
#if SPEED_ENABLE == 1
				spt_start();
#endif
				uart_puts("n=");
				uart_putint(n,10);
#if SPEED_ENABLE == 1
				spt_stop();
				uart_puts("\n\rspt_tim=");
				uart_putint(spt_get_tim(),10);
				uart_puts("\n\rspt_val=");
				uart_putint(spt_get_val(),10);
#endif
			}
		}
    }
}


