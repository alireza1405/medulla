#include <avr/io.h>
#include <util/delay.h>

#define UART_USES_E0

#include "uart.h"
#include "cpu.h"

int main(void) {
	cpu_set_clock_source(cpu_32mhz_clock);
	PORTC.DIRSET = 0b11;
	PMIC.CTRL = PMIC_MEDLVLEN_bm;
	sei();

	uint8_t outbuffer[128];
	uint8_t inbuffer[128];
	uart_port_t debug_port = uart_init_port(&PORTE, &USARTE0, uart_baud_115200, outbuffer, 128, inbuffer, 128);
	uart_connect_port(&debug_port, false);

	uint8_t in[128];

	while (1) {
		uart_tx_data(&debug_port,in,uart_rx_data(&debug_port,in,128));
	}

	while(1);
	return 1;
}

