/*
 * mod.h
 *
 * Created: 31.08.2020 09:14:54
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 


#ifndef MOD_H_
#define MOD_H_


void mod_init(void);
int8_t mod_pwr_set(uint8_t st);
uint8_t get_pwr_stan(void);
uint8_t get_det_stan(void);

#endif /* MOD_H_ */