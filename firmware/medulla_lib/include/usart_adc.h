#ifndef USART_ADC_H
#define USART_ADC_H

/** @file
 *  @brief Driver for controling the ADC124 serial ADC using the USART port on the xmega
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

typedef struct {
	PORT_t *usart_port;
	USART_t *usart_reg;
	uint16_t *ch0_destination;
	uint16_t *ch1_destination;
	uint16_t *ch2_destination;
	uint16_t *ch3_destination;
	bool currently_reading;
} usart_adc_t;

typedef struct {
	uint8_t tx_buffer[8];
	uint8_t rx_buffer[8];
	uint8_t buffer_position;
	usart_adc_t *adc_pntr;
} _usart_adc_buffer_t;

_usart_adc_buffer_t _usart_adc_USARTC0,
                    _usart_adc_USARTC1,
                    _usart_adc_USARTD0,
                    _usart_adc_USARTD1,
                    _usart_adc_USARTE0,
                    _usart_adc_USARTE1,
                    _usart_adc_USARTF0,
                    _usart_adc_USARTF1;

#define USART_ADC_USES_PORT(USART_PORT) \
ISR(USARTF0_TXC_vect) { \
	PORTC.OUTSET = 1; \
	if (_usart_adc_##USART_PORT.buffer_position < 7) { \
		USART_PORT.DATA = _usart_adc_##USART_PORT.tx_buffer[_usart_adc_##USART_PORT.buffer_position+1]; \
	} \
	PORTC.OUTCLR = 1; \
} \
ISR(USARTF0_RXC_vect) { \
	PORTC.OUTSET = 0b10; \
	_usart_adc_##USART_PORT.rx_buffer[_usart_adc_##USART_PORT.buffer_position++] = USART_PORT.DATA; \
	if (_usart_adc_##USART_PORT.buffer_position > 7) \
		_usart_adc_##USART_PORT.adc_pntr->currently_reading = false; \
	PORTC.OUTCLR = 0b10; \
}\

usart_adc_t usart_adc_init(PORT_t *usart_port, USART_t *usart_reg,uint16_t *ch0_dest,uint16_t *ch1_dest,uint16_t *ch2_dest,uint16_t *ch3_dest);

void usart_adc_start_read(usart_adc_t *usart_adc);

bool usart_adc_read_complete(usart_adc_t *usart_adc);

void usart_adc_process_data(usart_adc_t *usart_adc);

#endif //USART_ADC_H
