/*
 * adc_m328pb.c
 *
 * Created: 30.08.2020 22:00:11
  * Author : Piotr Michna
  * e-mail : pm@piotrmichna.pl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc_m328pb.h"
#if ADC_SLEEP_MODE==1
	#include <util/atomic.h>
	#include <avr/sleep.h>
	
#else
extern volatile uint16_t adc_res;
extern volatile uint8_t adc_flag;
#endif


ISR(ADC_vect){
#if ADC_SLEEP_MODE==0
adc_res=ADC;
adc_flag=1;
ADCSRA=0;
#endif
}

#if ADC_SLEEP_MODE==1
uint16_t adc_get(uint8_t modx){
	ADMUX=ADC_REFS | modx;
	if(modx==1) DIDR0=(1<<ADC1D);
	if(modx==0) DIDR0=(1<<ADC0D);
	if(!ADCSRA){
		ADCSRA=(1<<ADEN) | (1<<ADIE);	//wlaczenie przetwornika i zezwolenie naprzerwanie
		ADCSRA |= ADC_PRESCALER;		//ustawienie preskalera			
	}	

	ADCSRA |= (1<<ADATE);
	set_sleep_mode(SLEEP_MODE_ADC);    //Tryb noise canceller
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {sleep_enable();};     //Odblokuj mo¿liwoœæ wejœcia w tryb sleep
	sleep_cpu();                       //WejdŸ w tryb uœpienia
	sleep_disable();                   //Zablokuj mo¿liwoœæ wejœcia w tryb sleep
	return ADC;
}

void adc_stop(void){
	ADMUX=0;
	ADCSRA=0;
}
#else
void adc_start(uint8_t modx){
	ADMUX=ADC_REFS | modx;
	if(!ADCSRA){
		ADCSRA=(1<<ADEN) | (1<<ADIE);	//wlaczenie przetwornika i zezwolenie naprzerwanie
		ADCSRA |= ADC_PRESCALER;		//ustawienie preskalera
	}
	if(modx==1) DIDR0=(1<<ADC1D);
	if(modx==0) DIDR0=(1<<ADC0D);
		
	ADCSRA |= (1<<ADSC);
	adc_flag=0;
}
uint16_t adc_get(void){
	if(adc_flag) return adc_res; else return 0;
}
#endif
