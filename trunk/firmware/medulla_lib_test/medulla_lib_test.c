#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "cpu.h"
#include "io_pin.h"

UART_USES_PORT(USARTE0)

int32_t read_reg(uint8_t reg)
{
	int32_t result;
	USARTF0.DATA = 1<<6 | ((reg & 0b111)<<3);
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	result = USARTF0.DATA;
	_delay_us(100);
	USARTF0.DATA = 0;
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	result = USARTF0.DATA;
	_delay_us(100);
	USARTF0.DATA = 0;
	_delay_us(1);
	while (!(USARTF0.STATUS & 1<<7));
	result = result<<8 | USARTF0.DATA;
	_delay_us(100);
	USARTF0.DATA = 0;
	_delay_us(1);
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
	cpu_configure_interrupt_level(cpu_interrupt_level_high, true);
	cpu_configure_interrupt_level(cpu_interrupt_level_low, true);
	sei();

	uint8_t outbuffer[128];
	uint8_t inbuffer[128];
	uart_port_t debug_port = uart_init_port(&PORTE, &USARTE0, uart_baud_115200, outbuffer, 128, inbuffer, 128);
	uart_connect_port(&debug_port, true);
	printf("Starting...\n");

	PORTF.DIRSET = 1<<1 | 1<<3;

	// now setup the usart for SPI mode
	//USARTF->CTRLA = USART_TXCINTLVL_MED_gc | USART_RXCINTLVL_MED_gc;
	USARTF0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	USARTF0.CTRLC = USART_CMODE_MSPI_gc | 1<<1;
	PORTF.PIN1CTRL |= 1<<6;
	USARTF0.BAUDCTRLA = 100;

	// Setup the chip select pin
	io_pin_t CS_pin = io_init_pin(&PORTD,4);
	io_set_direction(CS_pin,io_output);
	io_set_output(CS_pin,io_low);

	_delay_ms(1000);
	int32_t adc;

	// Configure ADC register
	write_reg(2,0x110);

	while (1) {
		// Start conversion
		write_reg(1,0x080001 | ((uint32_t)(0b001)<<21));
		// Wait for conversion ready
		while ((PORTF.IN & (1<<3)) == 0);
		adc = read_reg(3);
		adc -= (int32_t)(0x800000);
		printf("%ld\n",adc);
		_delay_ms(100);
	}

	while(1);
	return 1;
}

