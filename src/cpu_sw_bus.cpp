#include <stdio.h>

#include <main.h>

//#define ADDRESS_MASK 0xFFFF0000

static sc_uint<32> bus_data;

void bus_write(unsigned long addr, unsigned long data)
{
    bus_data = (sc_uint<32>) data;
    
    ts->cpu->bus_trans(addr, &bus_data, true);
}

unsigned long bus_read(unsigned long addr)
{
    ts->cpu->bus_trans(addr, &bus_data, false);
    return (unsigned long) bus_data;
}

/*void select_device(unsigned long addr) {
	unsigned long selected_device = addr && ADDRESS_MASK;
	
	switch(selected_device) {
		case DIN_DOUT_BASE:
			ts->hsel_dig = true;
			ts->hsel_i2c = false;
		break;
		case I2C_BASE:
			ts->hsel_dig = false;
			ts->hsel_i2c = true;
		break;
		default:
			ts->hsel_dig = false;
			ts->hsel_i2c = false;
		break;
	}
}

void bus_write(unsigned long addr, unsigned long data)
{	
	//select_device(addr);
	
    bus_data = (sc_uint<32>) data;
    
    ts->cpu->bus_trans(addr, &bus_data, true);
}

unsigned long bus_read(unsigned long addr)
{
	//select_device(addr);
	
    ts->cpu->bus_trans(addr, &bus_data, false);
    return (unsigned long) bus_data;
}*/
