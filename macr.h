/*
 * macr.h
 *
 * Created: 30.08.2020 20:10:31
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 


#ifndef MACR_H_
#define MACR_H_

// *** PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
// *** PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
// *** DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)





#endif /* MACR_H_ */