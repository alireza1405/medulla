#ifndef AD7193_ADC_H
#define AD7193_ADC_H

/** @file
 *  @brief Driver for controlling the AD7193 SPI ADC using one of the usart ports 
 *  on the xmega.
 *
 *  Driver for controlling the AD7193 SPI ADC using one of the usart ports on the 
 *  xmega. Currently the driver only supports reading a single usart at a time since
 *  this is the only way only way to achieve 1Khz update speed.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

typedef struct {
	PORT_t *usart_port;
	USART_t *usart_reg;
	uint16_t *destination;
	volatile bool currently_reading;
} ad7193_t;

typedef struct {
	uint8_t buffer[4];
	volatile uint8_t buffer_position;
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
ISR(USART_PORT##_RXC_vect) { \
	if (_ad7193_##USART_PORT.buffer_position > 0)  \
		_ad7193_##USART_PORT.buffer[5 - _ad7193_##USART_PORT.buffer_position] = USART_PORT.DATA; \
	if (_ad7193_##USART_PORT.buffer_position < 4)  \
		USART_PORT.DATA = 0; \
} \

ad7193_t ad7193_init(PORT_t *usart_port, USART_t *usart_reg, uint16_t *destination);

void ad7193_start_read(ad7193_t *usart_adc);

bool ad7193_read_complete(ad7193_t *usart_adc);

void ad7193_process_data(ad7193_t *usart_adc);

#endif //AD7193_H
