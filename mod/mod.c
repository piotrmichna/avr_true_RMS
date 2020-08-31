/*
 * mod.c
 *
 * Created: 31.08.2020 09:16:11
  * Author : Piotr Michna
  * e-mail : pm@piotrmichna.pl
 */ 

#include <avr/io.h>
#include <stdlib.h>

#include "mod_io.h"
#include "adc_m328pb.h"

void mod_init(void){
	mod_io_init();
}
