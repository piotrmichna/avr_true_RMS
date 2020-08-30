/*
 * speed_test.h
 *
 * Created: 30.08.2020 20:32:24
 *  Author: pmich
 */ 


#ifndef SPEED_TEST_H_
#define SPEED_TEST_H_

#define SPEED_ENABLE 1

#if SPEED_ENABLE == 1
	uint16_t spt_get_tim(void);
	int16_t spt_get_val(void);

	void spt_set_val(int16_t val);
	void spt_start(void);
	void spt_stop(void);
#endif

#endif /* SPEED_TEST_H_ */