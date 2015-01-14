#include "dac.h"

dac_port_t dac_init_port(DAC_t *dac) {
	// Setup the port struct
	dac_port_t port;
	port.dac_port = dac;
	port.value = 0;

	// Setup control registers
	dac->CTRLA = (DAC_IDOEN_bm | DAC_ENABLE_bm); // Enable DAC and internal output
	dac->CTRLB = (DAC_CHSEL_SINGLE_gc | DAC_CH0TRIG_bm); // Enable Ch0 output and event trigger
	dac->CTRLC = (DAC_REFSEL_AVCC_gc); // Set reference to AVCC
	dac->EVCTRL = (DAC_EVSEL_6_gc); // Set event channel to Ch1
	dac->TIMCTRL = (DAC_CONINTVAL_1CLK_gc | DAC_REFRESH_16CLK_gc);

	return port;
}

void dac_set_value(dac_port_t *dac, uint16_t value) {
	dac->dac_port->CH0DATAL = (uint8_t)(value & 0xFF);
	dac->dac_port->CH0DATAH = (uint8_t)(value >> 8);
	dac->value = value;
}

void dac_set_calibration(dac_port_t *dac, uint8_t offset, uint8_t gain) {
	dac->dac_port->GAINCAL = gain;
	dac->dac_port->OFFSETCAL = offset;
}