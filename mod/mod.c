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
#include "fnc_ret.h"
#include "adc_m328pb.h"

void mod_init(void){
	mod_io_init();
}

int8_t mod_pwr_set(uint8_t st){
	if(st!=mcnf.pwr_on_f){
		if(st){
			//wlaczenie zasilania
			if(!mcnf.pwr_cnt){
				if( mcnf.det){
					if(mcnf.det_on_f==0) mcnf.det(st);
					if(det_int_f){
						det_int_f=0;
						mcnf.pwr_cnt++;
						if(mcnf.pwr){
							mcnf.pwr(st);
							return F_ON_PROGRES+2;
						}else{
							return F_BRAK_DEF-2;
						}
					}else{
						return F_ON_PROGRES+1;
					}
				}else{
					return F_BRAK_DEF-1;
				}
			}else{
				if(mcnf.pwr_cnt==mcnf.pwr_delay){
					mcnf.pwr_on_f=st;
					return F_OK;
				}else{
					mcnf.pwr_cnt++;
					return F_ON_PROGRES+3;
				}
			}			
			
		}else{
			//wylaczenie zasilania
			if(mcnf.pwr){
				mcnf.pwr(st);
				mcnf.pwr_on_f=st;
				mcnf.pwr_cnt=0;
				//wylaczenie detekcji
				if( mcnf.det_on_f && mcnf.det ) mcnf.det(0);
				return F_OK;
			}else{
				return F_BRAK_DEF-3;
			}
		}
	}else{
		return F_EMPTY_COLL;
	}
	
}