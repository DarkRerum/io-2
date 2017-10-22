#ifndef __CPU_SW_BUS_H
#define __CPU_SW_BUS_H

void bus_write(unsigned long addr, unsigned long data);
unsigned long bus_read(unsigned long addr);

#endif // __CPU_SW_BUS_H
