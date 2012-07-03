#ifndef UART_H
#define UART_H

/** @file
 *  @brief This library provides a RS232 driver through the USART hardware on the xMega.
 * 
 *  The UART library implements the RS232 serial protocol over the USART
 *  hardware provided in the xmega. This driver can be used as a general purpose
 *  serial port, or it can be connected the stdin an stdout streams so the
 *  standard printf and scanf functions can be used with a uart port. The driver
 *  is fully buffered and uses interrupts to manage transmitting and receiving
 *  from a port's buffers.
 *
 *  This driver uses two circular buffers to store it's transmit and receive
 *  data. These buffers are supplied by the user when the port is initilized,
 *  this gives the user complete control over their memory. Once the transmit
 *  buffer is full, you cannot add anything more to it. If the receive buffer is
 *  full and a new byte is received, the oldest data in the buffer is thrown
 *  out.
 *
 *  @note
 *  It is important that the including program defines a UART_USES_XX
 *  keyword for any hardware USART ports that are used before including uart.h.
 *  If this is not done, the interrupt service routines will not be compiled.
 *  This allows the users or other drivers to use the USART interrupts on ports
 *  that are not being used.
 *
 *  @par
 *  @note
 *  This driver only supports buffers up to 256 bytes long.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>

/** @brief Possibe baud rates for a port.
 *
 *  This enumeration defines most of the standard baud rates used for UARTs. The
 *  values of the enumerators 16 bit integers which contain the value of the two
 *  baud rate registers for the USART system. This simplfies the configuration
 *  of the UART port.
 */
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

/** @brief Struct that defines a UART port.
 *
 *  This struct is used to define a port. It contains all the information nedded
 *  to configure and reference a port. All the function calls take a pointer to
 *  this struct to know which port to use. 
 */
typedef struct {
	PORT_t *uart_port;		/**< Pointer to the PORT_t register struct the port is on */
	USART_t *uart_register;		/**< Poiner to the USART_t register struct for the USART being used */
	uart_baud_t baud_rate;		/**< The port's configured baud rate */
	uint8_t *tx_buffer;		/**< Pointer to the transmit buffer */
	uint8_t tx_buffer_size;		/**< Size of the transmit buffer */
	uint8_t	*rx_buffer;		/**< Pointer to the receive buffer */
	uint8_t rx_buffer_size;		/**< Size of the receive buffer */
} uart_port_t;

/** @brief This struct defines buffer to be used by an ISR
 *
 *  To make the interrupt service rutines run as quickly as possible each
 *  hardware USART has a globally defined struct of this type. This allows the
 *  ISR to know everything about the UART port that is currently using it
 *  without having to dereference a pointer to it.
 */
typedef struct {
	uint8_t *tx_buffer;		/**< Pointer to the transmit buffer */
	uint8_t tx_buffer_size;		/**< Size of the transmit buffer */
	uint8_t tx_buffer_start;	/**< Beginning position of data in the transmit buffer */
	uint8_t tx_buffer_end;		/**< Ending position of data in the transmit buffer */
	uint8_t *rx_buffer;		/**< Pointer to the receive buffer */
	uint8_t rx_buffer_size;		/**< Size of the receive buffer */
	uint8_t rx_buffer_start;	/**< Beginning position of data in the receive buffer */
	uint8_t rx_buffer_end;		/**< Ending position of data in the receive buffer */
	uart_port_t *current_port;	/**< Pointer to the uart_port_t struct that is currently using this USART hardware */
} _uart_buffer_t;

// _uart_buffer_t structs for each hardware USART port
_uart_buffer_t _uart_buffer_c0,		/**< Struct for storing the buffer information for USARTC0 */
               _uart_buffer_c1,         /**< Struct for storing the buffer information for USARTC1 */
               _uart_buffer_d0,         /**< Struct for storing the buffer information for USARTD0 */
               _uart_buffer_d1,         /**< Struct for storing the buffer information for USARTD1 */
               _uart_buffer_e0,         /**< Struct for storing the buffer information for USARTE0 */
               _uart_buffer_e1,         /**< Struct for storing the buffer information for USARTE1 */
               _uart_buffer_f0,         /**< Struct for storing the buffer information for USARTF0 */
               _uart_buffer_f1;         /**< Struct for storing the buffer information for USARTF1 */

/** @brief Macro for handling the transmit complete interrupt
 *
 *  To reduce overhead in the interrupt service rutines, we use a macro to
 *  define behavior of the ISRs. This macro is used in all of the transmit
 *  receive interrupts.
 *
 *  @param _UART_REG The USART_t struct of USART being used
 *  @param _UART_BUFFER The global _uart_buffer_t struct for the port
 */
#define _UART_TX_HANDLER(_UART_REG, _UART_BUFFER) \
	if (_UART_BUFFER.tx_buffer_start != _UART_BUFFER.tx_buffer_end) { \
                /* There is actually data to send, so increment the start position and send that byte. Make sure to handle wrap arounds*/ \
                _UART_REG.DATA = _UART_BUFFER.tx_buffer[_UART_BUFFER.tx_buffer_start];      /* write the data into the output buffer */ \
                _UART_BUFFER.tx_buffer_start = (_UART_BUFFER.tx_buffer_start+1)%_UART_BUFFER.tx_buffer_size; \
        }

/** @brief Macro for handling the receive complete interrupt
 *
 *  To reduce overhead in the interrupt service rutines, we use a macro to
 *  define behavior of the ISRs. This macro is used in all of the receive
 *  receive interrupts.
 *
 *  @param _UART_REG The USART_t struct of USART being used
 *  @param _UART_BUFFER The global _uart_buffer_t struct for the port
 */
