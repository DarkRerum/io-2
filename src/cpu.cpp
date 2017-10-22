#include <systemc.h>

#include <cpu.h>
#include <cpu_sw_main.h>

void CPU::cpu_software()
{
    
    cpu_main();
}

void CPU::bus_trans(sc_uint<32> addr, sc_uint<32> *data, bool write)
{
    wait();
    haddr_bo.write(addr);
    hwrite_o.write(write);
    
    wait();
    hwrite_o.write(false);
    
    if (write)
    {
        hwdata_bo.write(*data);
        
        cout << "CPU WRITE" << endl;
        cout << "  -> addr: 0x" << hex << addr << endl;
        cout << "  -> data: 0x" << hex << *data << endl;
    }
    else
    {
        wait();
        
        *data = hrdata_bi.read();
        
        cout << "CPU READ" << endl;
        cout << "  -> addr: 0x" << hex << addr << endl;
        cout << "  -> data: 0x" << hex << *data << endl;
    }
}
