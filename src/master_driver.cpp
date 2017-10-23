#include <systemc.h>
#include <master_driver.h>

enum {
    MAX_ADDRESS_INDEX = 6,
    MAX_INNER_ADDRESS_INDEX = 7,
	MAX_DATA_INDEX = 7,
	ACK_INDEX = 8,
	ONE_PAST_LAST_INDEX = 9
};

typedef enum {
	NONE,
	START_BEGIN,
	START_MIDDLE,
	START_END,
	PAUSE_BEGIN,
	PAUSE,
	PAUSE_END,
	TRANSIT,
	STOP_BEGIN,
	STOP_END
} transaction_state_t;

typedef enum {
	DEVICE_ADDRESS,
	INNER_ADDRESS,
	WDATA,
	RDATA
} data_type_t;

static transaction_state_t state;
static data_type_t type;
static bool release_sda;
static int bit_index;
static int bytes_remaining;
static int max_all_data_index;
static sc_uint<16> data_out;
static sc_uint<16> data_in;
static sc_uint<7> addr_in;
static sc_uint<8> inner_addr_in;
static bool read_not_write;
static bool repeat;

static bool sda;
static bool scl;

void master_driver::listen_start()
{
	if (rst_i.read())
    {
		repeat = 0;
		data_out = 0;
        sda = 1;
		scl = 1;
        state = NONE;
		bit_index = 0;
		error_o.write(0);
		release_sda = 0;
		ready_o.write(1);
		type = DEVICE_ADDRESS;
		data_out = 0;
		bytes_remaining = 0;
		max_all_data_index = 0;
    }
	else if (start_i.read())
	{
		repeat = 0;
		ready_o.write(0);
		state = START_BEGIN;
		release_sda = 0;
        bit_index = 0;
        state = START_BEGIN;
        type = DEVICE_ADDRESS;
		data_out = 0;
		data_in = data_in_bi.read();
		addr_in = addr_in_bi.read();
		inner_addr_in = inner_addr_in_bi.read();
		read_not_write = read_not_write_i.read();
		if (two_bytes_i.read()) 
		{
			bytes_remaining = 1;
			max_all_data_index = 15;
		}
		else
		{
			bytes_remaining = 0;
			max_all_data_index = 7;
		}
	}
	else 
	{
		switch(state) 
		{
			case NONE: break;
			
			case START_BEGIN: 
			{
				release_sda = 0;
				sda = 1;
				state = START_MIDDLE;
			} break;

			case START_MIDDLE: 
			{
				scl = 1;
				state = START_END;
			} break;

			case START_END: 
			{
				sda = 0;
				state = PAUSE_BEGIN;
			} break;

			case PAUSE_BEGIN:
			{
				scl = 0;
				state = PAUSE;
				if (bit_index == ONE_PAST_LAST_INDEX)
				{
					//release_sda = read_not_write;
					if (type == WDATA || type == RDATA)
					{
						if (bytes_remaining == 0)
						{
                        	state = STOP_BEGIN;							
						}
						else
						{
							
							bytes_remaining--;
                        	state = PAUSE;							
						}

                    }
                    else if (type == DEVICE_ADDRESS)
					{
						if (repeat)
						{
							type = (read_not_write) ? RDATA : WDATA;
						}
                     	else
						{
							type = INNER_ADDRESS;
						}						
                    }
					else if (type == INNER_ADDRESS)
					{
						if (read_not_write)
						{
							repeat = true;
							state = START_BEGIN;
							type = DEVICE_ADDRESS;
						}
						else
						{
							type = (read_not_write) ? RDATA : WDATA;
						}
                        
                    }
                    bit_index = 0;
				}
			} break;
                
            case PAUSE:
			{
				switch (bit_index)
				{
					case ACK_INDEX:
					{
						sda = (bytes_remaining == 0) ? 1 : 0;
                        release_sda = (type == RDATA) ? 0 : 1;
                        state = PAUSE_END;
					} break;
                    default:
					{
						release_sda = (type == RDATA) ? 1 : 0;
                        switch (type)
						{
                            case WDATA:
							{
                                //sda = data_in[MAX_DATA_INDEX - bit_index];
								sda = data_in[max_all_data_index];
								data_in <<= 1;
                            } break;
                            case DEVICE_ADDRESS:
							{
								bool is_read = read_not_write && repeat;
                                sda = (bit_index == MAX_DATA_INDEX) ? is_read : addr_in[MAX_ADDRESS_INDEX - bit_index];
                            } break;
							case INNER_ADDRESS:
							{
                                sda = inner_addr_in[MAX_INNER_ADDRESS_INDEX - bit_index];
                            } break;
							case RDATA: break;
							
                        }
                        state = PAUSE_END;
					} break;
				}
			} break;
            
			case PAUSE_END:
			{
				scl = 1;
				state = TRANSIT;
			} break;
                
            case TRANSIT:
			{
                if (bit_index == ACK_INDEX)
				{
                    /*if (type != RDATA && i2c_sda_io.read() != '0') // Critical
					{
                        error_o.write(1);
                    }*/
                }
                else if (type == RDATA)
				{
					//data_out[MAX_DATA_INDEX - bit_index] = (i2c_sda_io.read() == '1') ? 1 : 0;
					data_out <<= 1;
					data_out[0] = (i2c_sda_io.read() == '1') ? 1 : 0;
                }
                bit_index = bit_index + 1;
                state = PAUSE_BEGIN;
			} break;
            
			case STOP_BEGIN:
			{
                release_sda = 0;
                scl = 1;
                sda = 0;
                state = STOP_END;
            } break;
                
            case STOP_END:
			{
                sda = 1;
                ready_o.write(1);
                state = NONE;
			} break;
		}
	}
}

void master_driver::output_selector()
{
	if (rst_i.read()) 
	{
		i2c_sda_io.write(sc_logic(1));
		i2c_scl_io.write(sc_logic(1));
		data_out_bo.write(sc_uint<16>(0));
	}
	else 
	{
		if (release_sda)
		{
		    //i2c_sda_io.write(sc_logic('z'));
		}
		else
		{
		    i2c_sda_io.write(sc_logic(sda));
		}
		i2c_scl_io.write(sc_logic(scl));
		if (state == NONE)
			data_out_bo.write(data_out);
	}
}
