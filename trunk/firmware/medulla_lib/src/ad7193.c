#include "ad7193.h"

#define CLK_PIN (1<<1)
#define MISO_PIN (1<<2)
#define MOSI_PIN (1<<3)

uint8_t _ad7193_shift_byte(uint8_t out) {
	uint8_t result = 0;
	uint8_t mask = 1<<7;
	for (int8_t count = 0; count < 8; count++) {
		PORTF.OUTCLR = CLK_PIN;
		if (mask & out)
			PORTF.OUTSET = MOSI_PIN;
		else
			PORTF.OUTCLR = MOSI_PIN;
		
		_delay_us(3);

		PORTF.OUTSET = CLK_PIN;
		if (PORTF.IN & MISO_PIN)
			result |= mask;
		
		_delay_us(3);
	}
	return result;
}

/** @brief Internal function to write a configration register in the ad7139
 */
bool _ad7193_write_reg(_ad7193_buffer_t *adc_buffer, uint8_t reg, uint32_t value)
{
	if (adc_buffer->adc_pntr->currently_reading)
		return false; // Can't do this, already reading;
	else {
		adc_buffer->adc_pntr->currently_reading = true;
		/*
		adc_buffer->buffer[0] = ((reg & 0b111)<<3);
		adc_buffer->buffer[1] = (uint8_t)((value>>16) & 0xFF);
		adc_buffer->buffer[2] = (uint8_t)((value>>8) & 0xFF);
		adc_buffer->buffer[3] = (uint8_t)(value & 0xFF);
		adc_buffer->buffer_position = 1;
		adc_buffer->adc_pntr->usart_reg->DATA = adc_buffer->buffer[0];
		while (adc_buffer->buffer_position < 4); // wait to finish transmitting
		*/
		_ad7193_shift_byte((reg & 0b111)<<3);
		_ad7193_shift_byte((uint8_t)((value>>16) & 0xFF));
		_ad7193_shift_byte((uint8_t)((value>>8) & 0xFF));
		_ad7193_shift_byte((uint8_t)(value & 0xFF));
		adc_buffer->adc_pntr->currently_reading = false;
	}
	return true;
}

/** @brief Internal function to reset the ad7139
 */
bool _ad7193_reset(_ad7193_buffer_t *adc_buffer)
{
	if (adc_buffer->adc_pntr->currently_reading)
		return false; // Can't do this, already reading;
	else {
		adc_buffer->adc_pntr->currently_reading = true;
		_ad7193_shift_byte(0xFF);
		/*adc_buffer->buffer[0] = 0xFF;
		adc_buffer->buffer[1] = 0xFF;
		adc_buffer->buffer[2] = 0xFF;
		adc_buffer->buffer[3] = 0xFF;
		adc_buffer->buffer_position = 0;
		adc_buffer->adc_pntr->usart_reg->DATA = 0xFF; // Start transmitting
		while (adc_buffer->buffer_position < 4); // wait to finish transmitting
		*/
		adc_buffer->adc_pntr->currently_reading = false;
		_delay_us(500); // You need to wait at least 500uS after resetting before you can do anything
	}
	return true;
}

ad7193_t ad7193_init(PORT_t *usart_port, USART_t *usart_reg, int16_t *destination) {
	// First set up our adc struct
	ad7193_t adc;
	adc.usart_port = usart_port;
	adc.usart_reg = usart_reg;
	adc.destination = destination;
	adc.currently_reading = false;

	//adc.usart_port->DIRSET = 1<<1 | 1<<3;
	//adc.usart_port->DIRCLR = 1<<2;

	// now setup the usart for SPI mode
	//adc.usart_reg->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	//adc.usart_reg->CTRLC = USART_CMODE_MSPI_gc | 1<<1;
	//adc.usart_port->PIN1CTRL |= 1<<6;
	//adc.usart_reg->BAUDCTRLA = 50;
	//adc.usart_reg->CTRLA = USART_TXCINTLVL_MED_gc;
	
	PORTF.OUTSET = MOSI_PIN;
	PORTF.OUTSET = CLK_PIN;
	PORTF.DIRSET = MOSI_PIN;
	PORTF.DIRSET = CLK_PIN;
	PORTF.DIRCLR = MISO_PIN;

	// setup the transmit buffer so we can configure the ADC
	_ad7193_buffer_t *buffer;

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
	buffer->adc_pntr = &adc;
	
	_ad7193_reset(buffer);
	// Configure ADC register
	_ad7193_write_reg(buffer,2,0x6);
	_ad7193_write_reg(buffer,1,0x0C0004);
	adc.usart_reg->CTRLA = USART_RXCINTLVL_MED_gc;

	return adc;
}

bool ad7193_start_read(ad7193_t *adc) {

	if (adc->currently_reading)
		return false; //Trying to start reading while read is already active, fail.
	
	adc->currently_reading = true;
	// Set up the transmission buffer
	_ad7193_buffer_t *buffer;
	switch((intptr_t)(adc->usart_reg)) {
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
	buffer->adc_pntr = adc;
	// Start reading
	buffer->buffer_position = 0;
//	adc->usart_reg->DATA = 1<<6 | ((3 & 0b111)<<3);
	_ad7193_shift_byte(1<<6 | ((3 & 0b111)<<3));
	buffer->buffer[2] =  _ad7193_shift_byte(0);
	buffer->buffer[1] =  _ad7193_shift_byte(0);
	buffer->buffer[0] =  _ad7193_shift_byte(0);
	adc->currently_reading = false;
	return true;
}

inline bool ad7193_read_complete(ad7193_t *adc) {
	return true;
	//return (_ad7193_USARTF0.buffer_position>4);
}

void ad7193_process_data(ad7193_t *adc) {
	if (adc->currently_reading)
		return; // If we are currently clocking in data, don't do anything

	_ad7193_buffer_t *buffer;
	switch((intptr_t)(adc->usart_reg)) {
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
	buffer->adc_pntr = adc;
	memcpy(adc->destination,buffer->buffer+1,2);
	*(adc->destination) -= (int16_t)(0x8000);
}

