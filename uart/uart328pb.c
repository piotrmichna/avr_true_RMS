/*
 * uart328pb.c
 *
 * Created: 30.08.2020 20:16:13
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "uart328pb.h"
volatile char adr_arr[]={"ST"};

volatile char UART_RxBuf[UART_RX_BUF_SIZE];		// definicja bufora odbiorczego
volatile uint8_t UART_RxHead;					// indeks poczatkowy
volatile uint8_t UART_RxTail;					// indeks koncowy

volatile char UART_TxBuf[UART_TX_BUF_SIZE];		// definicja bufora nadawczego
volatile uint8_t UART_TxHead;					// indeks poczatkowy
volatile uint8_t UART_TxTail;					// indeks koncowy

#if RS485==1
void rs485_odbieranie(void){
	RS_TXEN_PORT &= ~(1<<RS_TXEN_PIN);
	#ifdef RS_RXEN_PIN
	RS_RXEN_PORT &= ~(1<<RS_RXEN_PIN);
	#endif
}
void rs485_nadawanie(void){
	RS_TXEN_PORT |= (1<<RS_TXEN_PIN);
	#ifdef RS_RXEN_PIN
	RS_RXEN_PORT |= (1<<RS_RXEN_PIN);
	#endif
}
#endif

void USART_Init( uint16_t baud ) {
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t)baud;						// ustawienie predkosci komunikacji
	#if RS485 == 1
	RS_TXEN_DDR |= (1<<RS_TXEN_PIN);
	RS_TXEN_PORT &= ~(1<<RS_TXEN_PIN);
	#ifdef RS_RXEN_PIN
	RS_RXEN_DDR |= (1<<RS_RXEN_PIN);
	RS_RXEN_PORT &= ~(1<<RS_RXEN_PIN);
	#endif
	rs485_odbieranie();
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);				// uruchomienie lini komunikacji
	UCSR0B |= (1<<RXCIE0)|(1<<TXCIE0);			// zezwolenie na przerwanie od odbiornika i nadajnika
	UCSR0C = (3<<UCSZ00);						// ramka 8bit 1bit stop
	#else
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);				// uruchomienie lini komunikacji
	UCSR0B |= (1<<RXCIE0);						// zezwolenie na przerwanie od odbiornika
	UCSR0C = (3<<UCSZ00);						// ramka 8bit 1bit stop
	#endif
}

char uart_getc(void) {
	if ( UART_RxHead == UART_RxTail ) return 0;			// sprawdzenie zawartosci pufora odbiorczego
	UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;	// przesuniecie indexu
	return UART_RxBuf[UART_RxTail];						// zwrucenie bajta buforu odbiorczego
}

void uart_putc( char data ) {
	uint8_t tmp_head;
	tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;	// przesuniecie indeksu
	while ( tmp_head == UART_TxTail ){}					// oczekiwanie na zwolnienie miejsca w buforze

	UART_TxBuf[tmp_head] = data;						// zapis znaku do bofora nadawczego
	UART_TxHead = tmp_head;								// zapis indeksu

	UCSR0B |= (1<<UDRIE0);								// wlaczenie przerwania od pustego bufora sprzetowego
}

void uart_puts(char *str){				// wysylanie lancucha
	register char c;
	while ((c = *str++)) uart_putc(c);	// zapis lancucha do bufora nadawczego
}

void uart_putint(int value, int radix){
	char string[17];				// bufor lancucha
	itoa(value, string, radix);		// konwersja liczby na znaki
	uart_puts(string);				// zapis lancucha do bufora nadawczego
}

void uart_clear(void){
	uart_puts("\x1b[");
	uart_puts("m");

	uart_puts("\x1b[");
	uart_puts(";H");

	uart_puts("\x1b[");
	uart_puts("2J");

	uart_puts("\x1b[");
	uart_puts("?25l");
}

#if RS485 == 1
ISR( USART0_TX_vect ) {
	rs485_odbieranie();	// ustaw tryb odbioru
}
#endif

ISR( USART0_RX_vect ) {
	uint8_t tmp_head;
	char data;
	static uint8_t flag=3;

	data = UDR0; //odczyt bufora sprzetowego
	
	if(flag){
		if(data=='@') flag--;
		if(flag<3){
			if(flag==2){
				if(adr_arr[0]==data) flag--; else flag=3;
			}else{
				 if(adr_arr[1]==data) flag--; else flag=3;
			}
		}
	}else{
		if(data=='&'){
			flag=3;
		}else{
			tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;	// inkremetacja licznika
			if ( tmp_head == UART_RxTail ) {
				// przepelnieie bufora
			} else {
				UART_RxHead = tmp_head; 		// zapis indeksu
				UART_RxBuf[tmp_head] = data; 	// zapis do bufora odbiorczego
			}
		}
	}
}

ISR( USART0_UDRE_vect)  {
	if ( UART_TxHead != UART_TxTail ) {						// sprawdzenei zawartosci buforu
		UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK; // przesuniecie indexu
		#if RS485 == 1
		rs485_nadawanie();	// ustaw tryb nadawania
		#endif
		UDR0 = UART_TxBuf[UART_TxTail];						// zapis bajta do buforu sprzetowego
	} else {
		UCSR0B &= ~(1<<UDRIE0);								// zerowanie flagi pustego sprzetowego bufora nadawczego
	}
}
