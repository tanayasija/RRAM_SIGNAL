#include "RRAM.h"
#include <cmath>

void RRAM::start()
{
	cs_val = cs_p->read();
	if (!cs_low & cs_val==SC_LOGIC_0)
	{
		cout << "CS set low" << endl;
		cs_low= true;
		ins_read.notify();
	}
	else if (cs_low & cs_val==SC_LOGIC_1)
	{
		cout << "CS set high" << endl;
		cs_low=false;
	}
	next_trigger(cs_p->default_event() & clk_p->posedge_event());
}

void RRAM::read_instruction()
{
	for (;;)
	{
		cout << "Reading instruction" << endl;
		int i=0;
		while (cs_low == true && i<8)
		{
			wait(clk_p.posedge_event() | cs_p->default_event());
			if(cs_low == false)
			{
				break;
			}
			wait(SC_ZERO_TIME);
			cout << "Reading instruction bit " << i+1  << endl;
			instruction[7-i] = data_in_p->read();
			i++;
			cout << "Instruction bit " << i << " read as " << instruction[7-i] << endl;
		}		
		cout << "Instruction read " << instruction.to_string(SC_BIN) << endl;

		if (i==8)
		{
			if (instruction.to_string(SC_BIN)==instruction_read)
			{	
				if (status_register_1[0]==SC_LOGIC_0)
				{
					begin_read.notify();
				}
				else
				{
					cout << "Instruction ignored" << endl;
				}			
			}
			else if (instruction.to_string(SC_BIN)==instruction_write_enable)
			{
				if (status_register_1[0]==SC_LOGIC_0)
				{
					begin_write_enable.notify();
					cout << "Write enable instruction read" << endl;
				}
				else
				{
					cout << "Instruction ignored" << endl;
				}
			} 
			else if (instruction.to_string(SC_BIN)==instruction_program_page)
			{
				if (status_register_1[1]==1 && status_register_1[0]==SC_LOGIC_0)
				{
					begin_program_page.notify();
				}
				else
				{
					cout << "Instruction ignored" << endl;
				}
			}
			else if (instruction.to_string(SC_BIN)==instruction_page_write)
			{
				if (status_register_1[0]==SC_LOGIC_0 && status_register_1[1]==SC_LOGIC_1)
				{
					begin_page_write.notify();
					cout << "Write enable instruction read" << endl;
				}
				else
				{
					cout << "Instruction ignored" << endl;
				}
			} 
			else if (instruction.to_string(SC_BIN)==instruction_page_erase)
			{
				if (status_register_1[1]==1 && status_register_1[0]==SC_LOGIC_0)
				{
					begin_page_erase.notify();
				}
				else
				{
					cout << "Instruction ignored" << endl;
				}
			}
			else if (instruction.to_string(SC_BIN)==instruction_read_status_register)
			{
				begin_read_status_register.notify();
			}
			else
			{
				cout << "Instruction not recognized, ignoring instruction" << endl;
			} 
		}
		else
		{
			cout << "Instruction not recognized, ignoring instruction" << endl;
		}
		wait();
	}
}

