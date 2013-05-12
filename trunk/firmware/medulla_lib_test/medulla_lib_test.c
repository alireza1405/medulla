#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "cpu.h"
#include "io_pin.h"
#include "ad7193.h" 

UART_USES_PORT(USARTE0)
AD7193_USES_PORT(USARTF0)

int main(void) {
	cpu_set_clock_source(cpu_32mhz_clock);
	cpu_configure_interrupt_level(cpu_interrupt_level_medium, true);
	//cpu_configure_interrupt_level(cpu_interrupt_level_high, true);
	//cpu_configure_interrupt_level(cpu_interrupt_level_low, true);
	sei();
	PORTC.DIRSET = 0b111;
	uint8_t outbuffer[128];
	uint8_t inbuffer[128];
	uart_port_t debug_port = uart_init_port(&PORTE, &USARTE0, uart_baud_115200, outbuffer, 128, inbuffer, 128);
	uart_connect_port(&debug_port, true);
	printf("Starting...\n");

	int16_t adc_value;
	ad7193_t adc = ad7193_init(&PORTF,&USARTF0,&adc_value);

	while (1) {
		ad7193_start_read(&adc);
		while (!ad7193_read_complete(&adc));
		ad7193_process_data(&adc);
		printf("%d\n",adc_value);
		_delay_ms(1);
	}

	while(1);
	return 1;
}

