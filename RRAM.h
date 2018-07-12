#ifndef RRAM_H
#define RRAM_H

#include <systemc>
#include <iostream>
#include <iomanip>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE(RRAM)
{
	sc_in<sc_logic> data_in_p;
	sc_out<sc_logic> data_out_p;
	sc_in<sc_bit> cs_p;
	sc_in<bool> clk_p;

	#define instruction_read "0b000000011"
	#define instruction_write_enable "0b000000110"
	#define instruction_program_page "0b000000010"
	#define instruction_page_write "0b000001010"
	#define instruction_page_erase "0b011011011"
	#define instruction_sector_erase "0b000100000"
	#define instruction_chip_erase "0b001100000"
	#define instruction_read_status_register "0b000000101"

	sc_bit data[256][2048];
	sc_bit page_buffer[2048];
	sc_logic bit;
	sc_bv<24> address;
	int address_int;
	sc_bv<8> instruction;
	sc_bit cs_val;

	bool cs_low;
	bool cs_wait;
	bool instruction_valid;

	sc_time time_program_page;
	sc_time time_page_write;
	sc_time time_page_erase;

	sc_lv<8> status_register_1;
	sc_lv<8> status_register_2;

	sc_event cs_read;
	sc_event begin_read;
	sc_event begin_write_enable;
	sc_event begin_program_page;
	sc_event begin_page_write;
	sc_event begin_page_erase;
	sc_event begin_read_status_register;
	sc_event ins_read;
	
	SC_CTOR(RRAM):
		cs_low(false),
		instruction_valid(false),
		cs_wait(false),
		status_register_1("0b000000000"),
		status_register_2("0b000000000"),
		time_program_page(1,SC_MS),
		time_page_write(1,SC_MS),
		time_page_erase(1,SC_MS)
	{
		SC_METHOD(start);
			sensitive << cs_p.value_changed();
		SC_THREAD (read_data);
			sensitive << clk_p.neg();
			dont_initialize();
		SC_THREAD(read_instruction);
			sensitive << ins_read;
			dont_initialize();
		SC_THREAD(write_enable);
		SC_THREAD(program_page);
		SC_THREAD(page_write);
		SC_THREAD(page_erase);
		SC_THREAD(read_status_register);
			sensitive << clk_p.neg();
		
		data_out_p.initialize(sc_logic('Z'));

		for (int i=0;i<256;i++)
		{
			for(int j=0;j<2048;j++)
			{
				data[i][j]=SC_LOGIC_1;
			}
		}
		for(int j=0;j<2048;j++)
		{
			page_buffer[j]=SC_LOGIC_1;
		}
	}
	void start(void);
	void read_data(void);
	void read_instruction(void);
	void write_enable(void);
	void program_page(void);
	void page_write(void);
	void page_erase(void);
	void read_status_register(void);
};
#endif
