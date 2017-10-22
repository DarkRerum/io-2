#include <systemc.h>
#include <master_driver.h>

enum {
    MAX_ADDRESS_INDEX = 6,
	MAX_DATA_INDEX = 7,
	ACK_INDEX = 8,
	ONE_PAST_LAST_INDEX = 9
};

typedef enum {
	NONE,
	START,
	PAUSE_BEGIN,
	PAUSE,
	PAUSE_END,
	TRANSIT,
	STOP_BEGIN,
	STOP_END
} transaction_state_t;

typedef enum {
	ADDRESS,
	WDATA,
	RDATA
} data_type_t;

static transaction_state_t state;
static data_type_t type;
static bool release_sda;
static int bit_index;
static sc_uint<8> data_out;

static bool sda;
static bool scl;

void master_driver::listen_start()
{
	if (rst_i.read())
    {
		data_out = 0;
        sda = 1;
		scl = 1;
        state = NONE;
		bit_index = 0;
		error_o.write(0);
		release_sda = 0;
		ready_o.write(1);
		type = ADDRESS;
		data_out = 0;
    }
	else if (start_i.read())
	{
		ready_o.write(0);
		state = START;
		release_sda = 0;
        bit_index = 0;
        state = START;
        type = ADDRESS;
		data_out = 0;
	}
	else 
	{
		switch(state) 
		{
			case NONE: break;
			
			case START: 
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
					release_sda = read_not_write_i.read();
                    if (type == RDATA || type == WDATA)
					{
                        state = STOP_BEGIN;
                    }
                    else if (type == ADDRESS)
					{
                        type = (read_not_write_i.read()) ? RDATA : WDATA;
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
						sda = (type == RDATA) ? 1 : 0;
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
                                sda = ( data_in_bi.read() )[MAX_DATA_INDEX - bit_index];
                            } break;
                            case ADDRESS:
							{
                                sda = (bit_index == MAX_DATA_INDEX) ? read_not_write_i.read() : ( addr_in_bi.read() )[MAX_ADDRESS_INDEX - bit_index];
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
                    if (type != RDATA && i2c_sda_io.read() != '0')
					{
                        error_o.write(1);
                    }
                }
                else if (type == RDATA)
				{
					data_out[MAX_DATA_INDEX - bit_index] = (i2c_sda_io.read() == '1') ? 1 : 0;
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
		data_out_bo.write(sc_uint<8>(0));
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
