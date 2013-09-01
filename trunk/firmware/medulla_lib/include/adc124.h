#ifndef ADC124_ADC_H
#define ADC124_ADC_H

/** @file
 *  @brief Driver for controling the ADC124 serial ADC using the USART port on the xmega
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "io_pin.h"

typedef struct {
	PORT_t *usart_port;
	USART_t *usart_reg;
	io_pin_t CS_pin;
	uint16_t *ch0_destination;
	uint16_t *ch1_destination;
	uint16_t *ch2_destination;
	uint16_t *ch3_destination;
	volatile bool currently_reading;
} adc124_t;

typedef struct {
	uint8_t tx_buffer[8];
	volatile uint8_t rx_buffer[8];
	volatile uint8_t tx_buffer_position;
	volatile uint8_t rx_buffer_position;
	adc124_t *adc_pntr;
} _adc124_buffer_t;

_adc124_buffer_t _adc124_USARTC0,
                 _adc124_USARTC1,
                 _adc124_USARTD0,
                 _adc124_USARTD1,
                 _adc124_USARTE0,
                 _adc124_USARTE1,
                 _adc124_USARTF0,
                 _adc124_USARTF1;

#define ADC124_USES_PORT(USART_PORT) \
ISR(USART_PORT##_TXC_vect) { \
	if (_adc124_##USART_PORT.tx_buffer_position < 7) { \
		USART_PORT.DATA = _adc124_##USART_PORT.tx_buffer[++(_adc124_##USART_PORT.tx_buffer_position)]; \
	} \
} \
\
ISR(USART_PORT##_RXC_vect) { \
	_adc124_##USART_PORT.rx_buffer[_adc124_##USART_PORT.rx_buffer_position++] = USART_PORT.DATA; \
	if (_adc124_##USART_PORT.rx_buffer_position > 7) { \
		(_adc124_##USART_PORT.adc_pntr)->currently_reading = false; \
		_adc124_##USART_PORT.adc_pntr->CS_pin.io_port->OUTSET = 1<<_adc124_##USART_PORT.adc_pntr->CS_pin.pin; \
	} \
}\

adc124_t adc124_init(PORT_t *usart_port, USART_t *usart_reg, io_pin_t CS_pin, uint16_t *ch0_dest,uint16_t *ch1_dest,uint16_t *ch2_dest,uint16_t *ch3_dest);

void adc124_start_read(adc124_t *usart_adc);

bool adc124_read_complete(adc124_t *usart_adc);

void adc124_process_data(adc124_t *usart_adc);

#endif //ADC124_H
