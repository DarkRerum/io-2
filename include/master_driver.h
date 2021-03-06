#ifndef __MASTER_DRIVER_H
#define __MASTER_DRIVER_H

#include <systemc.h>

SC_MODULE(master_driver)
{
    // External interface

	sc_in<bool> HCLK_i;
    sc_in<bool> HRESETn_i;
    sc_in<bool> HWRITE_i;
    sc_in<bool> HSEL_i;
	sc_in< sc_uint<32> > HADDR_bi;
	sc_in< sc_uint<32> > HWDATA_bi;
	sc_out< sc_uint<32> > HRDATA_bo;
	
    /*
    sc_in<bool> clk_i;
    sc_in<bool> rst_i;
    
	sc_in<bool> start_i;		// signal to start transaction
	sc_in<bool> two_bytes_i;	// set - read/write 2 bytes; unset - read/write 1 byte
    sc_in< sc_uint<7> >  addr_in_bi;	// address of target slave
	sc_in< sc_uint<8> > inner_addr_in_bi;  // inner target slave address
    sc_in<bool> read_not_write_i;	// transaction type
    sc_in< sc_uint<16> >  data_in_bi;	// data for writing to slave
    sc_out< sc_uint<16> >  data_out_bo;	// data read from slave in last transaction
    sc_out<bool> ready_o;		// transaction is not processed now
    sc_out<bool> error_o;		// error in last transaction (not implemented)
	*/
    
    // I2C interface
    sc_inout<sc_logic> i2c_sda_io;
    sc_inout<sc_logic> i2c_scl_io;
    
    // Constructor
    
    SC_CTOR(master_driver)
    {
        // Sequential block
        SC_METHOD(listen_start);
        sensitive << HCLK_i.pos() << HRESETn_i.neg();
		SC_METHOD(handle_amba);
		sensitive << HCLK_i.pos();
        
        // Combinational block
        SC_METHOD(output_selector);
        sensitive << HCLK_i << HRESETn_i;
    }
    
    // Methods
    
    void listen_start();
    void output_selector();
	void handle_amba();
};

#endif // __MASTER_DRIVER_H
