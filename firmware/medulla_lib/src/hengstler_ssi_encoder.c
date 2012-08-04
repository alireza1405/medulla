#include "hengstler_ssi_encoder.h"

#define _SSI_ENCODER_SEND_CLOCK \
encoder->spi_port.spi_port->OUTCLR = (1<<7); \
_delay_us(0.125);        /* Half of the 2 Mhz period*/ \
encoder->spi_port.spi_port->OUTSET = (1<<7); \
_delay_us(0.125);        /* Other half of the 2 Mhz period*/

hengstler_ssi_encoder_t hengstler_ssi_encoder_init(PORT_t *spi_port, SPI_t *spi_register, void *timestamp_timer, uint32_t *data_pointer, uint16_t *timestamp_pointer) {

	hengstler_ssi_encoder_t encoder;

	// setup the encoder struct
	encoder.spi_port = spi_init_port(spi_port, spi_register, spi_div32, false);
	encoder.timestamp_timer = (TC0_t*)timestamp_timer;
	encoder.data_pointer = data_pointer;
	encoder.timestamp_pointer = timestamp_pointer;
	encoder.input_buffer[0] = 0;
	encoder.input_buffer[1] = 0;
	encoder.input_buffer[2] = 0;

	// The spi driver defaults to the wrong SPI mode, so we switch it now
	spi_register->CTRL = (spi_register->CTRL & ~SPI_MODE_gm) | SPI_MODE_2_gc;

	spi_port->DIRSET = 1<<7;
	spi_port->OUTSET = 1<<7;

	// Then just return the encoder
	return encoder;
}

int hengstler_ssi_encoder_start_reading(hengstler_ssi_encoder_t *encoder) {
	// first check that we are not already reading
	if (hengstler_ssi_encoder_read_complete(encoder) == false)
		return -1;

	// Check that the encoder is ready to be read from. MISO has be high
	if ((encoder->spi_port.spi_port->IN & (1<<6)) == 0)
		return -2;

	// clear out the data buffer
	encoder->input_buffer[0] = 0;
	encoder->input_buffer[0] = 0;
	encoder->input_buffer[0] = 0;

	encoder->spi_port.spi_register->CTRL &= ~SPI_ENABLE_bm;

	// First record the start time, and then send start bit
	cli();
	*(encoder->timestamp_pointer) = encoder->timestamp_timer->CNT;
	encoder->spi_port.spi_port->OUTCLR = 1<<7;
	sei();
	_delay_us(2);
	encoder->spi_port.spi_port->OUTSET = 1<<7;
	_delay_us(0.625);
	
	// Now we read in the 17th bit
	encoder->spi_port.spi_port->OUTCLR = (1<<7);
   	_delay_us(0.0625);		// sample the bit
	cli();
	encoder->spi_port.spi_port->OUTSET = (1<<7);
	encoder->input_buffer[0] |= ((encoder->spi_port.spi_port->IN & (1<<6)) >> (5));
	sei();
	_delay_us(0.0625);
	encoder->spi_port.spi_port->OUTCLR = (1<<7);
   	_delay_us(0.0625);		// sample the bit
	cli();
	encoder->spi_port.spi_port->OUTSET = (1<<7);
	encoder->input_buffer[0] |= ((encoder->spi_port.spi_port->IN & (1<<6)) >> (6));
	sei();
	_delay_us(0.0625);

	encoder->spi_port.spi_register->CTRL |= SPI_ENABLE_bm;
	spi_start_receive(&(encoder->spi_port), encoder->input_buffer + 1,2);
	// At this point the clock generator will run. When an Ack is received by the pin interrupt the ISR will start the SPI transfer
	return 0;
}

void hengstler_ssi_encoder_process_data(hengstler_ssi_encoder_t *encoder) {
	// Fill the data pointer. Since the xMega is little-endian, we have to swap the byte order, by shiffing the data in one byte at a time.
	*(encoder->data_pointer) = (((uint32_t)encoder->input_buffer[0]) << 15) |
	                           (((uint32_t)encoder->input_buffer[1]) <<  7) |
	                           (((uint32_t)encoder->input_buffer[2]) >> 1);

}

bool hengstler_ssi_encoder_read_complete(hengstler_ssi_encoder_t *encoder) {
	return !(encoder->spi_port.transaction_underway);
}


