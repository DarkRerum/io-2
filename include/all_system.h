#ifndef __ALL_SYSTEM_H
#define __ALL_SYSTEM_H

#include <systemc.h>

#include <cpu.h>
#include <din_dout.h>
#include <master_driver.h>

#define DIN_DOUT_BASE     0x40000000
#define I2C_BASE 0x80000000
#define I2C_ADDRESS_REG 0x00000004
#define I2C_START_REG 0x00000008
#define I2C_READY_REG 0x0000000C
#define I2C_DATA_OUT_REG 0x00000010

#define I2C_THERM_ADDRESS 0x55
#define THERM_TEMP_VALUE_REG 0x00


SC_MODULE(AllSystem)
{
    sc_clock *clk;
    sc_signal<bool> rst_n;
    sc_signal<sc_uint<32> > haddr;
    sc_signal<sc_uint<32> > hwdata;
    sc_signal<sc_uint<32> > hrdata;
    sc_signal<bool> hwrite;
    sc_signal<bool> hsel_i2c;
	sc_signal<bool> hsel_dig;
	//sc_signal<sc_uint<2> > hsel;
    
    CPU *cpu;
    DinDout *din_dout;
	master_driver *i2c_master;
    
    SC_CTOR(AllSystem)
    {
        clk = new sc_clock("clk", 10, SC_NS);
        
        cpu = new CPU("CPU");
        
        din_dout = new DinDout("DinDout");
        din_dout->set_base_address(DIN_DOUT_BASE);
        
		i2c_master = new master_driver("I2C_master");
		
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
        din_dout->hsel_i(hsel_dig);
		
		i2c_master->clk_i(*clk);
		i2c_master->HRESETn_i(rst_n);
		i2c_master->HADDR_bi(haddr);
		i2c_master->HWDATA_bi(hwdata);
		i2c_master->HRDATA_bo(hrdata);
		i2c_master->HWRITE_i(hwrite);
		i2c_master->HSEL_i(hsel_i2c);
		/*sc_in<bool> HRESETn_i;
    sc_in<bool> HWRITE_i;
    sc_in<bool> HSEL_i;
	sc_in< sc_uint<32> > HADDR_bi;
	sc_in< sc_uint<32> > HWDATA_bi;
	sc_out< sc_uint<32> > HRDATA_bo;*/
        
        SC_THREAD(stimuli);
        sensitive << *clk << rst_n;
    }
    
    ~AllSystem()
    {
        delete clk;
        
        delete cpu;
        delete din_dout;
		delete i2c_master;
    }
    
private:
    void stimuli();
};

#endif // __ALL_SYSTEM_H
