#include "spi.h"

#define _SPI_HANDLE_INTERRUPT(spi_port) \
	/* Just finished transmitting a byte, so increment buffer location */ \
	spi_port->io_buffer_position++; \
	/* we just read in some data, so we need to get it out of the input register */ \
	if (spi_port->io_buffer_position < spi_port->rx_buffer_size) \
		spi_port->rx_buffer[spi_port->io_buffer_position] = spi_port->spi_register->DATA; \
	\
	/* If we still need to send or receive data, then start the data transfer */ \
	if (spi_port->io_buffer_position < spi_port->tx_buffer_size){ \
		/* If there is data still to be transmitted, then write it to the output buffer */ \
		spi_port->spi_register->DATA = spi_port->tx_buffer[spi_port->io_buffer_position]; \
	} \
	else if (spi_port->io_buffer_position < spi_port->rx_buffer_size) { \
		/* If there is no data to be transmitted, but there is still data to be read, send zeros until all the data is in */ \
	       	spi_port->spi_register->DATA = 0; \
	} \
	else { \
		/* we are done with the transmission, clean up */ \
		spi_port->transaction_underway = false; \
		spi_port = 0; \
	} \

ISR(SPIC_INT_vect) {
	_SPI_HANDLE_INTERRUPT(_spi_port_c);
}

ISR(SPID_INT_vect) {
	_SPI_HANDLE_INTERRUPT(_spi_port_d);
}

ISR(SPIE_INT_vect) {
	_SPI_HANDLE_INTERRUPT(_spi_port_d);
}

ISR(SPIF_INT_vect) {
	_SPI_HANDLE_INTERRUPT(_spi_port_d);
}

spi_port_t spi_init_port(PORT_t *spi_port,
			SPI_t *spi_register,
			bool uses_chip_select,
			spi_callback_t io_complete_callback) {
	

	// Store values into the spi_port_t struct
	spi_port_t port;
	port.spi_port = spi_port;
	port.spi_register = spi_register;
	port.uses_chip_select = uses_chip_select;
	port.io_complete_callback = io_complete_callback;

	// Configure the IO directions for the SPI pins
	port.spi_port->DIRSET = (1<<SPI_MOSI) | (1<<SPI_CLK);
	port.spi_port->DIRCLR = 1<<SPI_MISO;
	port.spi_port->DIRSET = 1<<SPI_CS;

	// Configure the SPI registers
	port.spi_register->CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc;
	port.spi_register->INTCTRL = SPI_INTLVL_MED_gc;
	
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
	PORTC.OUTTGL = 1;
	// If this spi_port already has a transfer underway, then returrn -1
	if (spi_port->transaction_underway == true)
		return -1;

	// If the hardware already has a transfer underway, then return -2. Otherwise, take cntrol of the hardware
	if (spi_port->spi_register == &SPIC) {
		if (_spi_port_c != 0)
			return -2;
		else 
			_spi_port_c = spi_port;
	}
	if (spi_port->spi_register == &SPID) {
		if (_spi_port_d != 0)
			return -2;
		else 
			_spi_port_d = spi_port;
	}
	if (spi_port->spi_register == &SPIE) {
		if (_spi_port_e != 0)
			return -2;
		else
			_spi_port_e = spi_port;
	}
	if (spi_port->spi_register == &SPIF) {
		if (_spi_port_f != 0)
			return -2;
		else 
			_spi_port_f = spi_port;
	}

	// Signal that a transaction is now in progress
	spi_port->transaction_underway = true;

	// set output bufffer address and length
	spi_port->tx_buffer = tx_data;
	spi_port->tx_buffer_size = tx_data_length;
	
	// set the input buffer address and length
	spi_port->rx_buffer = rx_data;
	spi_port->rx_buffer_size = rx_data_length;

	// set buffer position to beginning
	spi_port->io_buffer_position = 0;
	
	// Set the data output register to start the transaction rolling. If there is no data to send, send a zero.
	if (spi_port->tx_buffer_size == 0)
		spi_port->spi_register->DATA = 0;
	else
		spi_port->spi_register->DATA = spi_port->tx_buffer[0];
}

