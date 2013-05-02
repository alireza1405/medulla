#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "cpu.h"
#include "io_pin.h"

UART_USES_PORT(USARTE0)

volatile uint8_t result_buffer[4];
volatile uint8_t position;
/*
ISR(USARTF0_TXC_vect) {
	PORTC.OUTSET = 1;
	if (tx_position < 5)
		USARTF0.DATA = 0;
	tx_position++;
	PORTC.OUTCLR = 1;
}
*/
ISR(USARTF0_RXC_vect) {
	if (position > 0)
		result_buffer[5-position] = USARTF0.DATA;
	if (position < 4)
		USARTF0.DATA=0;
	position++;
}

int32_t read_reg(uint8_t reg)
{
	int32_t result;
	USARTF0.DATA = 1<<6 | ((reg & 0b111)<<3);
	_delay_us(7);
	while (!(USARTF0.STATUS & 1<<7));
	result = USARTF0.DATA;
	USARTF0.DATA = 0;
	_delay_us(7);
	while (!(USARTF0.STATUS & 1<<7));
	result = USARTF0.DATA;
	USARTF0.DATA = 0;
	_delay_us(7);
	while (!(USARTF0.STATUS & 1<<7));
	result = result<<8 | USARTF0.DATA;
	USARTF0.DATA = 0;
	_delay_us(7);
	while (!(USARTF0.STATUS & 1<<7));
	result = result<<8 | USARTF0.DATA;
	return result;
}

void write_reg(uint8_t reg,uint32_t value)
{
	uint8_t data;
	USARTF0.DATA = ((reg & 0b111)<<3);
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	data = USARTF0.DATA;
	_delay_us(100);
	USARTF0.DATA = (uint8_t)((value>>16) & 0xFF);
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	data = USARTF0.DATA;
	_delay_us(100);
	USARTF0.DATA = (uint8_t)((value>>8) & 0xFF);
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	data = USARTF0.DATA;
	_delay_us(100);
	USARTF0.DATA = (uint8_t)(value & 0xFF);
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	data = USARTF0.DATA;
}

int main(void) {
	cpu_set_clock_source(cpu_32mhz_clock);
	cpu_configure_interrupt_level(cpu_interrupt_level_medium, true);
	//cpu_configure_interrupt_level(cpu_interrupt_level_high, true);
	//cpu_configure_interrupt_level(cpu_interrupt_level_low, true);
	sei();
	PORTC.DIRSET = 1;
	uint8_t outbuffer[128];
	uint8_t inbuffer[128];
	uart_port_t debug_port = uart_init_port(&PORTE, &USARTE0, uart_baud_115200, outbuffer, 128, inbuffer, 128);
	uart_connect_port(&debug_port, true);
	printf("Starting...\n");

	PORTF.DIRSET = 1<<1 | 1<<3;
	PORTF.DIRCLR = 1<<2;

	// now setup the usart for SPI mode
	USARTF0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	USARTF0.CTRLC = USART_CMODE_MSPI_gc | 1<<1;
	PORTF.PIN1CTRL |= 1<<6;
	USARTF0.BAUDCTRLA = 3;

	_delay_ms(1000);
	int16_t adc;

	int data;
	for (int i = 0; i<5; i++) {
		USARTF0.DATA = 0xFF;
		_delay_us(1);
		while (!(USARTF0.STATUS & 1<<7));
		_delay_us(100);
	}

	_delay_ms(100);

	// Configure ADC register
	write_reg(2,0x4);
	write_reg(1,0x080004);

	USARTF0.CTRLA = USART_RXCINTLVL_MED_gc;
	while (1) {
		position = 0;
		USARTF0.DATA = 1<<6 | ((3 & 0b111)<<3);
		while (position < 5);
		adc = *((int16_t*)(&result_buffer[2]));
		adc -= (int16_t)(0x8000);
		printf("%d\n",adc);
		_delay_ms(1);
	}

	while(1);
	return 1;
}

