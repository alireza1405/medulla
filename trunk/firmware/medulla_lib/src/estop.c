#include "estop.h"

estop_port_t estop_init_port(io_pin_t panic_pin, io_pin_t estop_pin, void *counter_pointer, void (*callback)(void)) {
	estop_port_t port;
	port.panic_pin = panic_pin;
	port.estop_pin = estop_pin;
	port.is_enabled = false;
	port.limit_sw_port = limit_sw_init_port(estop_pin.io_port, 1<<estop_pin.pin, counter_pointer, callback);

	io_set_direction(estop_pin, io_input);
	// Since the limit switch driver is going to try to enable the pullup, let's disable it again
	io_pin_disable_pullup(estop_pin);

	io_set_direction(panic_pin,io_output);
	io_set_output(panic_pin,io_high);
	return port;
}

inline int estop_enable_port(estop_port_t *estop_port) {
	estop_port->is_enabled = true;
	return limit_sw_enable_port(&(estop_port->limit_sw_port));
}

inline void estop_disable_port(estop_port_t *estop_port) {
	estop_port->is_enabled = false;
	limit_sw_disable_port(&(estop_port->limit_sw_port));
}

inline void estop_assert_port(estop_port_t *estop_port) {
	if (estop_port->is_enabled)
		io_set_output(estop_port->panic_pin, io_low);
}

inline void estop_deassert_port(estop_port_t *estop_port) {
	if (estop_port->is_enabled)
		estop_port->estop_pin.io_port->OUTSET = 1<<estop_port->panic_pin.pin;
}

bool estop_is_estopped(estop_port_t *estop_port) {
	if (estop_port->is_enabled)
		return io_get_input(estop_port->estop_pin);
	else
		return false;
}

