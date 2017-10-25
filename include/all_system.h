#ifndef __ALL_SYSTEM_H
#define __ALL_SYSTEM_H

#include <systemc.h>

#include <cpu.h>
#include <din_dout.h>
#include <master_driver.h>
#include <slave_driver.h>

#define DIN_DOUT_BASE     0x40000000
#define I2C_BASE 0x80000000
#define I2C_ADDRESS_REG 0x00000004
#define I2C_START_REG 0x00000008
#define I2C_READY_REG 0x0000000C
#define I2C_DATA_OUT_REG 0x00000010

#define I2C_THERM_ADDRESS 0x35
#define THERM_TEMP_VALUE_REG 0x00


SC_MODULE(AllSystem)
{
    sc_clock *clk;
    sc_signal<bool> rst_n;
    sc_signal<sc_uint<32> > haddr;
    sc_signal<sc_uint<32> > hwdata;
    sc_signal<sc_uint<32>, SC_MANY_WRITERS> hrdata;
    sc_signal<bool> hwrite;
    sc_signal<bool> hsel_i2c;
	sc_signal<bool> hsel_dig;

	sc_signal<sc_uint<7> > s_self_addr;
	sc_signal<sc_uint<8> > s_data_out;
	sc_signal<bool> s_ready;

	sc_signal<sc_logic, SC_MANY_WRITERS> sda;
	sc_signal<sc_logic> scl;
	//sc_signal<sc_uint<2> > hsel;
    
    CPU *cpu;
    DinDout *din_dout;
	master_driver *i2c_master;
	slave_driver *i2c_slave;
    
    SC_CTOR(AllSystem)
    {
        clk = new sc_clock("clk", 10, SC_NS);
        
        cpu = new CPU("CPU");
        
        din_dout = new DinDout("DinDout");
        din_dout->set_base_address(DIN_DOUT_BASE);
        
		i2c_master = new master_driver("I2C_master");
		i2c_slave = new slave_driver("I2C_slave");

        cpu->hclk_i(*clk);
        cpu->haddr_bo(haddr);
        cpu->hwdata_bo(hwdata);
        cpu->hrdata_bi(hrdata);
        cpu->hwrite_o(hwrite);
		cpu->rst_n_i(rst_n);
        
        din_dout->hclk_i(*clk);
        din_dout->hresetn_i(rst_n);
        din_dout->haddr_bi(haddr);
        din_dout->hwdata_bi(hwdata);
        din_dout->hrdata_bo(hrdata);
        din_dout->hwrite_i(hwrite);
        din_dout->hsel_i(hsel_dig);
		
		i2c_master->HCLK_i(*clk);
		i2c_master->HRESETn_i(rst_n);
		i2c_master->HADDR_bi(haddr);
		i2c_master->HWDATA_bi(hwdata);
		i2c_master->HRDATA_bo(hrdata);
		i2c_master->HWRITE_i(hwrite);
		i2c_master->HSEL_i(hsel_i2c);
		i2c_master->i2c_sda_io(sda);
		i2c_master->i2c_scl_io(scl);

		i2c_slave->clk_i(*clk);
		i2c_slave->rstn_i(rst_n);
		i2c_slave->self_addr_bi(s_self_addr);
		i2c_slave->data_out_bo(s_data_out);
		i2c_slave->ready_o(s_ready);
		i2c_slave->i2c_sda_io(sda);
		i2c_slave->i2c_scl_i(scl);
		/*sc_in<bool> HRESETn_i;
    sc_in<bool> HWRITE_i;
    sc_in<bool> HSEL_i;
	sc_in< sc_uint<32> > HADDR_bi;
	sc_in< sc_uint<32> > HWDATA_bi;
	sc_out< sc_uint<32> > HRDATA_bo;*/
        
        SC_THREAD(stimuli);
        sensitive << *clk << rst_n << s_self_addr;
    }
    
    ~AllSystem()
    {
        delete clk;
        
        delete cpu;
        delete din_dout;
		delete i2c_master;
		delete i2c_slave;
    }
    
private:
    void stimuli();
};

#endif // __ALL_SYSTEM_H