void RRAM::read_data()
{	
	for(;;)
	{
		wait(begin_read);
		cout << "Page read started" << endl;
		int j=0;
		address_int = 0;

        while(cs_low && j<24)
        {
        	wait(clk_p->posedge_event() | cs_p->default_event());
        	if(cs_low == false)
        	{
        		break;
        	}
			wait(SC_ZERO_TIME);
        	address[23-j] = data_in_p->read();
            j++;
        }
        if (cs_low && j==24)
        {
            for (int k=0;k<16;k++)
            {
				sc_bit add_bit = (sc_bit)address[k];
				if (add_bit==SC_LOGIC_1)
                {	
					address_int +=(int)pow(2.0,15-k) ;
            	}
	   		}	
		}
		cout << "Starting read operation" << endl;	
		int row=address_int/256;
		cout << row << endl;
		int col=address_int%256;
		int i=col*8;
		cout << "Sending data at location " << endl; cout<< "Row " << setw(2) << "Column " << setw(2) << "Data" << endl;		
		cout << row << " " << col << " " << i << endl;	
		while (cs_low)
		{  
			cout << "Waiting for negative clock edge" << endl;
			wait(clk_p->negedge_event() | cs_p.default_event());
			if (cs_low==false)
			{
				break;
			}
			bit = data[row][i];
			cout << setw(3) <<  row << setw(8) << i << setw(6) << bit << endl;
			data_out_p->write(bit);
			i++;
			if (2048<=i)
			{
				i=0;
				cout << "Moving to next row" << endl ;
				++row;		
			}
			if (256<=row)
			{
				cout << "Moving to the first cell" << endl ;
				row=0;
			}		
		}
		data_out_p->write(sc_logic('Z'));
		cout << "CS set HIGH, ending read operation" << endl;		
	}
}

void RRAM::write_enable(void)
{
	for(;;)
	{
		wait(begin_write_enable);
		status_register_1[1]=SC_LOGIC_1;
		cout << "Write enable bit set" << endl;
	}
}

void RRAM::program_page(void)
{
	for(;;)
	{	
		wait(begin_program_page);
		cout << "Page program started" << endl;
		int j=0;
		address_int = 0;

         while(cs_low && j<24)
        {
        	wait(clk_p->posedge_event() | cs_p->default_event());
        	if(cs_low == false)
        	{
        		break;
        	}
			wait(SC_ZERO_TIME);
        	address[23-j] = data_in_p->read();
            j++;
        }
        if (cs_low && j==24)
        {
            for (int k=0;k<16;k++)
            {
				sc_bit add_bit = (sc_bit)address[k];
				if (add_bit==SC_LOGIC_1)
                {	
					address_int +=(int)pow(2.0,15-k) ;
            	}
	   		}	
		}
		cout << "Setting busy bit to 1" << endl;
		status_register_1[0]=SC_LOGIC_1;

		int row=address_int/256;
		int col=address_int%256;
		int i=col*8;

		
		cout << row << " " << col << " " << i << endl;	
		sc_bit program_data;
		bool over_write = false;
		
		for (int j=0;j<2048;j++)
		{
			page_buffer[j]=data[row][j];
			
		}
		cout << "Deepcopy created" << endl;
		int num=0;
		
		while (cs_low)
		{
			if (2048<=i)
			{
				cout << "Moving to initial cell" << endl ;
				i=0;
				over_write = true;
			}
			wait(clk_p->posedge_event() | cs_p->default_event());
			if (cs_low == false)
			{
				break;
			}
			wait(SC_ZERO_TIME);
			program_data = data_in_p->read();
			if (data[row][i]==SC_LOGIC_1 && program_data==SC_LOGIC_0)
			{
				page_buffer[i] = SC_LOGIC_0;
			}
			else if (over_write==true)
			{
				page_buffer[i] = program_data;
			}
			num++;
			i++;
		}
		cout << "CS set high, checking complete byte transfer with number of bytes transferred " << num  << endl;
		if ((i-col*8)%8==0)
		{
			for (int j=0;j<2048;j++)
			{
				data[row][j]=page_buffer[j];
				
			}
			cout << "Programming data" << endl;
			wait(time_program_page);
			cout << "Programmed bits successfully" << endl;
		}
		status_register_1[0]=SC_LOGIC_0;
		cout << "Busy bit set to 0" << endl;
		status_register_1[1]=SC_LOGIC_0;
		cout << "Write enable latch set to 0" << endl;
	}
}

