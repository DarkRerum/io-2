#ifndef __DIN_DOUT_H
#define __DIN_DOUT_H

#include <systemc.h>

// Register map of bus slave
#define DIN_DOUT_IN_REG   0x00000004
#define DIN_DOUT_OUT_REG  0x00000008


SC_MODULE(DinDout)
{
    sc_in<bool> hclk_i;
    sc_in<bool> hresetn_i;
    sc_in<sc_uint<32> >  haddr_bi;
    sc_in<sc_uint<32> >  hwdata_bi;
    sc_out<sc_uint<32> > hrdata_bo;
    sc_in<bool> hwrite_i;
    sc_in<bool> hsel_i;
    
    sc_uint<16> switches, leds;
    
    SC_CTOR(DinDout)
    {
        SC_METHOD(bus_slave);
        sensitive << hclk_i.pos() << hresetn_i.pos();
        
        SC_THREAD(user_input); // imitation of switches moving by user
        sensitive << hclk_i.pos();
        
        leds = 0;
    }
    
    void set_base_address(sc_uint<32> base_addr)
    {
        this->base_addr = base_addr;
    }
    
private:
    sc_uint<32> base_addr;

    void bus_slave();
    void user_input();
    
    sc_uint<32> execute_read(sc_uint<16> addr);
    void execute_write(sc_uint<16> addr, sc_uint<32> data);
};

#endif // __DIN_DOUT_H