#define _UART_RX_HANDLER(_UART_REG, _UART_BUFFER) \
        /* Check if the buffer is full, if it is, then throw out the oldest data by incrementing rx_buffer_start */ \
	if (((_UART_BUFFER.rx_buffer_start != 0) && (_UART_BUFFER.rx_buffer_end == _UART_BUFFER.rx_buffer_start-1)) \
	   || ((_UART_BUFFER.rx_buffer_start == 0) && (_UART_BUFFER.rx_buffer_end == _UART_BUFFER.rx_buffer_size-1))) { \
		/* There is not enough room, so make so space */ \
		_UART_BUFFER.rx_buffer_start = ((_UART_BUFFER.rx_buffer_start+1) % _UART_BUFFER.rx_buffer_size); \
	} \
	/* Now that we have space, copy the byte into the buffer and increment the end */ \
	_UART_BUFFER.rx_buffer[_UART_BUFFER.rx_buffer_end] = _UART_REG.DATA; \
	_UART_BUFFER.rx_buffer_end = ((_UART_BUFFER.rx_buffer_end+1) % _UART_BUFFER.rx_buffer_size); \
        

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

/** @brief Initilize a UART port.
 *
 *  This function initilizes a uart_port_t struct and USART hardware for a new
 *  UART port. 
 *
 *  @note
 *  This function only initilizes the port struct and hardware, it does not
 *  connect the buffers to the interrupts. uart_connect_port() needs to be called
 *  before the port can be used.
 *
 *  @param port_reg Pointer to the PORT_t struct register on which the USART is located
 *  @param uart_reg Pointer to the UART_t struct register of the USART to be used
 *  @param baud_rate Baud rate to configure the port with
 *  @param tx_buffer Pointer to the transmit buffer
 *  @param tx_buffer_length Length of the transmit buffer
 *  @param rx_buffer Pointer to the receive buffer
 *  @param rx_buffer_length Length of the receive buffer
 *  @return uart_port_t struct for the newly configured port
 */
uart_port_t uart_init_port(PORT_t *port_reg, USART_t *uart_reg, uart_baud_t baud_rate, void *tx_buffer, uint8_t tx_buffer_length, void *rx_buffer, uint8_t rx_buffer_length);

/** @brief Connects a UART port to the hardware interrupts.
 *  This function connects a uart_port_t struct's buffers to the USART
 *  interrupts for transmission and receiving. This function must be called
 *  before data can be received or tranmitted.
 *
 *  The use_for_stdio parameter sets
 *  if the port is used by the stdio functions such as printf and scanf. If it
 *  is set to true, then stdout an stdin will be connected to the tx and rx
 *  functions of this port, otherwise it will behave normally. When the port is
 *  connected to the stdio streams it can still be used normally.
 *
 * @param port Pointer to uart_port_t struct to connect interrupts to
 * @param use_for_stdio Specifies if this port is used for stdio
 * @return -1 if the hardware USART is ready in use. 
 * @return 0 on success
 */
int uart_connect_port(uart_port_t *port, bool use_for_stdio);

/** @brief Disconnects a port from the hardware interrupts.
 *
 *  This function disconnects a port from the hardware USART. If one wants to
 *  connect a new uart_port_t to the USART hardware, The existing one must be
 *  disconnected from the hardware using this function.
 *
 *  @param port Pointer to the UART struct to disconnect
 *  @return 0 on sucess
 *  @return -1 if the port wasn't connected in the first place
*/
int uart_disconnect_port(uart_port_t *port);

/** @brief Transmit data by copying it the output buffer.
 *
 *  This function tries to copy data_length bytes of data from the data poitner
 *  into the output buffer. If there is not enough room in the output buffer for
 *  all the data, the function will return the number of bytes that fit.
 *
 *  @param port Pointer to the UART struct used to transmit on
 *  @param data Pointer to the data to transmit
 *  @param data_length Number of bytes to try to transmit
 *  @return Number of bytes copied into the output buffer
 */
int uart_tx_data(uart_port_t *port, void *data, uint8_t data_length);

/** @brief Transmit a byte of data through a UART port.
 *
 *  This function puts a single byte into the transmit buffer if there is room.
 *  If the transmit buffer is already full, then the byte is not added to the
 *  buffer.
 *
 *  @param port Pointer to the UART struct used to transmit
 *  @param data The byte to transmit
 *  @return Number of bytes added to the transmit buffer. 1 if byte was
 *  successfully added to the buffer, 0 if not.
 */
int uart_tx_byte(uart_port_t *port, uint8_t data);

/** @brief Read data out of the receive buffer
 *
 *  This function tries to retrieve data_length number of bytes from a port's
 *  receive buffer. If there are not that many bytes available in the buffer
 *  then the whole buffer is read into data. The number of bytes retrieved is
 *  returned.
 *
 * @param port Pointer to the UART port struct to read from
 * @param data Pointer to where the retrieved data should be written
 * @param data_length Number of bytes to try to read
 * @return Number of bytes actaully copied into data
 */
int uart_rx_data(uart_port_t *port, void *data, uint8_t data_length);

/** @brief Read one byte from a UART port
 *
 *  This function retrieves a single byte from the receive buffer. If the byte
 *  is returned, if there was data to receive, 0 is returned.
 *
 *  @param port Pointer to the UART port to read from
 *  @return Byte read from the receive buffer
 */
uint8_t uart_rx_byte(uart_port_t *port);

#endif //UART_H