void RRAM::page_write(void)
{
	for(;;)
	{	
		wait(begin_page_write);
		cout << "Page write started" << endl;
		int j=0;
		address_int = 0;

         while(cs_low && j<24)
        {
        	wait(clk_p->posedge_event() | cs_p->default_event());
        	if(cs_low == false)
        	{
        		break;
        	}
			wait(SC_ZERO_TIME);
        	address[23-j] = data_in_p->read();
            j++;
        }
        if (cs_low && j==24)
        {
            for (int k=0;k<16;k++)
            {
				sc_bit add_bit = (sc_bit)address[k];
				if (add_bit==SC_LOGIC_1)
                {	
					address_int +=(int)pow(2.0,15-k) ;
            	}
	   		}	
		}
		cout << "Setting busy bit to 1" << endl;
		status_register_1[0]=SC_LOGIC_1;

		int row=address_int/256;
		int col=address_int%256;
		int i=col*8;

		cout << row << " " << col << " " << i << endl;
		sc_bit program_data = sc_bit('1');	
		for (int j=0;j<2048;j++)
		{
			page_buffer[j]=data[row][j];
			
		}
		cout << "Deepcopy created" << endl;

		while (cs_low)
		{
			if (2048<=i)
			{
				cout << "Moving to initial cell" << endl ;
				i=0;
			}
			wait(clk_p->posedge_event() | cs_p->default_event());
			if (cs_low == false)
			{
				break;
			}
			wait(SC_ZERO_TIME);
			program_data = data_in_p->read();
			page_buffer[i]=program_data;
			i++;
		}
		if ((i-col*8)%8==0)
		{
			for (int k=0;k<2048;k++)
			{
				data[row][k]=page_buffer[k];
			}
		
			cout << "Writing data" << endl;
			wait(time_page_write);
			cout << "Written bits successfully" << endl;
		}
		status_register_1[0]=SC_LOGIC_0;
		cout << "Busy bit set to 0" << endl;
		status_register_1[1]=SC_LOGIC_0;
		cout << "Write enable latch set to 0" << endl;
	}
}

void RRAM::page_erase(void)
{
	for(;;)
	{	
		wait(begin_page_erase);
		cout << "Page erase started" << endl;
		int j=0;
		address_int = 0;

       while(cs_low && j<24)
        {
        	wait(clk_p->posedge_event() | cs_p->default_event());
        	if(cs_low == false)
        	{
        		break;
        	}
			wait(SC_ZERO_TIME);
        	address[23-j] = data_in_p->read();
            j++;
        }
        if (cs_low && j==24)
        {
            for (int k=0;k<16;k++)
            {
				sc_bit add_bit = (sc_bit)address[k];
				if (add_bit==SC_LOGIC_1)
                {	
					address_int +=(int)pow(2.0,15-k) ;
            	}
	   		}	
		}
		if (j==24)
		{
			cout << "Setting busy bit to 1" << endl;
			status_register_1[0]=SC_LOGIC_1;

			int row=address_int/256;
			int col=address_int%256;
			int i=col*8;

			cout << row << " " << col << " " << i << endl;
			for (int k=0;k<2048;k++)
			{
				data[row][k]=SC_LOGIC_1;
			}

			cout << "Erasing data" << endl;
			wait(time_page_write);
			cout << "Erased bits successfully" << endl;
		}
		status_register_1[0]=SC_LOGIC_0;
		cout << "Busy bit set to 0" << endl;
		status_register_1[1]=SC_LOGIC_0;
		cout << "Write enable latch set to 0" << endl;
	}
}

void RRAM::read_status_register(void)
{
	for(;;)
	{
		wait(begin_read_status_register);

		int i=0;
		while(cs_low)
		{
			wait(clk_p->negedge_event() | cs_p.default_event());
			if (cs_p.event() && cs_low ==false)
			{
				break;
			}
			sc_logic status_data =(sc_logic) status_register_1[7-i];
			data_out_p->write(status_data);
			i++;
			if (i==8)
			{
				i=0;
			}
		}
		data_out_p->write(sc_logic('Z'));
		cout << "Exiting read status register" << endl;
	}
}
