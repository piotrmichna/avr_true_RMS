/*
 * mod_io.h
 *
 * Created: 31.08.2020 06:39:38
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 


#ifndef MOD_IO_H_
#define MOD_IO_H_

#define MOD_NUM 2					// ilosc definiowanych modolow
#define NAZWA_NUM 5					// dlugosc nazwy modulu +1
#define ADC_SAMPLE_NUM 5			// ilosc pomiarow ADC do usrednienia max 15
#define ADC_ON_DELAY 10				// czas zwloki pomiaru po wlaczeniu przetwornika

#define DET_INT_OFF 1				// detekcja przejscia prze zero fazy 230VAC
#define DET_INT_PIN PD3
#define DET_INT_PORT D

#define PWR_OFF 0					// sterowanie zasilaniem modulow
#define PWR_DELAY 10				// czas zwloki po wlaczeniu zasilania max 255
#define PWR_CNT_DELAY 0x11111111	// maksymalny licznik zwloki dostepu do zasilacza
#define PWR_PIN PC4
#define PWR_PORT C


// STEROWNIE KANA� 0
#define ADC0_KANAL 0		//nr kanalu ADC
#define SW0_OFF 1		//detekcja obecnosci modulu sterujacego
#define MPK0_OFF 0		//sterowanie przekaznikiem
#define MTK0_OFF 0		//sterowanie triakiem
#define ENA0_OFF 1		//detekcja obecnosci modulu pomiarowego
#define DET0_OFF 1		//detekcja napiecia na wyjscu modulu pomiarowego
#define RMS0_OFF 0		//sygnal pomiarowy True RMS dla pro�du odbiornika

#define SW0_PIN PD4
#define MPK0_PIN PD5
#define MTK0_PIN PD6
#define ENA0_PIN PD7
#define DET0_PIN PB0
#define RMS0_PIN PC0

#define SW0_PORT D
#define MPK0_PORT D
#define MTK0_PORT D
#define ENA0_PORT D
#define DET0_PORT B
#define RMS0_PORT C

// STEROWANIE KANA� 1
#define ADC1_KANAL 1		//nr kanalu ADC
#define SW1_OFF 1		//detekcja obecnosci modulu sterujacego
#define MPK1_OFF 0		//sterowanie przekaznikiem
#define MTK1_OFF 0		//sterowanie triakiem
#define ENA1_OFF 1		//detekcja obecnosci modulu pomiarowego
#define DET1_OFF 1		//detekcja napiecia na wyjscu modulu pomiarowego
#define RMS1_OFF 0		//sygnal pomiarowy True RMS dla pro�du odbiornika

#define SW1_PIN PB1
#define MPK1_PIN PB2
#define MTK1_PIN PC2
#define ENA1_PIN PC5
#define DET1_PIN PC3
#define RMS1_PIN PC1

#define SW1_PORT B
#define MPK1_PORT B
#define MTK1_PORT C
#define ENA1_PORT C
#define DET1_PORT C
#define RMS1_PORT C


typedef struct{
	uint8_t mod_f:4;	// bity stanow modulow
	uint8_t mod_num:4;	// dostepnych modulow
	uint8_t mod_instal:4;
	uint8_t init_f:1;	// stan inicjacji biblioteki
	uint8_t pwr_on_f:1;	// stan wlaczenia zasilania
	uint8_t det_on_f:1;	// stan detekcji napiecia 240VAC
	
	void (*pwr)(uint8_t);	// wskaznik na funkcje setrujaca zasilaniem
	void (*det)(uint8_t);	// funkcja sterujaca zezwoleniem na przerwanie
	uint8_t pwr_delay;	// zwloka dostepu do zasilania
	uint8_t pwr_cnt;	// odliczanie zwloki dostepu do zasilania
	uint8_t adc_delay;
	uint8_t adc_cnt;
	
}TMOD_CNF;

typedef struct{
	uint8_t kanal:7;
	uint8_t adc_f:1;
	uint16_t adc_buf[ADC_SAMPLE_NUM];
	uint8_t buf_id:4;
	uint8_t buf_num:4;
	uint16_t i;
	uint16_t imax;
	uint16_t imin;
}TMOD_ADC;

typedef struct{
	char nazwa[NAZWA_NUM];
	void (*mpk)(uint8_t);
	void (*mtk)(uint8_t);
	void (*ena)(uint8_t);
	uint8_t (*sw)(void);
	uint8_t mpk_f :1;
	uint8_t mtk_f :1;
	uint8_t sw_f :1;
	uint8_t ena_f :5;
	TMOD_ADC madc;
}TMOD;

TMOD mod[MOD_NUM];
TMOD_CNF mcnf;
volatile uint8_t det_int_f;

void mod_io_init(void);
void mod_check(void);

#endif /* MOD_IO_H_ */