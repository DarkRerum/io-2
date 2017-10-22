#include <stdio.h>

#include <all_system.h>
#include <cpu_sw_bus.h>

void dout_write(unsigned long data)
{
    bus_write(DIN_DOUT_BASE + DIN_DOUT_OUT_REG, data);
}

unsigned long din_read()
{
    return bus_read(DIN_DOUT_BASE + DIN_DOUT_IN_REG);
}

void cpu_main()
{
    unsigned short data;
    
    printf("Software started!\n");

    while (1)
    {
        data = din_read();
        printf("Software reads from switches: 0x%04X\n", data);
        
        data = ~data;
        
        printf("Software writes to leds:      0x%04X\n", data);
        dout_write(data);
    }
}
