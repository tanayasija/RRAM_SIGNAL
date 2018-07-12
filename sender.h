#ifndef sender_H
#define sender_H

#include <systemc>
using namespace std;
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(sender)
{
	sc_out<bool> clk;
	sc_out<sc_bit> cs_pin;
	sc_out<sc_logic> bit_out;
	sc_in<sc_logic> bit_in;

	sc_event clk_posedge;
	SC_CTOR(sender)
	{
		SC_THREAD(transciever);
		sensitive <<clk_posedge;
		SC_THREAD(clock);
		bit_out.initialize(sc_logic('Z'));	
		cs_pin.initialize(sc_bit('1'));
	}
	void transciever(void);
	void clock(void);
};
#endif
