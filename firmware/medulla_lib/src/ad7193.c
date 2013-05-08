#include "ad7193.h"

ad7193_t adc7193_init(PORT_t *usart_port, USART_t *usart_reg, uint16_t *destination) {
	// First set up our adc struct
	ad7193_t adc;
	adc.usart_port = usart_port;
	adc.usart_reg = usart_reg;
	adc.destination = destination;
	adc.currently_reading = false;

	// now setup the usart for SPI mode
	adc.usart_reg->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	adc.usart_reg->CTRLC = USART_CMODE_MSPI_gc | 1<<1;
	adc.usart_port->PIN1CTRL |= 1<<6;
	adc.usart_reg->BAUDCTRLA = 3;

	// setup the transmit buffer so we can configure the ADC
	_adc124_buffer_t *buffer;

	switch((intptr_t)usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_ad7193_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_ad7193_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_ad7193_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_ad7193_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_ad7193_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_ad7193_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_ad7193_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_ad7193_USARTF1; break;
		default: buffer = 0;
	}
	
	// Load buffer with reset command
	buffer->buffer[0] = 0xFF;
	buffer->buffer[1] = 0xFF;
	buffer->buffer[2] = 0xFF;
	buffer->buffer[3] = 0xFF;
	buffer->buffer_position = 0;
	adc->usart_reg.PORT = 0xFF; // Start transmitting
	while (buffer->buffer_position < 4); // wait to finish transmitting

	// Configure ADC register
	write_reg(2,0x4);
	write_reg(1,0x080004);

	USARTF0.CTRLA = USART_RXCINTLVL_MED_gc;
}

bool ad7193_start_read(ad7193_t *adc) {
	if (adc->currently_reading == false) {
		// Start reading
		adc->currently_reading = true
		adc->usart_reg->DATA = 1<<6 | ((3 & 0b111)<<3);
	}
	else
		return false; // Trying to start reading while read is active, fail.	
}

inline bool ad7193_read_complete(ad7193_t *adc) {
	return adc->currently_reading
}

void ad7193_process_data(ad7193_t *adc) {
		*(adc->destination) = *((int16_t*)(&result_buffer[2]));
		*(adc->destination) -= (int16_t)(0x8000);
}

