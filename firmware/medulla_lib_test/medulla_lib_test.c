#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "cpu.h"
#include "usart_adc.h"
#include "biss_encoder.h"

UART_USES_PORT(USARTE0)
USART_ADC_USES_PORT(USARTF0)
SPI_USES_PORT(SPIC)
SPI_USES_PORT(SPID)

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
	uint32_t encoder;
	uint32_t encoder2;
	uint16_t time;
	uint16_t time2;
	biss_encoder_t motor_encoder = biss_encoder_init(&PORTC,&SPIC,&TCC0,32,&encoder,&time);
	biss_encoder_t leg_encoder = biss_encoder_init(&PORTD,&SPID,&TCC0,32,&encoder2,&time2);
	PORTC.DIRSET = 0b11;

	uint16_t ch1, ch2, ch3, ch4;

	usart_adc_t adc = usart_adc_init(&PORTF, &USARTF0, io_init_pin(&PORTD,4), &ch1, &ch2, &ch3, &ch4);

	while (1) {
		usart_adc_start_read(&adc);
		biss_encoder_start_reading(&motor_encoder);
		biss_encoder_start_reading(&leg_encoder);

		while (!usart_adc_read_complete(&adc));
		while (!biss_encoder_read_complete(&motor_encoder));
		while (!biss_encoder_read_complete(&leg_encoder));
		usart_adc_process_data(&adc);
		printf("%04d\n",ch3);
		_delay_ms(100);
	}

	while(1);
	return 1;
}

