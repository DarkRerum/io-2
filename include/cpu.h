#ifndef __CPU_H
#define __CPU_H

#include <systemc.h>

SC_MODULE(CPU)
{
    sc_in<bool> hclk_i;
    sc_out<sc_uint<32> > haddr_bo;
    sc_out<sc_uint<32> > hwdata_bo;
    sc_in<sc_uint<32> >  hrdata_bi;
    sc_out<bool> hwrite_o;
    
    SC_CTOR(CPU)
    {
        haddr_bo.initialize(0);
        hwdata_bo.initialize(0);
        hwrite_o.initialize(0);
        
        SC_THREAD(cpu_software);
        sensitive << hclk_i.pos();
    }
    
    void bus_trans(sc_uint<32> addr, sc_uint<32> *data, bool write);
    
private:
    void cpu_software();
};

#endif // __CPU_H
