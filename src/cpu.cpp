#include <systemc.h>

#include <cpu.h>
#include <cpu_sw_main.h>
#include <all_system.h>
#include <main.h>

#define ADDRESS_MASK 0xFFFF0000

void CPU::cpu_software()
{
    
    cpu_main();
}

void CPU::select_device(unsigned long addr) {
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

void CPU::bus_trans(sc_uint<32> addr, sc_uint<32> *data, bool write)
{
    wait();
	select_device(addr);
    haddr_bo.write(addr);
    hwrite_o.write(write);
    
    wait();
    hwrite_o.write(false);
    
    if (write)
    {
        hwdata_bo.write(*data);
        
        cout << "CPU WRITE" << endl;
        cout << "  -> addr: 0x" << hex << addr << endl;
        cout << "  -> data: 0x" << hex << *data << endl;
    }
    else
    {
        wait();
        
        *data = hrdata_bi.read();
        
        cout << "CPU READ" << endl;
        cout << "  -> addr: 0x" << hex << addr << endl;
        cout << "  -> data: 0x" << hex << *data << endl;
    }
}
