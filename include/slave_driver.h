#ifndef __SLAVE_DRIVER_H
#define __SLAVE_DRIVER_H

#include <systemc.h>

SC_MODULE(slave_driver)
{
    // External interface
    
    sc_in<bool> clk_i;
    sc_in<bool> rst_i;
    
	sc_in< sc_uint<7> > self_addr_bi;     // I2C slave self address
    sc_in< sc_uint<16> > data_in_bi;		// data that can be read from slave
    sc_out< sc_uint<16> > data_out_bo;     // data written to slave in last transaction
    sc_out<bool> ready_o;          		// transaction is not processed now
    
	// I2C interface
    sc_inout<sc_logic> i2c_sda_io;
    sc_in<sc_logic> i2c_scl_i;

    // Constructor
    
    SC_CTOR(slave_driver)
    {
        // Sequential block
        SC_METHOD(listen_start);
        sensitive << clk_i.pos() << rst_i.pos();
        
        SC_METHOD(listen_stop);
        sensitive << clk_i.pos() << rst_i.pos();

        SC_METHOD(emulate_slave);
        sensitive << clk_i.pos() << rst_i.pos();
        
        // Combinational block
        SC_METHOD(output_selector);
        sensitive << clk_i << rst_i;
    }
    
    // Methods
    
    void listen_start();
    void listen_stop();
    void emulate_slave();
    void output_selector();
};

#endif // __SLAVE_DRIVER_H
