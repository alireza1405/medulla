#include "usart_adc.h"

usart_adc_t usart_adc_init(PORT_t *usart_port, USART_t *usart_reg, io_pin_t CS_pin, uint16_t *ch0_dest,uint16_t *ch1_dest,uint16_t *ch2_dest,uint16_t *ch3_dest) {

	// setup the adc struct
	usart_adc_t adc;
	adc.usart_port = usart_port;
	adc.usart_reg = usart_reg;
	adc.ch0_destination = ch0_dest;
	adc.ch1_destination = ch1_dest;
	adc.ch2_destination = ch2_dest;
	adc.ch3_destination = ch3_dest;
	adc.currently_reading = false;

	usart_port->DIRSET = 1<<1 | 1<<3;

	// now setup the usart for SPI mode
	usart_reg->CTRLA = USART_TXCINTLVL_MED_gc | USART_RXCINTLVL_MED_gc;
	usart_reg->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	usart_reg->CTRLC = USART_CMODE_MSPI_gc;
	usart_reg->BAUDCTRLA = 127;

	// Setup the chip select pin
	adc.CS_pin = CS_pin;
	io_set_direction(CS_pin,io_output);
	io_set_output(CS_pin,io_high);

	// setup the transmit buffer
	_usart_adc_buffer_t *buffer;

	switch((intptr_t)usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_usart_adc_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_usart_adc_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_usart_adc_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_usart_adc_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_usart_adc_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_usart_adc_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_usart_adc_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_usart_adc_USARTF1; break;
		default: buffer = 0;
	}
	buffer->tx_buffer[0] = 1<<3;
	buffer->tx_buffer[1] = 0;
	buffer->tx_buffer[2] = 2<<3;
	buffer->tx_buffer[3] = 0;
	buffer->tx_buffer[4] = 3<<3;
	buffer->tx_buffer[5] = 0;
	buffer->tx_buffer[6] = 0<<3;
	buffer->tx_buffer[7] = 0;

	return adc;
}

void usart_adc_start_read(usart_adc_t *usart_adc) {
	if (usart_adc->currently_reading) {
		return;
	}

	usart_adc->currently_reading = true;
	_usart_adc_buffer_t *buffer;

	switch((intptr_t)usart_adc->usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_usart_adc_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_usart_adc_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_usart_adc_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_usart_adc_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_usart_adc_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_usart_adc_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_usart_adc_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_usart_adc_USARTF1; break;
		default: buffer = 0;
	}
	io_set_output(usart_adc->CS_pin,io_low);
	
	buffer->adc_pntr = usart_adc;

	buffer->tx_buffer_position = 0;
	buffer->rx_buffer_position = 0;
	usart_adc->usart_reg->DATA = buffer->tx_buffer[0];
}

bool usart_adc_read_complete(usart_adc_t *usart_adc) {
	return !usart_adc->currently_reading;
}

void usart_adc_process_data(usart_adc_t *usart_adc) {
	_usart_adc_buffer_t *buffer;

	switch((intptr_t)usart_adc->usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_usart_adc_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_usart_adc_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_usart_adc_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_usart_adc_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_usart_adc_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_usart_adc_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_usart_adc_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_usart_adc_USARTF1; break;
		default: buffer = 0;
	}

	*(usart_adc->ch0_destination) = (((uint16_t)(buffer->rx_buffer[0]))<<8 | buffer->rx_buffer[1]) & 0xFFF;
	*(usart_adc->ch1_destination) = (((uint16_t)(buffer->rx_buffer[2]))<<8 | buffer->rx_buffer[3]) & 0xFFF;
	*(usart_adc->ch2_destination) = (((uint16_t)(buffer->rx_buffer[4]))<<8 | buffer->rx_buffer[5]) & 0xFFF;
	*(usart_adc->ch3_destination) = (((uint16_t)(buffer->rx_buffer[6]))<<8 | buffer->rx_buffer[7]) & 0xFFF;
}

