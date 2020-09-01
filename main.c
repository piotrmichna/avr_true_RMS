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

#include "mod/mod.h"

void main_event(void);
uint8_t stan=1, x=10,cnt,id=0;
int8_t err;
uint8_t mod_n;
int16_t gadc;

int main(void){
    //timer1 mode CTC
    TCCR1B |= (1<<WGM12);				// tryb CTC
    //TCCR1B |= (1<<CS10) | (1<<CS12);	// prescaler 1024
	//TCCR1B |= (1<<CS12);				// prescaler 256
	TCCR1B |= (1<<CS10)|(1<<CS11);		// prescaler 64
    OCR1A= 2880;						//przerwanie co 10ms
	
	USART_Init( __UBRR);
	mod_init();
	sei();
	
	uart_clear();
	uart_puts("START\n\r");
	uint8_t x=0;
	char c;
    while (1){
		c=uart_getc();
		while(c){
			uart_putc(c);
			c=uart_getc();
		}
		if(TIFR1 & (1<<OCF1A)){
			TIFR1 |= (1<<OCF1A);
			mod_event();
			#if SPEED_ENABLE==1
			if(!x){
				x=10;
				if(!cnt){					
					if(stan){
						err=mod_set_on(id);
					}else{
						err=mod_set_off(id);
					}
					if(!err){
						if(stan){
							stan=0;
						}else{
							stan=1;
							mod_n=get_mod_install();
							if(id){
								id=0;
								if( !(mod_n & (1<<id)) ) id=1;
							}else{
								id=1;
								if( !(mod_n & (1<<id)) ) id=0;
							}
						}
						cnt=200;
					}else{
						cnt=0;
					}
				}else{
					cnt--;
				}
			}else{
				x--;
			}
			#endif
			//main_event();
		}
    }
}

void main_event(void){
	static uint8_t cnt=99,n=0;
	
	if(cnt){
		cnt--;
	}else{
		cnt=49;

		n++;
		uart_clear();
		uart_puts("n=");
		uart_putint(n,10);		
#if SPEED_ENABLE==1
		uart_puts("\n\rspt_tim=");
		uart_putint(spt_get_tim(),10);		
#endif
		gadc=get_mod_adci(0);
		uart_puts("\n\rADC0=");
		uart_putint(gadc,10);
		if(gadc>0){
			gadc=get_mod_adcimin(0);
			uart_puts("\n\rADC0min=");
			uart_putint(gadc,10);
			gadc=get_mod_adcimax(0);
			uart_puts("\n\rADC0max=");
			uart_putint(gadc,10);
		}
		gadc=get_mod_adci(1);
		uart_puts("\n\rADC1=");
		uart_putint(gadc,10);
		if(gadc>0){
			gadc=get_mod_adcimin(1);
			uart_puts("\n\rADC1min=");
			uart_putint(gadc,10);
			gadc=get_mod_adcimax(1);
			uart_puts("\n\rADC1max=");
			uart_putint(gadc,10);
		}
		uart_puts("\n\rget_mod_f=");
		uart_putint(get_mod_f(),10);
		uart_puts("\n\rerr=");
		uart_putint(err,10);
		//uart_puts(" sample=");
		//uart_putint(adc_get_sample_num(),10);

	}
}
