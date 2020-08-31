/*
 * mod.c
 *
 * Created: 31.08.2020 09:16:11
  * Author : Piotr Michna
  * e-mail : pm@piotrmichna.pl
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <util/atomic.h>

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
uint8_t get_mod_num(void){
	return mcnf.mod_num;
}
uint8_t get_mod_install(void){
	mod_check();
	return mcnf.mod_instal;
}
int16_t get_mod_adci(uint8_t xmod){
	if(mod[xmod].madc.adc_f){
		return mod[xmod].madc.i;
	}else{
		return -1;
	}
}

void mod_event(void){
	static uint8_t id, cnt=ADC_SAMPLE_NUM*2;
	if(mod[0].madc.adc_f || mod[1].madc.adc_f){
		if(mod[id].madc.adc_f && cnt){
			mod[id].madc.adc_buf[mod[id].madc.buf_id]=adc_get(mod[id].madc.kanal);
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				if(mod[id].madc.buf_num<ADC_SAMPLE_NUM) mod[id].madc.buf_num++;
				if(mod[id].madc.buf_id<mod[id].madc.buf_num-1) mod[id].madc.buf_id++; else mod[id].madc.buf_id=0;
				mod[id].madc.i=0;
				for(uint8_t n=0; n<mod[id].madc.buf_num; n++){
					mod[id].madc.i+=mod[id].madc.adc_buf[n];
				}
				mod[id].madc.i=mod[id].madc.i/mod[id].madc.buf_num;
				if(mod[id].madc.imin>mod[id].madc.i) mod[id].madc.imin=mod[id].madc.i;
				if(mod[id].madc.imax<mod[id].madc.i) mod[id].madc.imax=mod[id].madc.i;
			}
			cnt--;
		}else{
			uint8_t x= get_mod_install();
			if(id){				
				id=0;
				if( !(x & (1<<id)) ) id=1; else adc_stop();
			}else{
				id=1;
				if( !(x & (1<<id)) ) id=0; else adc_stop();
			}
			cnt=ADC_SAMPLE_NUM*2;
		}
	}
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
					mod[xmod].madc.adc_f=1;
					
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
			mod[xmod].madc.adc_f=0;
			mod[xmod].madc.imax=0;
			mod[xmod].madc.imin=0;
			mod[xmod].madc.imin-=1;
			mod[xmod].madc.buf_id=0;
			mod[xmod].madc.buf_num=0;
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