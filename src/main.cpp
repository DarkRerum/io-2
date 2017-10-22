#include <systemc.h>

#include <main.h>

AllSystem *ts;

int sc_main(int argc, char* argv[])
{
    sc_trace_file *f_trace;
    
    ts = new AllSystem("AllSystem");
    
    f_trace = sc_create_vcd_trace_file("waveforms");
    f_trace->set_time_unit(1, SC_PS);
    
    sc_trace(f_trace, *(ts->clk),    "clk");
    sc_trace(f_trace, ts->rst_n,  "rst_n");
    sc_trace(f_trace, ts->haddr,  "haddr");
    sc_trace(f_trace, ts->hwdata, "hwdata");
    sc_trace(f_trace, ts->hrdata, "hrdata");
    sc_trace(f_trace, ts->hwrite, "hwrite");
    
    sc_start(300, SC_NS);
    sc_stop();

    sc_close_vcd_trace_file(f_trace);
    
    delete ts;
    
    return 0;
}
