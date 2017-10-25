#include <systemc.h>

#include <all_system.h>

void AllSystem::stimuli()
{
    //hsel_dig = true; // slave is always on/
    
    rst_n = false; // assert reset
    s_self_addr = 0x35;
    wait(20, SC_NS);
    rst_n = true; // deassert reset
}
