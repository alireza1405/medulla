#ifndef AC_H
#define AC_H

/** @file
 *  @brief Provides an asyncronous interface to the xMega internal ACs
 *  
 *  This driver provides an interrupt driven analog comparator interface
 *	which allows the user to configure the reference voltage from the DAC
 *	and change which pins are being read from.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

/** @brief This struct stores the AC configuration
 */
typedef struct {
	AC_t *ac_port; /**< Pointer to the AC registers which this struct uses */
	volatile uint8_t pin_0; /**< Zero indexed number for which pin to be read on comparator 0 */
	volatile uint8_t pin_1; /**< Zero indexed number for which pin to be read on comparator 1 */
} ac_port_t;

/** @brief Initialize a port of AC inputs
 *  
 *  This function creates and returns a configured ac_port_t struct. It also
 *  configures the given analog comparator to operate with this driver.
 *  
 *  @param ac Pointer to the AC_t register for the port to configure.
 *	@param therm_max 5 Bit value to set the scaler with.
 *  @return Returns a struct that can be used to reference the port.
 */
ac_port_t ac_init_port(AC_t *ac, uint8_t therm_max);

/** @brief Sets a pin to be compared on the port
 *
 *  This function configures a given pin to be compared on the port
 *
 *  @param ac Pointer to an ac_port_t struct to initialize the pins on.
 *  @param pin_a Zero indexed number of the pin to read on comparator 0.
 *	@param pin_b Zero indexed number of the pin to read on comparator 1.
 */
void ac_set_pins(ac_port_t *ac, uint8_t pin_0, uint8_t pin_1);

/** @brief Checks the status of the comparators.
 *
 *  This function checks the output status of the comparators and
 *	returns a true value if either comparator is triggered.
 *
 *  @param ac Pointer to the ac_port_t struct to start reading.
 *	@param pin Selects which comparator to read (0 or 1)
 *	@return Returns a 1 if selected comparator is triggered.
 */
bool ac_check_value(ac_port_t *ac, uint8_t pin);

#endif //ADC_H