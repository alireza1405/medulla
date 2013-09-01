#include "adc124.h"

adc124_t adc124_init(PORT_t *usart_port, USART_t *usart_reg, io_pin_t CS_pin, uint16_t *ch0_dest,uint16_t *ch1_dest,uint16_t *ch2_dest,uint16_t *ch3_dest) {
	PORTC.DIRSET = 1;
	// setup the adc struct
	adc124_t adc;
	adc.usart_port = usart_port;
	adc.usart_reg = usart_reg;
	adc.ch0_destination = ch0_dest;
	adc.ch1_destination = ch1_dest;
	adc.ch2_destination = ch2_dest;
	adc.ch3_destination = ch3_dest;
	adc.currently_reading = false;
	usart_port->DIRSET = 1<<1 | 1<<3; 
	usart_port->DIRCLR = 1<<2;
	// now setup the usart for SPI mode
	usart_reg->CTRLA = USART_TXCINTLVL_MED_gc | USART_RXCINTLVL_MED_gc;
	usart_reg->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	usart_reg->CTRLC = USART_CMODE_MSPI_gc;
	usart_reg->BAUDCTRLA = 4; // run it at 3.2 Mhz (max for ADC124)







	/* Correct for frame bit offset */
	uint8_t pmic_tmp = PMIC.CTRL;
	PMIC.CTRL &= ~PMIC_MEDLVLEN_bm; // disable interrupt handling for a bit.
	uint16_t sample = 0;
	int i;
	for(i = 0; i < 4; ++i ) {
		usart_reg->DATA = 0xFF;
		while( ~usart_reg->STATUS & USART_RXCIF_bm );
		//Read from SPI
		sample |= (uint16_t)usart_reg->DATA << 8;
		usart_reg->DATA = 0xFF;
		while( ~usart_reg->STATUS & USART_RXCIF_bm );
		//Read from SPI
		sample |= usart_reg->DATA;
	}
	int8_t rotate = 24; // This should be 16, but then ends up off by 8 after the first read. idk.
	while( (sample & 0xF800) != 0x0800 ) {
		sample = sample >> 1 | ( (sample & 1) ? 1<<15 : 0 ); //rotate sample to the right.
		--rotate;
		if( rotate <= 0 ) {
			break;	//reading strain gauge failed - don't stop forever
		}
	}
	adc124_process_data(&adc);	// Somehow this clears the bad first read
	rotate %= 16;
	// Disable SPI so we can bit-bang the clock.
	uint8_t b_tmp = usart_reg->CTRLB;
	uint8_t c_tmp = usart_reg->CTRLC;
	usart_reg->CTRLB = 0;
	usart_reg->CTRLC = 0;
	
	// Fix the frame offset
	for(i = 0; i < rotate; ++i ) {
		PORTF.OUTSET = 2;
		_delay_us(0.2);
		PORTF.OUTCLR = 2;
		_delay_us(0.2);
	}
	// Now re-eanble SPI
	usart_reg->CTRLB = b_tmp;
	usart_reg->CTRLC = c_tmp;
	PMIC.CTRL = pmic_tmp;
	/* Done correcting frame bit offset */










#ifdef USING_ADC124_CHIP_SELECT
	// Setup the chip select pin
	adc.CS_pin = CS_pin;
	adc.CS_pin.io_port->DIRSET = 1<<adc.CS_pin.pin;
	adc.CS_pin.io_port->OUTSET = 1<<adc.CS_pin.pin;
#endif

	// setup the transmit buffer
	_adc124_buffer_t *buffer;

	switch((intptr_t)usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_adc124_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_adc124_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_adc124_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_adc124_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_adc124_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_adc124_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_adc124_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_adc124_USARTF1; break;
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

void adc124_start_read(adc124_t *adc) {
	if (adc->currently_reading) {
		return;
	}

	adc->currently_reading = true;
	_adc124_buffer_t *buffer;

	switch((intptr_t)adc->usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_adc124_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_adc124_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_adc124_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_adc124_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_adc124_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_adc124_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_adc124_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_adc124_USARTF1; break;
		default: buffer = 0;
	}

#ifdef USING_ADC124_CHIP_SELECT
	adc->CS_pin.io_port->OUTCLR = 1<<adc->CS_pin.pin;
#endif

	buffer->adc_pntr = adc;

	buffer->tx_buffer_position = 0;
	buffer->rx_buffer_position = 0;
	adc->usart_reg->DATA = buffer->tx_buffer[0];
}

bool adc124_read_complete(adc124_t *adc) {
	return !adc->currently_reading;
}

void adc124_process_data(adc124_t *adc) {
	_adc124_buffer_t *buffer;

	switch((intptr_t)adc->usart_reg) {
		case ((intptr_t)&USARTC0): buffer = &_adc124_USARTC0; break;
		case ((intptr_t)&USARTC1): buffer = &_adc124_USARTC1; break;
		case ((intptr_t)&USARTD0): buffer = &_adc124_USARTD0; break;
		case ((intptr_t)&USARTD1): buffer = &_adc124_USARTD1; break;
		case ((intptr_t)&USARTE0): buffer = &_adc124_USARTE0; break;
		case ((intptr_t)&USARTE1): buffer = &_adc124_USARTE1; break;
		case ((intptr_t)&USARTF0): buffer = &_adc124_USARTF0; break;
		case ((intptr_t)&USARTF1): buffer = &_adc124_USARTF1; break;
		default: buffer = 0;	// defaults to a seg fault???
	}

	// We get a 12-bit value, but I'm not masking it so we can tell if the SPI frame isn't lined up
	*(adc->ch0_destination) = (((uint16_t)(buffer->rx_buffer[0]))<<8 | buffer->rx_buffer[1]);
	*(adc->ch1_destination) = (((uint16_t)(buffer->rx_buffer[2]))<<8 | buffer->rx_buffer[3]);
	*(adc->ch2_destination) = (((uint16_t)(buffer->rx_buffer[4]))<<8 | buffer->rx_buffer[5]);
	*(adc->ch3_destination) = (((uint16_t)(buffer->rx_buffer[6]))<<8 | buffer->rx_buffer[7]);
}
