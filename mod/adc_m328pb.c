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
#if ADC_SLEEP_MODE == 1
	#include <util/atomic.h>
	#include <avr/sleep.h>
#else
	extern volatile uint16_t adc_res;
	extern volatile uint8_t adc_flag;
#endif
