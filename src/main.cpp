#include <systemc.h>

#include <main.h>

AllSystem *ts;

int sc_main(int argc, char* argv[])
{
    sc_trace_file *f_trace;
    
    ts = new AllSystem("AllSystem");
    
    f_trace = sc_create_vcd_trace_file("waveforms");
    f_trace->set_time_unit(1, SC_PS);
    
    sc_trace(f_trace, *(ts->clk), "clk");
    sc_trace(f_trace, ts->rst_n,  "rst_n");
    sc_trace(f_trace, ts->haddr,  "haddr");
    sc_trace(f_trace, ts->hwdata, "hwdata");
    sc_trace(f_trace, ts->hrdata_to_cpu, "hrdata_to_cpu");
    sc_trace(f_trace, ts->hwrite, "hwrite");
    sc_trace(f_trace, ts->hrdata_to_i2c,  "hrdata_to_i2c");
    sc_trace(f_trace, ts->hrdata_to_din_dout,  "hrdata_to_din_dout");
    sc_trace(f_trace, ts->hsel_i2c,  "hsel_i2c");
    sc_trace(f_trace, ts->hsel_dig,  "hsel_dig");

    sc_trace(f_trace, ts->s_self_addr, "s_self_addr");
    sc_trace(f_trace, ts->s_data_out, "s_data_out");
    sc_trace(f_trace, ts->s_ready, "s_ready");

    sc_trace(f_trace, ts->sda, "sda");
    sc_trace(f_trace, ts->scl, "scl");

    
    sc_start(2300, SC_NS);
    sc_stop();

    sc_close_vcd_trace_file(f_trace);
    
    delete ts;
    
    return 0;
}
