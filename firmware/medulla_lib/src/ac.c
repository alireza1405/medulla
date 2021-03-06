#include "ac.h"

ac_port_t ac_init_port(AC_t *ac, uint8_t therm_max) {
	// Setup the port struct
	ac_port_t port;
	port.ac_port = ac;
	port.pin_0 = 0;
	port.pin_1 = 0;
	// Clean input
	therm_max &= 0x3F;

	// Setup control registers
	ac->CTRLA = 0 << AC_AC0OUT_bp; // Disable pin 7 output of comparator 0
	ac->CTRLB = therm_max << AC_SCALEFAC_gp; // Set scaling factor
	// Disable interrupt, set hysteresis mode to none, and enable comparator
	ac->AC0CTRL = (AC_INTLVL_OFF_gc | AC_HYSMODE_NO_gc | AC_ENABLE_bm);
	ac->AC1CTRL = (AC_INTLVL_OFF_gc | AC_HYSMODE_NO_gc | AC_ENABLE_bm);
	// Disable window function and interrupt
	ac->WINCTRL &= ~(AC_WEN_bm | AC_WINTLVL_gm);

	// Set the negative output of both comparators to the scaler output.
	ac->AC0MUXCTRL = AC_MUXNEG_SCALER_gc;
	ac->AC1MUXCTRL = AC_MUXNEG_SCALER_gc;

	return port;
}

void ac_set_pins(ac_port_t *ac, uint8_t pin_0, uint8_t pin_1) {
	// Zero out the positive mux control
	ac->ac_port->AC0MUXCTRL &= ~(AC_MUXPOS_gm);
	ac->ac_port->AC1MUXCTRL &= ~(AC_MUXPOS_gm);
	// Change inputs and update the struct
	ac->ac_port->AC0MUXCTRL |= (pin_0 << AC_MUXPOS_gp);
	ac->ac_port->AC1MUXCTRL |= (pin_1 << AC_MUXPOS_gp);
	ac->pin_0 = pin_0;
	ac->pin_1 = pin_1;
}

bool ac_check_value(ac_port_t *ac, uint8_t pin) {
	if (pin && (ac->ac_port->STATUS & AC_AC1STATE_bm))
		return 0;
	if (!pin && (ac->ac_port->STATUS & AC_AC0STATE_bm))
		return 0;
	// Input value is below negative input, return 1
	return 1;
}