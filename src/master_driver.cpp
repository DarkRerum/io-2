#include <systemc.h>
#include <master_driver.h>

using namespace sc_dt;

enum {
    MAX_ADDRESS_INDEX = 6,
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
	STOP_MIDDLE,
	STOP_END
} transaction_state_t;

typedef enum {
	ADDRESS,
	WDATA,
	RDATA
} data_type_t;


static sc_uint<32> address_reg;
static sc_uint<32> start_reg;
static sc_uint<32> ready_reg;
static sc_uint<32> data_out_reg;

static sc_uint<32>* select_reg(sc_uint<8> address) 
{
	switch (address)
	{
		case 0x04: return &address_reg;
		case 0x08: return &start_reg;
		case 0x0C: return &ready_reg;
		case 0x10: return &data_out_reg;
		default: return &address_reg;
	}
}

void master_driver::handle_amba()
{
	static sc_uint<32>* preg;
	static bool address = true;
	static bool write = false;
	if (!HRESETn_i.read())
    {
		address_reg = 0;
		start_reg = 0;
		ready_reg = 0x00000001;
		data_out_reg = 0;
		address = true;
		write = false;
	}
	else
	{
		if (address)
		{
			address = false;
			if (write && HSEL_i.read()) {
				*preg = HWDATA_bi.read();
				std::cout << "address_reg: " << address_reg.to_string(SC_HEX) <<
				" start_reg: " << start_reg.to_string(SC_HEX) <<
				" ready_reg: " << ready_reg.to_string(SC_HEX) <<
				" data_out_reg: " << data_out_reg.to_string(SC_HEX) << endl;
			}
		}
		else
		{
			address = true;
			preg = select_reg(HADDR_bi.read().range(7,0));
			write = HWRITE_i.read();
			if (HSEL_i.read() && !write) 
			{
				HRDATA_bo.write(*preg);
			}
		}
	
	}
}

/*void master_driver::handle_amba()
{
	static sc_uint<32>* preg;
	static bool address = true;
	static bool write = true;
	if (!HRESETn_i.read())
    {
		address_reg = 0;
		start_reg = 0;
		ready_reg = 0x00000001;
		data_out_reg = 0;
		address = true;
		write = true;
	}
	else
	{
		if (address)
		{
			preg = select_reg(HADDR_bi.read().range(7,0));
			write = HWRITE_i.read();
			address = false;
			HRDATA_bo.write(0xAAAAAAAA);
		}
		else if (write)
		{
			if (HSEL_i.read()) {
				*preg = HWDATA_bi.read();
				std::cout << "address_reg: " << address_reg.to_string(SC_HEX) <<
				" start_reg: " << start_reg.to_string(SC_HEX) <<
				" ready_reg: " << ready_reg.to_string(SC_HEX) <<
				" data_out_reg: " << data_out_reg.to_string(SC_HEX) << endl;
					HRDATA_bo.write(0xBBBBBBBB);
				address = true;
			}
			
		}
		else
		{	
			if (HSEL_i.read()) {
							HRDATA_bo.write(0xCCCCCCCC);
				address = true;
				//HRDATA_bo.write(*preg);
			}
		}
	
	}
}*/

static transaction_state_t state;
static data_type_t type;
static bool release_sda;
static int bit_index;
static int bytes_remaining;
static int max_all_data_index;
static sc_uint<16> data_out;
static sc_uint<24> data_in;
static sc_uint<7> addr_in;
static bool read_not_write;
static bool send_stop;

static bool sda;
static bool scl;

static sc_uint_bitref & start_bit() 
{
	return start_reg[28];
}

static sc_uint_bitref & send_stop_bit()
{
	return start_reg[27];
}

static sc_uint_bitref & ready_bit() 
{
	return ready_reg[0];
}

static sc_uint_bitref & read_not_write_bit() 
{
	return start_reg[24];
}

static sc_uint_subref & message_size()
{
	return start_reg.range(26, 25);
}

static sc_uint_subref & data_to_write() 
{
	return start_reg.range(23, 0);
}
static sc_uint_subref & address() 
{
	return address_reg.range(6, 0);
}

void master_driver::listen_start()
{
	if (!HRESETn_i.read())
    {
		data_out = 0;
        sda = 1;
		scl = 1;
        state = NONE;
		bit_index = 0;
		release_sda = 0;
		ready_bit() = 1;
		type = ADDRESS;
		data_out = 0;
		send_stop = 0;
		bytes_remaining = 0;
		max_all_data_index = 0;
    }
	else if (start_bit())
	{
		start_bit() = 0;
		ready_bit() = 0;
		state = START_BEGIN;
		release_sda = 0;
        bit_index = 0;
        state = START_BEGIN;
        type = ADDRESS;
		data_out = 0;
		
		send_stop = send_stop_bit();
		data_in = data_to_write();
		addr_in = address();
		read_not_write = read_not_write_bit();
		
		bytes_remaining = message_size();
		max_all_data_index = ( message_size() * 8 ) - 1;
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
				if (bytes_remaining == 0 && bit_index == ONE_PAST_LAST_INDEX) 
				{
					state = STOP_BEGIN;
				}
				else if (bit_index == ONE_PAST_LAST_INDEX)
				{
					state = PAUSE;
					
                    if (type == ADDRESS)
					{
						type = (read_not_write) ? RDATA : WDATA;					
                    }
                    bit_index = 0;
				}
				else
				{
					state = PAUSE;
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
								sda = data_in[max_all_data_index];
                            } break;
                            case ADDRESS:
							{
                                sda = (bit_index == MAX_DATA_INDEX) ? read_not_write : addr_in[MAX_ADDRESS_INDEX - bit_index];
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
				if ((type == WDATA || type == RDATA) && bit_index == MAX_DATA_INDEX)
				{
					bytes_remaining--;
                }
				if ( bit_index != ACK_INDEX && type == WDATA) 
				{
					data_in <<= 1;
				}
				if ( bit_index != ACK_INDEX && type == RDATA) 
				{
					data_out <<= 1;
				}
			} break;
                
            case TRANSIT:
			{
                if (bit_index != ACK_INDEX &&type == RDATA)
				{	
					data_out[0] = (i2c_sda_io.read() == '1') ? 1 : 0;
                }
                bit_index = bit_index + 1;
                state = PAUSE_BEGIN;
			} break;
            
			case STOP_BEGIN:
			{
                release_sda = 0;
                sda = 0;
                state = STOP_MIDDLE;
            } break;
                
			case STOP_MIDDLE:
			{
                scl = (send_stop) ? 1 : 0;
				state = STOP_END;
			} break;
            case STOP_END:
			{
                sda = (send_stop) ? 1 : 0;
				ready_bit() = 1;
                state = NONE;
			} break;
		}
	}
}

void master_driver::output_selector()
{
	if (!HRESETn_i.read()) 
	{
		i2c_sda_io.write(sc_logic(1));
		i2c_scl_io.write(sc_logic(1));
		data_out_reg = 0;
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
			data_out_reg = data_out;
	}
}
