#ifndef AD7193_ADC_H
#define AD7193_ADC_H

/** @file
 *  @brief Driver for controling the AD7193 serial ADC using the USART port on the xmega
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

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
} ad7193_t;

typedef struct {
	uint8_t tx_buffer[8];
	uint8_t rx_buffer[8];
	volatile uint8_t tx_buffer_position;
	volatile uint8_t rx_buffer_position;
	ad7193_t *adc_pntr;
} _ad7193_buffer_t;

_ad7193_buffer_t _usart_adc_USARTC0,
                    _ad7193_USARTC1,
                    _ad7193_USARTD0,
                    _ad7193_USARTD1,
                    _ad7193_USARTE0,
                    _ad7193_USARTE1,
                    _ad7193_USARTF0,
                    _ad7193_USARTF1;

#define AD7193_USES_PORT(USART_PORT) \
ISR(USART_PORT##_TXC_vect) { \
	if (_ad7193_##USART_PORT.tx_buffer_position < 7) { \
		USART_PORT.DATA = _ad7193_##USART_PORT.tx_buffer[++(_ad7193_##USART_PORT.tx_buffer_position)]; \
	} \
} \
\
ISR(USART_PORT##_RXC_vect) { \
	_ad7193_##USART_PORT.rx_buffer[_ad7193_##USART_PORT.rx_buffer_position++] = USART_PORT.DATA; \
	if (_ad7193_##USART_PORT.rx_buffer_position > 7) { \
		(_ad7193_##USART_PORT.adc_pntr)->currently_reading = false; \
		_ad7193_##USART_PORT.adc_pntr->CS_pin.io_port->OUTSET = 1<<_ad7193_##USART_PORT.adc_pntr->CS_pin.pin; \
	} \
}\

ad7193_t ad7193_init(PORT_t *usart_port, USART_t *usart_reg, io_pin_t CS_pin, uint16_t *ch0_dest,uint16_t *ch1_dest,uint16_t *ch2_dest,uint16_t *ch3_dest);

void ad7193_start_read(ad7193_t *usart_adc);

bool ad7193_read_complete(ad7193_t *usart_adc);

void ad7193_process_data(ad7193_t *usart_adc);

#endif //AD7193_H
