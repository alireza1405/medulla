#include "spi.h"

#define _SPI_HANDLE_INTERRUPT(spi_buffer) \
	PORTC.OUTSET = 1; \
	/* Just finished transmitting a byte, so increment buffer location */ \
	spi_buffer.io_buffer_position++; \
	/* we just read in some data, so we need to get it out of the input register */ \
	if (spi_buffer.io_buffer_position < spi_buffer.rx_buffer_size) \
		spi_buffer.rx_buffer[spi_buffer.io_buffer_position] = _SPI.DATA; \
	\
	/* If we still need to send or receive data, then start the data transfer */ \
	if (spi_buffer.io_buffer_position < spi_buffer.tx_buffer_size) { \
		/* If there is data still to be transmitted, then write it to the output buffer */ \
		_SPI.DATA = spi_buffer.tx_buffer[spi_buffer.io_buffer_position]; \
	} \
	else if (spi_buffer.io_buffer_position < spi_buffer.rx_buffer_size) { \
		/* If there is no data to be transmitted, but there is still data to be read, send zeros until all the data is in */ \
	       	_SPI.DATA = 0; \
	} \
	else { \
		/* we are done with the transmission, clean up */ \
		spi_buffer.spi_port->transaction_underway = false; \
		spi_buffer.spi_port = 0; \
	} \
	PORTC.OUTCLR = 1;\
	reti(); \

ISR(SPIC_INT_vect, ISR_NAKED) {
	#define _SPI SPIC
	_SPI_HANDLE_INTERRUPT(_spi_buffer_c);
	#undef _SPI
}

ISR(SPID_INT_vect,ISR_NAKED) {
	#define _SPI SPIF
	_SPI_HANDLE_INTERRUPT(_spi_buffer_d);
	#undef _SPI
}

ISR(SPIE_INT_vect,ISR_NAKED) {
	#define _SPI SPIE
	_SPI_HANDLE_INTERRUPT(_spi_buffer_e);
	#undef _SPI
}

ISR(SPIF_INT_vect,ISR_NAKED) {
	#define _SPI SPIF
	_SPI_HANDLE_INTERRUPT(_spi_buffer_f);
	#undef _SPI
}

spi_port_t spi_init_port(PORT_t *spi_port, SPI_t *spi_register, bool uses_chip_select) {
	// Store values into the spi_port_t struct
	spi_port_t port;
	port.spi_port = spi_port;
	port.spi_register = spi_register;
	port.uses_chip_select = uses_chip_select;

	// Configure the IO directions for the SPI pins
	port.spi_port->DIRSET = (1<<SPI_MOSI) | (1<<SPI_CLK);
	port.spi_port->DIRCLR = 1<<SPI_MISO;
	port.spi_port->DIRSET = 1<<SPI_CS;
	port.transaction_underway = false;

	// Configure the SPI registers
	port.spi_register->CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc;// | SPI_CLK2X_bm;
	port.spi_register->INTCTRL = SPI_INTLVL_HI_gc;
	
	return port;
}

int spi_start_transmit(spi_port_t *spi_port, uint8_t *data, uint8_t data_length) {
	// To start just a transmit, all we need to do is start a transmit_receive transaction with the rx_data_length set to zero.
	return spi_start_transmit_receive(spi_port, data, data_length, 0, 0);
}

int spi_start_receive(spi_port_t *spi_port, uint8_t *data, uint8_t data_length) {
	// To start just a receive, we just need to start a transmit/receive transaction with a tx_data_length set to zero.
	return spi_start_transmit_receive(spi_port, 0, 0, data, data_length);
}

int spi_start_transmit_receive(spi_port_t *spi_port, uint8_t *tx_data, uint8_t tx_data_length, uint8_t *rx_data, uint8_t rx_data_length) {
	// If this spi_port already has a transfer underway, then returrn -1
	if (spi_port->transaction_underway == true)
		return -1;
	
	// If the hardware already has a transfer underway, then return -2. Otherwise, take cntrol of the hardware
	if (spi_port->spi_register == &SPIC) {
		if (_spi_buffer_c.spi_port != 0)
			return -2;
		else { 
			_spi_buffer_c.spi_port = spi_port;
			_spi_configure_buffers(&_spi_buffer_c, tx_data, tx_data_length, rx_data, rx_data_length);
		}
	}
	if (spi_port->spi_register == &SPID) {
		if (_spi_buffer_d.spi_port != 0)
			return -2;
		else {
			_spi_buffer_d.spi_port = spi_port;
			_spi_configure_buffers(&_spi_buffer_d, tx_data, tx_data_length, rx_data, rx_data_length);
		}
	}
	if (spi_port->spi_register == &SPIE) {
		if (_spi_buffer_e.spi_port != 0)
			return -2;
		else {
			_spi_buffer_e.spi_port = spi_port;
			_spi_configure_buffers(&_spi_buffer_e, tx_data, tx_data_length, rx_data, rx_data_length);
		}
	}
	if (spi_port->spi_register == &SPIF) {
		if (_spi_buffer_f.spi_port != 0)
			return -2;
		else { 
			_spi_buffer_f.spi_port = spi_port;
			_spi_configure_buffers(&_spi_buffer_f, tx_data, tx_data_length, rx_data, rx_data_length);
		}
	}
	

	// Signal that a transaction is now in progress
	spi_port->transaction_underway = true;
}

void _spi_configure_buffers(spi_buffer_t *spi_buffer, uint8_t *tx_data, uint8_t tx_data_length, uint8_t *rx_data, uint8_t rx_data_length) {

        // set output bufffer address and length
        spi_buffer->tx_buffer = tx_data;
        spi_buffer->tx_buffer_size = tx_data_length;
        
        // set the input buffer address and length
        spi_buffer->rx_buffer = rx_data;
        spi_buffer->rx_buffer_size = rx_data_length;

        // set buffer position to beginning
        spi_buffer->io_buffer_position = 0;
        
	// Set the data output register to start the transaction rolling. If there is no data to send, send a zero.       
        if (spi_buffer->tx_buffer_size == 0)
                spi_buffer->spi_port->spi_register->DATA = 0;
        else
                spi_buffer->spi_port->spi_register->DATA = spi_buffer->tx_buffer[0];

}

