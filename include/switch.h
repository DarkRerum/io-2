#ifndef __SWITCH_H
#define __SWITCH_H

#include <systemc.h>

/*#define I2C_CONTROLLER_ADDRESS 0x80000000
#define DIN_DOUT_ADDRESS 0x40000000*/
#define ADDRESS_MASK 0xFFFF0000

SC_MODULE(Switch)
{
    sc_in<bool> hclk_i;
    sc_in<sc_uint<32> > haddr_bi;
    //sc_out<sc_uint<32> > haddr_bo;
    //sc_in<sc_uint<32> > hwdata_bi;
    //sc_in<sc_uint<32> > hwdata_bo;
    sc_in<sc_uint<32> > hrdata_i2c_bi;
    sc_in<sc_uint<32> > hrdata_din_dout_bi;
    sc_out<sc_uint<32> > hrdata_bo;
    //sc_in<bool> hwrite_i;
    sc_out<bool> hsel_i2c_o;
    sc_out<bool> hsel_din_dout_o;
    
    SC_CTOR(Switch)
    {
        SC_METHOD(select_device);
        sensitive << hclk_i.neg();
    }
    
private:
	void select_device();
};


#endif // __SWITCH_H
