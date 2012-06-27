#ifndef IO_PIN_H
#define IO_PIN_H
/// This library provides a basic abstraction layer for xMega IO.
/** The IO Pin library provides a light weight abstrcation layer for managing the
 *  IO pins on the xMega. This library not only makes basic DIO easier, but it is
 *  used in the interface of many of the other libraries.
 */

#include <avr/io.h>

/// Struct containing all the important information about a pin.
/** The io_pin_t type is a struct that can be used to describe any GPIO pin on
 *  the xMega. Once the struct has been initilized using the io_create_pin macro
 *  it contains pointers to all of the addresses nessesary to use the pin.
 */
typedef struct {
	PORT_t *io_port;		/**< Pointer to the pin's IO port */
	uint8_t pin;			/**< Zero indexed number of the pin on the port */
	register8_t *control_reg;	/**< Pointer to the control register for the pin */
} io_pin_t;

/// Enum used to specify a pin's direction.
/** This enum is used to specfy the direction of an IO pin in this library. The
 *  values of the enum have been chosen such that the integer value can be added
 *  to the PORT_t address to access either the DIRSET or DIRCLR registers.
 */
typedef enum {
	io_output = 1, 
	io_input = 2
} io_pin_direction_t;

/// Enum used to specify the logic level of a DIO pin
/** This enum specifies a logic level (0 or 3.3V) or an output or input on the
 *  xMega. Unlike the io_pin_direction these values do not directly associate
 *  with the OUTSET or OUTCLR registers of the PORT. Instead, a value of 6 must
 *  be added to the PORT_t address and then the enum must be added. This makes
 *  it possible to put this value directly into an if statement and have a high
 *  vale interpreted as true, and a low value interpreted as false.
 */
typedef enum {
	io_high = -1,
	io_low = 0
} io_pin_level_t;

/// Macro for initilizing an io_pin_t struct.
/** The IO_create_pin macro creates a struct definition for an io_pin_t struct.
 *  This is the preferred way to initilize an io_pin_t because it ensures that
 *  all the nessesary values will be in place.
 */
#define io_create_pin(port, pin) {&port, pin, &(port.PIN##pin##CTRL)}

/// Sets the direction of a DIO pin.
/** This is an inline function which set's the direciton of an xMega IO pin.
 *  @param pin The io_pin_t struct for the pin being configured.
 *  @param direction Direction that the pin should be set to.
 */
inline void io_set_direction(io_pin_t pin, io_pin_direction_t direction);

/// Sets the digital logic level of an output pin.
/** This function sets the output logic level of an IO pin assuming that the pin
 *  is configured as an output.
 *  @param pin Pin of which to set the logic level.
 *  @param level The logic level to set the pin to.
 */
inline void io_set_output(io_pin_t pin, io_pin_level_t level);

/// Gets the logic level of a DIO pin
/** This function gets the logic level of any DIO pin. If the pin is configured
 *  as in input, it's the current logic level. If the pin is an output then this
 *  returns the output value of the pin.
 *  @param pin The pin of which to read the logic level.
 */
inline io_pin_level_t io_get_input(io_pin_t pin);

/// Enables the internal pullup resistor on a pin
/// @param pin Pin of which to enable the pullup on.
inline void io_pin_enable_pullup(io_pin_t pin);

/// Disables the internal pullup resistor on a pin.
/// @param pin Pin of which to disable to pullup resistor.
inline void io_pin_disable_pullup(io_pin_t pin);

#endif // IO_PIN_H
