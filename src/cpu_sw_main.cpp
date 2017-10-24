#include <stdio.h>

#include <all_system.h>
#include <cpu_sw_bus.h>
#include <systemc.h>

void dout_write(unsigned long data)
{
    bus_write(DIN_DOUT_BASE + DIN_DOUT_OUT_REG, data);
}

unsigned long din_read()
{
    return bus_read(DIN_DOUT_BASE + DIN_DOUT_IN_REG);
}

unsigned short get_temperature()
{
	/*
	HADDRESS = I2C_BASE + I2C_ADDRESS_REG 
	HWDATA = I2C_THERM_ADDRESS << 8 + THERM_TEMP_VALUE_REG

	-2. Write
	HADDRESS = I2C_BASE + I2C_START_REG 
	HWDATA = START << 18 + TWO_BYTES << 17 + R/W << 16 + 0x0000 (DATA_TO_WRITE)

	-3. Read
	HADDRESS = I2C_BASE + I2C_READY_REG
	HRDATA = READY

	-3.1 Wait and ready again if not READY

	-4. Read
	HADDRESS = I2C_BASE + I2C_DATA_OUT_REG 
	HRDATA = TEMP_VALUE
	*/
	
	unsigned long address;
	unsigned long data;
	const double WAIT_DELAY_NS = 10000.0;
	int ready = 0;
	
	address = I2C_BASE + I2C_ADDRESS_REG;
	data = (I2C_THERM_ADDRESS << 8) + THERM_TEMP_VALUE_REG;
	
	bus_write(address, data);
	printf("sw write 0x%08lX 0x%08lX", address, data);
	
	address = I2C_BASE + I2C_START_REG;
	data = (1 /*START*/ << 18) + (1 /*TWO_BYTES*/ << 17) + (1 /*R/W*/ << 16) + 0x0000;
	
	bus_write(address, data);
	printf("sw write 0x%08lX 0x%08lX", address, data);
	
	while(!ready) {
		wait(WAIT_DELAY_NS, SC_NS);
		
		address = I2C_BASE + I2C_READY_REG;	
	
		data = bus_read(address);
		printf("sw read  0x%08lX 0x%08lX", address, data);
		ready = data;
	}
	
	address = I2C_BASE + I2C_DATA_OUT_REG;
	data = bus_read(address);
	printf("sw read  0x%08lX 0x%08lX", address, data);
	return (short) data;
}

void cpu_main()
{
    unsigned short data; 
	
    printf("Software started!\n");
	printf("sizeof data is %lu", sizeof(data));

    while (1)
    {
        data = get_temperature();
        printf("Software reads from therm: 0x%04X\n", data);
        
        //data = ~data;
        
        printf("Software writes to leds:   0x%04X\n", data);
        dout_write(data);
    }
}
