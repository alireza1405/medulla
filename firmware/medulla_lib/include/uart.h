#ifndef UART_H
#define UART_H

/// This library provides a RS232 driver through the USART hardware on the xMega.
/** The UART library implements the RS232 serial protocol over the USART
 *  hardware provided in the xmega. This driver can be used as a general purpose
 *  serial port, or it can be connected the printf to provide printf style
 *  output from the xMega.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>

/// Possibe baud rates. The values of this enum are 16 bit values that relate to the
/// baude rate registers.
typedef enum {
	uart_baud_9600 = 207,
	uart_baud_19200 = 103,
	uart_baud_57600 = 34,
	uart_baud_115200 = 0xF021,
	uart_baud_203400 = 0xE01F,
	uart_baud_460800 = 0xD01B,
	uart_baud_500000 = 0x1001,
	uart_baud_921600 = 0xC013,
	uart_baud_1M = 1
} uart_baud_t;

/// This struct defines all the information about a uart port. It is used by the user
/// to keep track of a port.
typedef struct {
	PORT_t *uart_port;
	USART_t *uart_register;
	uart_baud_t baud_rate;
	uint8_t *tx_buffer;
	uint8_t tx_buffer_size;
	uint8_t	*rx_buffer;
	uint8_t rx_buffer_size;
} uart_port_t;

/// This struct stores information about the buffers for a currently attached serial buffer
typedef struct {
	uint8_t *tx_buffer;
	uint8_t tx_buffer_size;
	uint8_t tx_buffer_start;
	uint8_t tx_buffer_end;
	uint8_t *rx_buffer;
	uint8_t rx_buffer_size;
	uint8_t rx_buffer_start;
	uint8_t rx_buffer_end;
	uart_port_t *current_port;
} uart_buffer_t;

/// Buffer information structs for each possible serial port
uart_buffer_t _uart_buffer_c0, _uart_buffer_c1, _uart_buffer_d0, _uart_buffer_d1, _uart_buffer_e0, _uart_buffer_e1, _uart_buffer_f0, _uart_buffer_f1;


#define _UART_TX_HANDLER(_UART_REG, _UART_BUFFER) \
	if (_UART_BUFFER.tx_buffer_start != _UART_BUFFER.tx_buffer_end) { \
                /* There is actually data to send, so increment the start position and send that byte. Make sure to handle wrap arounds*/ \
                _UART_REG.DATA = _UART_BUFFER.tx_buffer[_UART_BUFFER.tx_buffer_start];      /* write the data into the output buffer */ \
                _UART_BUFFER.tx_buffer_start = (_UART_BUFFER.tx_buffer_start+1)%_UART_BUFFER.tx_buffer_size; \
        }

#define _UART_RX_HANDLER(_UART_REG, _UART_BUFFER) \
	/* to check if there is room in the buffer is a little tricky, because we need to know when end is one less than start. However at the wrap around, this causes problems*/ \
        if (((_UART_BUFFER.rx_buffer_start != 0) && (_UART_BUFFER.rx_buffer_end != _UART_BUFFER.rx_buffer_start-1)) \
           || ((_UART_BUFFER.rx_buffer_start == 0) && (_UART_BUFFER.rx_buffer_end != _UART_BUFFER.rx_buffer_size-1))) { \
                /* if there is room in the rx buffer, then put it in */ \
                _UART_BUFFER.rx_buffer[_UART_BUFFER.rx_buffer_end] = _UART_REG.DATA; \
                _UART_BUFFER.rx_buffer_end = ((_UART_BUFFER.rx_buffer_end+1) % _UART_BUFFER.rx_buffer_size); \
        } \
        else \
                /* Even if the buffer is full, we have to read the data out, otherwise the xmega will never clear the interrupt flag, and thus the interrupt will keep getting called*/ \
                _UART_REG.DATA;

#ifdef UART_USES_C0
ISR(USARTC0_TXC_vect) {
	_UART_TX_HANDLER(USARTC0,_uart_buffer_c0);
}

ISR(USARTC0_RXC_vect) {
	_UART_RX_HANDLER(USARTC0,_uart_buffer_c0);
}
#endif

#ifdef UART_USES_C1
ISR(USARTC1_TXC_vect) {
        _UART_TX_HANDLER(USARTC1,_uart_buffer_c1);
}

ISR(USARTC1_RXC_vect) {
	_UART_RX_HANDLER(USARTC1,_uart_buffer_c1);
}
#endif

#ifdef UART_USES_D0
ISR(USARTD0_TXC_vect) {
        _UART_TX_HANDLER(USARTD0,_uart_buffer_d0);
}

ISR(USARTCD_RXC_vect) {
	_UART_RX_HANDLER(USARTD0,_uart_buffer_d0);
}
#endif

#ifdef UART_USES_D1
ISR(USARTD1_TXC_vect) {
        _UART_TX_HANDLER(USARTD1,_uart_buffer_d1);
}

ISR(USARTD1_RXC_vect) {
	_UART_RX_HANDLER(USARTD1,_uart_buffer_d1);
}
#endif

#ifdef UART_USES_E0
ISR(USARTE0_TXC_vect) {
        _UART_TX_HANDLER(USARTE0,_uart_buffer_e0);
}

ISR(USARTE0_RXC_vect) {
	_UART_RX_HANDLER(USARTE0,_uart_buffer_e0);
}
#endif

#ifdef UART_USES_E1
ISR(USARTE1_TXC_vect) {
        _UART_TX_HANDLER(USARTE1,_uart_buffer_e1);
}

ISR(USARTE1_RXC_vect) {
	_UART_RX_HANDLER(USARTE1,_uart_buffer_e1);
}
#endif

#ifdef UART_USES_F0
ISR(USARTF0_TXC_vect) {
        _UART_TX_HANDLER(USARTF0,_uart_buffer_f0);
}

ISR(USARTF0_RXC_vect) {
	_UART_RX_HANDLER(USARTF0,_uart_buffer_f0);
}
#endif

#ifdef UART_USES_F1
ISR(USARTF1_TXC_vect) {
        _UART_TX_HANDLER(USARTF1,_uart_buffer_f1);
}

ISR(USARTF1_RXC_vect) {
	_UART_RX_HANDLER(USARTF1,_uart_buffer_f1);
}
#endif

/// Initilize serial port, it still needs to be connected to the interrupt
uart_port_t uart_init_port(PORT_t *port_reg, USART_t *uart_reg, uart_baud_t baud_rate, void *tx_buffer, uint8_t tx_buffer_length, void *rx_buffer, uint8_t rx_buffer_length);

/// This function connects a serial port to the usart interrupt so it can send and receive data.
/// Returns -1 if the hardware is already being used. Returns 0 on success.
int uart_connect_port(uart_port_t *port, bool use_for_stdio);

/// This function disconnects a uart port from the hardware interrupts
/// Returns -1 if the port wasn't connected in the first place, 0 on sucess
int uart_disconnect_port(uart_port_t *port);

/// Copy data from a buffer into the tx buffer for transmit.
int uart_tx_data(uart_port_t *port, void *data, uint8_t data_length);

/// Put a single buffer into the tx buffer
int uart_tx_byte(uart_port_t *port, uint8_t data);

/// Get an arbitrary number of bytes out of the rx buffer. If there is not that much data in the
/// buffer, then the number of bytes received is returned.
int uart_rx_data(uart_port_t *port, void *data, uint8_t data_length);

/// Get a byte from the rx buffer
uint8_t uart_rx_byte(uart_port_t *port);

#endif //UART_H
