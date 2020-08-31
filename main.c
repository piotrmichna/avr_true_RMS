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
uint8_t stan=1, xx=10,cnt,id=0;
int8_t err;
uint8_t mod_n;

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
	
    while (1){
		if(TIFR1 & (1<<OCF1A)){
			TIFR1 |= (1<<OCF1A);
			#if SPEED_ENABLE==1
			spt_start();
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
							mod_n=get_mod_num();
							if(id){
								id=0;
								if( !(mod_n & (1<<id)) ) id=1;
							}else{
								id=1;
								if( !(mod_n & (1<<id)) ) id=0;
							}
						}
						cnt=40;
					}else{
						cnt=0;
					}
				}else{
					cnt--;
				}
			}else{
				x--;
			}
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
		cnt=49;

		n++;
		uart_clear();
		uart_puts("n=");
		uart_putint(n,10);
		
#if SPEED_ENABLE==1
		uart_puts("\n\rspt_tim=");
		uart_putint(spt_get_tim(),10);		
#endif
		uart_puts("\n\rstan=");
		uart_putint(stan,10);
		uart_puts("\n\rpwr_on_f=");
		uart_putint(get_mod_f(),2);
		uart_puts("\n\rdet_on_f=");
		uart_putint(get_det_stan(),10);		
		uart_puts("\n\rerr=");
		uart_putint(err,10);
		//uart_puts(" sample=");
		//uart_putint(adc_get_sample_num(),10);

	}
}
