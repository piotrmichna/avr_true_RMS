/*
 * mod_io.c
 *
 * Created: 31.08.2020 06:41:20
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "../macr.h"
#include "mod_io.h"

extern volatile uint8_t det_int_f;

extern TMOD mod[MOD_NUM];
extern TMOD_CNF mcnf;

void mod_check(void);
void det_set(uint8_t st);
#ifdef PWR_OFF
void pwr_set(uint8_t st);
#endif

#if MOD_NUM>0
	#ifdef MPK0_OFF
	void mpk0_set(uint8_t st);
	#endif
	#ifdef MTK0_OFF
	void mtk0_set(uint8_t st);
	#endif
	#ifdef SW0_OFF
	uint8_t sw0_get(void);
	#endif
	#ifdef ENA0_OFF
	void ena0_set(uint8_t st);
	#endif
#endif
#if MOD_NUM>1
	#ifdef MPK1_OFF
	void mpk1_set(uint8_t st);
	#endif
	#ifdef MTK1_OFF
	void mtk1_set(uint8_t st);
	#endif
	#ifdef SW1_OFF
	uint8_t sw1_get(void);
	#endif
	#ifdef ENA1_OFF
	void ena1_set(uint8_t st);
	#endif
#endif

ISR(INT1_vect){
	det_int_f=1;
}

void mod_io_init(void){

	if(DET_INT_OFF==1) PORT( DET_INT_PORT ) |= (1<<DET_INT_PIN); else PORT( DET_INT_PORT ) |= (1<<DET_INT_PIN);
	DDR( DET_INT_PORT ) &= ~(1<<DET_INT_PIN);
	if(PWR_OFF==1) PORT( PWR_PORT ) |= (1<<PWR_PIN); else PORT( PWR_PORT ) &= ~(1<<PWR_PIN);
	DDR( PWR_PORT ) |= (1<<PWR_PIN);
	
#if MOD_NUM>0	
	// INICJACJA KANALU 0	
	if(SW0_OFF==1) PORT( SW0_PORT ) |= (1<<SW0_PIN); else  PORT( SW0_PORT ) &= ~(1<<SW0_PIN);
	DDR( SW0_PORT ) &= ~(1<<SW0_PIN);
	if(MPK0_OFF==1) PORT( MPK0_PORT ) |= (1<<MPK0_PIN); else  PORT( MPK0_PORT ) &= ~(1<<MPK0_PIN);
	DDR( MPK0_PORT ) |= (1<<MPK0_PIN);
	if(MTK0_OFF==1) PORT( MTK0_PORT ) |= (1<<MTK0_PIN); else  PORT( MTK0_PORT ) &= ~(1<<MTK0_PIN);
	DDR( MTK0_PORT ) |= (1<<MTK0_PIN);
	if(ENA0_OFF==1) PORT( ENA0_PORT ) |= (1<<ENA0_PIN); else  PORT( ENA0_PORT ) &= ~(1<<ENA0_PIN);
	DDR( ENA0_PORT ) |= (1<<ENA0_PIN);
	if(RMS0_OFF==1) PORT( RMS0_PORT ) |= (1<<RMS0_PIN); else PORT( RMS0_PORT ) &= ~(1<<RMS0_PIN);
	DDR( RMS0_PORT ) &= ~(1<<RMS0_PIN);
	//inicjacja structury mod[0]
	mod[0].nazwa[0]='M';
	mod[0].nazwa[1]='O';
	mod[0].nazwa[2]='D';
	mod[0].nazwa[3]='0';
	mod[0].nazwa[4]='\0';
	#ifdef MPK0_OFF
		mod[0].mpk=mpk0_set;
	#endif
	#ifdef MTK0_OFF
		mod[0].mtk=mtk0_set;
	#endif
	#ifdef SW0_OFF
		mod[0].sw=sw0_get;
	#endif
	#ifdef ENA0_OFF
		mod[0].ena=ena0_set;
	#endif
	// ADC
	mod[0].madc.kanal=ADC0_KANAL;
	mod[0].madc.imin-=1;
#endif

#if MOD_NUM>1
	// INICJACJA KANALU 1
	if(SW1_OFF==1) PORT( SW1_PORT ) |= (1<<SW1_PIN); else  PORT( SW1_PORT ) &= ~(1<<SW1_PIN);
	DDR( SW1_PORT ) &= ~(1<<SW1_PIN);
	if(MPK1_OFF==1) PORT( MPK1_PORT ) |= (1<<MPK1_PIN); else  PORT( MPK1_PORT ) &= ~(1<<MPK1_PIN);
	DDR( MPK1_PORT ) |= (1<<MPK1_PIN);
	if(MTK1_OFF==1) PORT( MTK1_PORT ) |= (1<<MTK1_PIN); else  PORT( MTK1_PORT ) &= ~(1<<MTK1_PIN);
	DDR( MTK1_PORT ) |= (1<<MTK1_PIN);
	if(ENA1_OFF==1) PORT( ENA1_PORT ) |= (1<<ENA1_PIN); else  PORT( ENA1_PORT ) &= ~(1<<ENA1_PIN);
	DDR( ENA1_PORT ) |= (1<<ENA1_PIN);
	if(RMS1_OFF==1) PORT( RMS1_PORT ) |= (1<<RMS1_PIN); else PORT( RMS1_PORT ) &= ~(1<<RMS1_PIN);
	DDR( RMS1_PORT ) &= ~(1<<RMS1_PIN);	

	mod[1].nazwa[0]='M';
	mod[1].nazwa[1]='O';
	mod[1].nazwa[2]='D';
	mod[1].nazwa[3]='1';
	mod[1].nazwa[4]='\0';
	#ifdef MPK1_OFF
		mod[1].mpk=mpk1_set;
	#endif
	#ifdef MTK1_OFF
		mod[1].mtk=mtk1_set;
	#endif
	#ifdef SW1_OFF
		mod[1].sw=sw1_get;
	#endif
	#ifdef ENA1_OFF
		mod[1].ena=ena1_set;
	#endif
	// ADC
	mod[1].madc.kanal=ADC1_KANAL;
	mod[1].madc.imin-=1;
#endif
	mod_check();
	mcnf.pwr_delay=PWR_DELAY;
	mcnf.init_f=1;
	#ifdef PWR_OFF
	mcnf.pwr=pwr_set;
	mcnf.det=det_set;
	#endif
}

void mod_check(void){
	uint8_t st;
	mcnf.mod_num=0;
	for(uint8_t n=0; n<MOD_NUM; n++){
		if(mod[n].sw){
			st=mod[n].sw();
			if(st) mcnf.mod_num++;
		}
	}
}

void det_set(uint8_t st){
	mcnf.det_on_f=st;
	if(st){
		EIMSK |= (1<<INT1);
		EICRA |= (1<<ISC11);
	}else{
		EIMSK &= ~(1<<INT1);
		EICRA &= ~(1<<ISC11);
	}
}

#ifdef PWR_OFF
void pwr_set(uint8_t st){
	if (PWR_OFF==1){
		if(st) st=0; else st=1;
	}
	if(st) PORT( PWR_PORT ) |= (1<<PWR_PIN); else  PORT( PWR_PORT ) &= ~(1<<PWR_PIN);
}
#endif

#if MOD_NUM>0
	#ifdef MPK0_OFF	
	void mpk0_set(uint8_t st){
		mod[0].mpk_f=st;
		if (MPK0_OFF==1){
			if(st) st=0; else st=1;
		}
		if(st) PORT( MPK0_PORT ) |= (1<<MPK0_PIN); else  PORT( MPK0_PORT ) &= ~(1<<MPK0_PIN);
	}
	#endif
	#ifdef MTK0_OFF
	void mtk0_set(uint8_t st){
		mod[0].mtk_f=st;
		if (MTK0_OFF==1){
			if(st) st=0; else st=1;
		}
		if(st) PORT( MTK0_PORT ) |= (1<<MTK0_PIN); else  PORT( MTK0_PORT ) &= ~(1<<MTK0_PIN);
	}
	#endif
	#ifdef SW0_OFF
	uint8_t sw0_get(void){
		uint8_t st;
		st=( PIN(SW0_PORT) & (1<<SW0_PIN) );
		if (SW0_OFF==1){
			if(st) st=0; else st=1;
		}
		mod[0].sw_f=st;
		return st;
	}
	#endif
	#ifdef ENA0_OFF
	void ena0_set(uint8_t st){
		mod[0].ena_f=st;
		if (ENA0_OFF==1){
			if(st) st=0; else st=1;
		}
		if(st) PORT( ENA0_PORT ) |= (1<<ENA0_PIN); else  PORT( ENA0_PORT ) &= ~(1<<ENA0_PIN);
	}
	#endif
#endif
#if MOD_NUM>1
	#ifdef MPK1_OFF
	void mpk1_set(uint8_t st){
		mod[1].mpk_f=st;
		if (MPK1_OFF==1){
			if(st) st=0; else st=1;
		}
		if(st) PORT( MPK1_PORT ) |= (1<<MPK1_PIN); else  PORT( MPK1_PORT ) &= ~(1<<MPK1_PIN);
	}
	#endif
	#ifdef MTK1_OFF
	void mtk1_set(uint8_t st){
		mod[1].mtk_f=st;
		if (MTK1_OFF==1){
			if(st) st=0; else st=1;
		}
		if(st) PORT( MTK1_PORT ) |= (1<<MTK1_PIN); else  PORT( MTK1_PORT ) &= ~(1<<MTK1_PIN);
	}
	#endif
	#ifdef SW1_OFF
	uint8_t sw1_get(void){
		uint8_t st;
		st=( PIN(SW1_PORT) & (1<<SW1_PIN) );
		if (SW1_OFF==1){
			if(st) st=0; else st=1;
		}
		mod[1].sw_f=st;
		return st;
	}
	#endif
	#ifdef ENA1_OFF
	void ena1_set(uint8_t st){
		mod[1].ena_f=st;
		if (ENA1_OFF==1){
			if(st) st=0; else st=1;
		}
		if(st) PORT( ENA1_PORT ) |= (1<<ENA1_PIN); else  PORT( ENA1_PORT ) &= ~(1<<ENA1_PIN);
	}
	#endif
#endif