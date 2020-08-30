/*
 * uart328pb.h
 *
 * Created: 30.08.2020 20:14:08
 * Author : Piotr Michna
 * e-mail : pm@piotrmichna.pl
 */ 


#ifndef UART328PB_H_
#define UART328PB_H_

#define RS485 1

#define UART_BAUD 115200									// predkosc
#define __UBRR ((F_CPU+UART_BAUD*8UL) / (16UL*UART_BAUD)-1) // UBRR dla U2X=0


#if RS485 == 1
#define RS_TXEN_PIN PD2
#define RS_TXEN_PORT PORTD
#define RS_TXEN_DDR DDRD

//#define RS_RXEN_PIN PD3
#ifdef RS_RXEN_PIN
#define RS_RXEN_PORT PORTD
#define RS_RXEN_DDR DDRD
#endif
#endif

#define UART_RX_BUF_SIZE 16									// rozmiar bufora odbiorczego
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)			// maska bufora

#define UART_TX_BUF_SIZE 16									// rozmiar bufora nadawczego
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)			// maska bufora

void USART_Init( uint16_t baud );
char uart_getc(void);
void uart_putc( char data );
void uart_puts(char *str);
void uart_putint(int value, int radix);
void uart_clear(void);

#endif /* UART328PB_H_ */