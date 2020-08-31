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
#include "mod.h"

void mod_init(void){
	mod_io_init();
}

uint8_t get_pwr_stan(void){
	return mcnf.pwr_on_f;
}
uint8_t get_det_stan(void){
	return mcnf.det_on_f;
}
uint8_t get_mod_f(void){
	return mcnf.mod_f;
}

int8_t mod_set_on(uint8_t xmod){
	if( !(mcnf.mod_f & (1<<xmod)) ){
		if(!mcnf.pwr_on_f){
			int8_t err;
			err=mod_pwr_set(1);
			if(err) return err;
		}
		if(mod[xmod].mtk){
			if(!mod[xmod].mtk_f){
				mod[xmod].mtk(1);				
				return F_ON_PROGRES;
			}
		}
		if(mod[xmod].mpk){
			if(!mod[xmod].mpk_f){
				mod[xmod].mpk(1);
				return F_ON_PROGRES;				
			}
		}
		if(mod[xmod].ena && mod[xmod].ena_f==0){
			if(!mcnf.adc_cnt){
				mod[xmod].ena(1);
				mcnf.adc_cnt++;
				return F_ON_PROGRES;
			}else{
				if(mcnf.adc_cnt==mcnf.adc_delay){
					mod[xmod].ena_f=1;
				}else{
					mcnf.adc_cnt++;
					return F_ON_PROGRES;
				}
			}
		}
		mcnf.mod_f |= (1<<xmod);
		return F_OK;
	}else{
		return F_EMPTY_COLL;
	}
}

int8_t mod_set_off(uint8_t xmod){
	if( (mcnf.mod_f & (1<<xmod)) || mcnf.pwr_on_f ){	
		if(mod[xmod].ena && mod[xmod].ena_f==1){
			mod[xmod].ena(0);
			mcnf.adc_cnt=0;
			mod[xmod].ena_f=0;
			return F_ON_PROGRES;
		}	
		if(mod[xmod].mpk_f){
			if(mod[xmod].mpk) mod[xmod].mpk(0);
			return F_ON_PROGRES;
		}
		if(mod[xmod].mtk_f){
			if(mod[xmod].mtk) mod[xmod].mtk(0);
			mcnf.mod_f &= ~(1<<xmod);
			return F_ON_PROGRES;			
		}
		
		if(mcnf.pwr_on_f){			
			if(mcnf.mod_f==0) {
				int8_t err=mod_pwr_set(0); 
				if(err) return err;
			}
		}
		
		return F_OK;
	}else{
		return F_EMPTY_COLL;
	}
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