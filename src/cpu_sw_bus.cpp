#include <stdio.h>

#include <main.h>


static sc_uint<32> bus_data;

void bus_write(unsigned long addr, unsigned long data)
{
    bus_data = (sc_uint<32>) data;
    
    ts->cpu->bus_trans(addr, &bus_data, true);
}

unsigned long bus_read(unsigned long addr)
{
    ts->cpu->bus_trans(addr, &bus_data, false);
    return (unsigned long) bus_data;
}
