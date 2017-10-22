#ifndef __ALL_SYSTEM_H
#define __ALL_SYSTEM_H

#include <systemc.h>

#include <cpu.h>
#include <din_dout.h>

#define DIN_DOUT_BASE     0x40000000

SC_MODULE(AllSystem)
{
    sc_clock *clk;
    sc_signal<bool> rst_n;
    sc_signal<sc_uint<32> > haddr;
    sc_signal<sc_uint<32> > hwdata;
    sc_signal<sc_uint<32> > hrdata;
    sc_signal<bool> hwrite;
    sc_signal<bool> hsel;
    
    CPU *cpu;
    DinDout *din_dout;
    
    SC_CTOR(AllSystem)
    {
        clk = new sc_clock("clk", 10, SC_NS);
        
        cpu = new CPU("CPU");
        
        din_dout = new DinDout("DinDout");
        din_dout->set_base_address(DIN_DOUT_BASE);
        
        cpu->hclk_i(*clk);
        cpu->haddr_bo(haddr);
        cpu->hwdata_bo(hwdata);
        cpu->hrdata_bi(hrdata);
        cpu->hwrite_o(hwrite);
        
        din_dout->hclk_i(*clk);
        din_dout->hresetn_i(rst_n);
        din_dout->haddr_bi(haddr);
        din_dout->hwdata_bi(hwdata);
        din_dout->hrdata_bo(hrdata);
        din_dout->hwrite_i(hwrite);
        din_dout->hsel_i(hsel);
        
        SC_THREAD(stimuli);
        sensitive << *clk << rst_n;
    }
    
    ~AllSystem()
    {
        delete clk;
        
        delete cpu;
        delete din_dout;
    }
    
private:
    void stimuli();
};

#endif // __ALL_SYSTEM_H
