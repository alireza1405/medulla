#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "cpu.h"
#include "io_pin.h"
#include "ad7193.h" 

UART_USES_PORT(USARTE0)
AD7193_USES_PORT(USARTF0)

typedef struct {
	int *d;
} structo;

volatile structo e;
structo* f;

volatile int a;
volatile int *b;

ISR(TCC0_OVF_vect) {
	PORTC.OUT = 1;
//	(a)++;
	e.d = &a;
	PORTC.OUT = 0;
	PORTC.OUT = 1;
//	(*b)++;
	(*f).d += 1;
	PORTC.OUT = 0;
	PORTC.OUT = 1;
//	(a)++;
	e.d += 1;
	PORTC.OUT = 0;
	PORTC.OUT = 1;
//	test(b);
	(*f).d += 1;
	PORTC.OUT = 0;
	PORTC.OUT = 1;
//	(*b)++;
	(*f).d += 1;
	PORTC.OUTTGL = 1;
}

void test(int *c) {
	(*c) = 0;
}

int16_t adc_value;
int16_t *adc_pntr = &adc_value;
ad7193_t adc;
int main(void) {
	cpu_set_clock_source(cpu_32mhz_clock);
	cpu_configure_interrupt_level(cpu_interrupt_level_medium, true);
	cpu_configure_interrupt_level(cpu_interrupt_level_high, true);
	cpu_configure_interrupt_level(cpu_interrupt_level_low, true);
	sei();

	b = &a;
	f = &e;
	PORTC.DIR = 1;
	TCC0.CTRLA = 1;
	TCC0.INTCTRLA = 0b10;
	while(1);

	PORTC.DIRSET = 0b111;
	uint8_t outbuffer[128];
	uint8_t inbuffer[128];
	uart_port_t debug_port = uart_init_port(&PORTE, &USARTE0, uart_baud_115200, outbuffer, 128, inbuffer, 128);
	uart_connect_port(&debug_port, true);
	printf("Starting...\n");

	PORTD.DIRSET = 0b10000;
	PORTD.OUTCLR = 1<<4;
	_delay_us(5);
	adc = ad7193_init(&PORTF,&USARTF0,adc_pntr);
	_delay_us(100);
	PORTD.OUTSET = 1<<4;
	printf("Initilized...\n");
	while (1) {
		PORTD.OUTCLR = 1<<4;
		_delay_us(5);
		ad7193_start_read(&adc);
		while (!ad7193_read_complete(&adc));
		ad7193_process_data(&adc);

		PORTD.OUTSET = 1<<4;
		printf("%d\n",*adc_pntr);
		_delay_ms(10);
	}

	while(1);
	return 1;
}

