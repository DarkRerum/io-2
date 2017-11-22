#include <systemc.h>

#include "switch.h"
#include "all_system.h"
//
void Switch::select_device() {
	static bool address_phase = false;
	enum {I2C_CONTROLLER, DIN_DOUT, NONE};
	static int device;
	if (address_phase) {
		address_phase = false;
		hsel_i2c_o.write(0);
		hsel_din_dout_o.write(0);
		device = NONE;

		sc_uint<32> address = haddr_bi.read() & ADDRESS_MASK;
		if (address == I2C_BASE) {
			hsel_i2c_o.write(1);
			device = I2C_CONTROLLER;
		}
		else if (address == DIN_DOUT_BASE) {
			hsel_din_dout_o.write(1);
			device = DIN_DOUT;
		}
	}
	else {
		address_phase = true;
		if (device == I2C_CONTROLLER) {
			hrdata_bo.write(hrdata_i2c_bi.read());
		}
		else if (device == DIN_DOUT) {
			hrdata_bo.write(hrdata_din_dout_bi.read());
		}
	}
}
