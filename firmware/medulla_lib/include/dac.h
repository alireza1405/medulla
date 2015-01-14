#ifndef DAC_H
#define DAC_H

/** @file
 *  @brief Provides a asyncronous interface to the xMega internal DACs
 *  
 *  This driver provides an simple interface to control the output from the
 *	DAC to the AC/ADC.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>


/** @brief This struct stores the ADC configuration
 */
typedef struct {
	DAC_t *dac_port; /**< Pointer to the DAC registers this struct uses */
	volatile uint16_t value; /**< 12 bit value which the DAC is set to */
} dac_port_t;

/** @brief Initialize a port's DAC
 *  
 *  This function creates and returns a configured dac_port_t struct. It also
 *  configures the given digital to analog convertor to operate with this
 *  driver.
 *  
 *  @param dac Pointer to the DAC_t register for the port to configure.
 *  @return Returns a struct that can be used to reference the port's DAC.
 */
dac_port_t dac_init_port(DAC_t *dac);

/** @brief Sets the output of the DAC to a new value.
 *  
 *  This function can be used to set the internal output value of the DAC.
 *
 *  @param dac Pointer to the dac_port_t struct to check read status.
 *	@param new_value New 12 bit value to set the DAC output to.
 */
void dac_set_value(dac_port_t *dac, uint16_t new_value);

/** @brief Sets the calibration values of the DAC.
 *  
 *  This function can be used to set the internal calibration values of the DAC.
 *
 *  @param dac Pointer to the dac_port_t struct to check read status.
 *	@param offset Offset calibration value. See ATXMega128a1 datasheet page 313.
 *	@param gain Gain calibration value. See ATXMega128a1 datasheet page 313.
 */
void dac_set_calibration(dac_port_t *dac, uint8_t offset, uint8_t gain);

#endif //DAC_H
