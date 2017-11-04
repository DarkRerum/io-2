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
	--Reading temperature:

	-1. Write
	HADDRESS = I2C_BASE + I2C_ADDRESS_REG 
	HWDATA = I2C_THERM_ADDRESS (7 bits)

	-2. Write
	HADDRESS = I2C_BASE + I2C_START_REG 
	HWDATA = START << 28 + SEND_STOP << 27 + SIZE (0x0:0x3) << 25 + R/W << 24 + (DATA_TO_WRITE)

	-3. Read
	HADDRESS = I2C_BASE + I2C_READY_REG
	HRDATA = READY

	-3.1 Wait and read again if not READY

	-4. Write
	HADDRESS = I2C_BASE + I2C_START_REG 
	HWDATA = START << 28 + SEND_STOP << 27 + SIZE (0x0:0x3) << 25 + R/W << 24 + (DATA_TO_WRITE)

	-5. Read
	HADDRESS = I2C_BASE + I2C_READY_REG
	HRDATA = READY

	-5.1 Wait and read again if not READY

	-6. Read

	HADDRESS = I2C_BASE + I2C_DATA_OUT_REG
	HRDATA = TEMP_VALUE

	*/
	unsigned long address;
	unsigned long data;

	int ready = 0;
	
	address = I2C_BASE + I2C_ADDRESS_REG;
	data = I2C_THERM_ADDRESS;
	
	bus_write(address, data);
	//printf("sw write 0x%08lX 0x%08lX\n", address, data);
	
	address = I2C_BASE + I2C_START_REG;
	data = (1 /*START*/ << 28) + (0 /*SEND_STOP*/ << 27) + (1 /*SIZE*/ << 25) + (0 /*R/W*/ << 24) + THERM_TEMP_VALUE_REG_ADDR;
	
	bus_write(address, data);
	//printf("sw write 0x%08lX 0x%08lX\n", address, data);
	
	while(!ready) {
		
		address = I2C_BASE + I2C_READY_REG;	
	
		data = bus_read(address);
		//printf("sw read  0x%08lX 0x%08lX\n", address, data);
		ready = data;
	}

	address = I2C_BASE + I2C_START_REG;
	data = (1 /*START*/ << 28) + (1 /*SEND_STOP*/ << 27) + (2 /*SIZE*/ << 25) + (1 /*R/W*/ << 24) + 0x000000;
	
	bus_write(address, data);
	//printf("sw write 0x%08lX 0x%08lX\n", address, data);
	
	ready = 0;
	while(!ready) {
		
		address = I2C_BASE + I2C_READY_REG;	
	
		data = bus_read(address);
		//printf("sw read  0x%08lX 0x%08lX\n", address, data);
		ready = data;
	}

	address = I2C_BASE + I2C_DATA_OUT_REG;
	data = bus_read(address);
	//printf("sw read  0x%08lX 0x%08lX\n", address, data);
	return (short) data;
}

void cpu_main()
{
    unsigned short data; 
	
    printf("Software started!\n");
	//printf("sizeof data is %lu\n", sizeof(data));

    while (1)
    {
        data = get_temperature();
        printf("Software reads from therm: 0x%04X\n", data);
        
        //data = ~data;
        
        printf("Software writes to leds:   0x%04X\n", data);
        dout_write(data);
    }
}
