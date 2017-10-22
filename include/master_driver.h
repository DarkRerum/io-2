#ifndef __MASTER_DRIVER_H
#define __MASTER_DRIVER_H

#include <systemc.h>

SC_MODULE(master_driver)
{
    // External interface
    
    sc_in<bool> clk_i;
    sc_in<bool> rst_i;
    
    sc_in<bool> start_i;		// signal to start transaction
    sc_in< sc_uint<7> >  addr_in_bi;	// address of target slave
    sc_in<bool> read_not_write_i;	// transaction type
    sc_in< sc_uint<8> >  data_in_bi;	// data for writing to slave
    sc_out< sc_uint<8> >  data_out_bo;	// data read from slave in last transaction
    sc_out<bool> ready_o;		// transaction is not processed now
    sc_out<bool> error_o;		// error in last transaction (not implemented)
    
    // I2C interface
    sc_inout<sc_logic> i2c_sda_io;
    sc_inout<sc_logic> i2c_scl_io;
    
    // Constructor
    
    SC_CTOR(master_driver)
    {
        // Sequential block
        SC_METHOD(listen_start);
        sensitive << clk_i.pos() << rst_i.pos() << start_i.pos();
        
        // Combinational block
        SC_METHOD(output_selector);
        sensitive << clk_i << rst_i;
    }
    
    // Methods
    
    void listen_start();
    void output_selector();
};

#endif // __MASTER_DRIVER_H
