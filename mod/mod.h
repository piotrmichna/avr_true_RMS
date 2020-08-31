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
void mod_event(void);
int16_t get_mod_adci(uint8_t xmod);
int16_t get_mod_adcimin(uint8_t xmod);
int16_t get_mod_adcimax(uint8_t xmod);

int8_t mod_pwr_set(uint8_t st);
int8_t mod_set_on(uint8_t xmod);
int8_t mod_set_off(uint8_t xmod);
uint8_t get_pwr_stan(void);
uint8_t get_det_stan(void);
uint8_t get_mod_f(void);
uint8_t get_mod_num(void);
uint8_t get_mod_install(void);

#endif /* MOD_H_ */