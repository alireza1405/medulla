#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "cpu.h"
#include "renishaw_ssi_encoder.h"

UART_USES_PORT(USARTD0)
SPI_USES_PORT(SPIC)

int main(void) {
	cpu_set_clock_source(cpu_32mhz_clock);
	cpu_configure_interrupt_level(cpu_interrupt_level_medium, true);
	cpu_configure_interrupt_level(cpu_interrupt_level_high, true);
	cpu_configure_interrupt_level(cpu_interrupt_level_low, true);
	sei();

	uint8_t outbuffer[128];
	uint8_t inbuffer[128];
	uart_port_t debug_port = uart_init_port(&PORTD, &USARTD0, uart_baud_115200, outbuffer, 128, inbuffer, 128);
	uart_connect_port(&debug_port, true);
	printf("Starting...\n");

	PORTC.DIRSET = 0b11;

	uint32_t encoder_data;
	uint16_t encoder_timestamp;
	renishaw_ssi_encoder_t encoder = renishaw_ssi_encoder_init(&PORTC,&SPIC,&TCC0,&encoder_data,&encoder_timestamp);

	while (1) {
		PORTC.OUTTGL = 0b1;
		renishaw_ssi_encoder_start_reading(&encoder);
		while (!renishaw_ssi_encoder_read_complete(&encoder));
		renishaw_ssi_encoder_process_data(&encoder);
		printf("%lu\n",encoder_data);
		PORTC.OUTTGL = 0b1;
		_delay_ms(100);
	}

	while(1);
	return 1;
}

