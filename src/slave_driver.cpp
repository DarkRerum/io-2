#include <systemc.h>

#include <slave_driver.h>

static bool stop_detected;
static bool start_detected;

static bool sda;
static bool release_sda;

void slave_driver::emulate_slave()
{
	enum {IDLE, WAIT_SCL0_INIT, WAIT_SCL1, WAIT_SCL0};	
	enum {ADDRESS, WDATA, RDATA};
	static const int MAX_DATA_INDEX = 7;
	static const int ACK_INDEX = 8;

	static sc_uint<16> data_in;
	static int exec_state;
	static int data_type;
	static sc_uint<16> data;
	static int bit_index;
	static bool ack;
	
	//data_out_bo.write(exec_state);
	
    if (rst_i.read())
    {	
		exec_state = IDLE;
		sda = 1;
		release_sda = 1;
		//ready_o.write(1);// DEBUG
    }
    else
    {
		if (stop_detected)	
		{
			//ready_o.write(1); // DEBUG
            exec_state = IDLE;
			release_sda = 1;
			/*if (data_type == RDATA && ack == 0)
			{
				ready_o.write(0);
			}*/
		}
		else 
		{
			//data_out_bo.write(exec_state);
			//ready_o.write(i2c_scl_i.read() == '1' ? 1 : 0);
			//ready_o.write(release_sda);
			//data_out_bo.write(data_type);
			//data_out_bo.write(bit_index);
			switch (exec_state)
			{
			case (IDLE):
				release_sda = 1;
				if (start_detected)
				{
					data_in = data_in_bi.read();
					exec_state = WAIT_SCL0_INIT;
					data_type = ADDRESS;
					//ready_o.write(0);// DEBUG
					ack = 1;
				}
				break;

			case (WAIT_SCL0_INIT):
				if (i2c_scl_i.read() == '0') 
				{
					exec_state = WAIT_SCL1;
					bit_index = 0;
					data = 0;
				}
				break;

			case (WAIT_SCL1):
				if (i2c_scl_i.read() == '1')
				{
					exec_state = WAIT_SCL0;
					if (data_type != RDATA && bit_index != ACK_INDEX)
					{
						
						data[MAX_DATA_INDEX - bit_index] = (i2c_sda_io.read() == '0') ? 0 : 1;
					}
					if (data_type == RDATA && bit_index == ACK_INDEX)
					{
						ack = (i2c_sda_io.read() == '0') ? 0 : 1;
					}
					if (bit_index == ACK_INDEX)
					{
						bit_index = 0;
						data_out_bo.write(data);
					}
					else
					{
						bit_index++;
					}
				}
				break;

			case (WAIT_SCL0):
				if (start_detected)
				{
					exec_state = WAIT_SCL0_INIT;
					data_type = ADDRESS;
					ack = 1;
				}
				else if (i2c_scl_i.read() == '0') 
				{
					exec_state = WAIT_SCL1;
					if (bit_index == ACK_INDEX)
					{
						release_sda = (data_type == RDATA) ? 1 : 0;
						if (data_type != RDATA)
						{
							sda = 0;
						}
						if (data_type == ADDRESS)
						{
							data_type = (data[0] == 1) ? RDATA : WDATA;
						}
					}
					else
					{
						if (data_type == RDATA && ack == 1)
						{
							release_sda = 1;
						}
						else
						{
							release_sda = (data_type == RDATA) ? 0 : 1;
						}
						if (data_type == RDATA)
						{
							sda = data_in[MAX_DATA_INDEX - bit_index];
						}
					}
				}
				break;
			}
		}
    }
}


void slave_driver::listen_start()
{
	enum {WAIT_SCL1, WAIT_SDA1, WAIT_SDA0};

	static int start_state;
	
	if (rst_i.read())
	{
		start_state = WAIT_SCL1;
		start_detected = false;
	}
	else 
	{
		start_detected = false;
		switch (start_state) {
		    case WAIT_SCL1:
		        if (i2c_scl_i.read() == 1 && i2c_sda_io.read() == 0) start_state = WAIT_SDA1;
		        if (i2c_scl_i.read() == 1 && i2c_sda_io.read() == 1) start_state = WAIT_SDA0;
		    	break;

		    case WAIT_SDA1:
		        if (i2c_scl_i.read() == 0) start_state = WAIT_SCL1;
		        if (i2c_scl_i.read() == 1 && i2c_sda_io.read() == 1) start_state = WAIT_SDA0;
				break;

		    case WAIT_SDA0:
				if (i2c_scl_i.read() == 0)
				{
					start_state = WAIT_SCL1;
				}
		        if (i2c_scl_i.read() == 1 && i2c_sda_io.read() == 0)
				{
		            start_detected = 1;
		            start_state = WAIT_SCL1;
				}
		        break;

		    default:
		        start_state = WAIT_SCL1;
				break;
		}
	}
}

void slave_driver::listen_stop()
{
	enum {WAIT_SCL1, WAIT_SDA1, WAIT_SDA0};

	static int stop_state;	

	if (rst_i.read())
	{
		stop_detected = 0;
		stop_state = WAIT_SCL1;
	}
    else
	{
		stop_detected = 0;
            
        switch (stop_state)
		{
            case WAIT_SCL1:
                if (i2c_scl_i.read() == 1 && i2c_sda_io.read() == 0) stop_state = WAIT_SDA1;
				break;

            case WAIT_SDA1:
                if (i2c_scl_i.read() == 1 && i2c_sda_io.read() == 1)
				{
                    stop_detected = 1;
                    stop_state = WAIT_SCL1;
                }
                else if (i2c_scl_i.read() == 0)
				{
                    stop_state = WAIT_SCL1;
                }
				break;

            default:
                stop_state = WAIT_SCL1;
				break;
        }
	}
}

void slave_driver::output_selector()
{
    if (rst_i.read())
    {	
        //ready_o.write(true);
    }
    else
    {
		if (start_detected)
		{
			ready_o.write(1);
		}
		else
						ready_o.write(0);
		if (!release_sda) 
		{
			i2c_sda_io.write(sc_logic(sda));
		}
		else
		{
			//i2c_sda_io.write(sc_logic('z'));
		}
    }
}
