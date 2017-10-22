#include <systemc.h>

#include <din_dout.h>

#include <stdio.h>

void DinDout::bus_slave()
{
    static bool wr_flag = false;
    static sc_uint<32> wr_addr = 0;
    
    if (!hresetn_i.read())
    {
        wr_flag = false;
        wr_addr = 0;
        
        leds = 0;
    }
    else if (hclk_i.read() && hsel_i.read())
    {
        if (wr_flag)
        {
            execute_write((sc_uint<16>) (wr_addr - base_addr), hwdata_bi.read());
            wr_flag = false;
        }
        
        if (hwrite_i.read())
        {
            wr_flag = true;
            wr_addr = haddr_bi.read();
        }
        else
        {
            hrdata_bo.write((sc_uint<16>) (execute_read(haddr_bi.read() - base_addr)));
        }
    }
}

void DinDout::user_input()
{
    switches = 0x7895;
    printf("Switches new state: 0x%04X\n", (unsigned short) switches);
    
    wait(100, SC_NS);
    
    switches = 0x55AA;
    printf("Switches new state: 0x%04X\n", (unsigned short) switches);
    
    wait(100, SC_NS);
    
    switches = 0x7744;
    printf("Switches new state: 0x%04X\n", (unsigned short) switches);
}

sc_uint<32> DinDout::execute_read(sc_uint<16> addr)
{
    sc_uint<32> data;

    switch (addr)
    {
    case DIN_DOUT_IN_REG:
        data = switches;
        break;
    case DIN_DOUT_OUT_REG:
        data = leds;
        break;
    default:
        data = 0;
        break;
    }
    
    return data;
}

void DinDout::execute_write(sc_uint<16> addr, sc_uint<32> data)
{
    switch (addr)
    {
    case DIN_DOUT_OUT_REG:
        leds = data;
        break;
    default:
        break;
    }
}
