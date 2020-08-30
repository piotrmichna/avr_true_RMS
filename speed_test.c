/*
 * speed_test.c
 *
 * Created: 30.08.2020 20:33:25
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 

#include <avr/io.h>
#include <stdlib.h>

#include "speed_test.h"

#if SPEED_ENABLE == 1
typedef struct{
	uint16_t tstart;
	uint16_t tim;
	int16_t val;
}TSPEED;

TSPEED spt;

uint16_t spt_get_tim(void){	
	spt.tim-=spt.tstart;
	return spt.tim;
}
int16_t spt_get_val(void){	
	return spt.val;
}
void spt_set_val(int16_t val){
	spt.val=val;
}
void spt_start(void){
	spt.tstart=TCNT1;
}
void spt_stop(void){
	spt.tim=TCNT1;
}
#endif