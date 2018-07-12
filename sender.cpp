#include "sender.h"

void sender::transciever()
{
	wait();
	wait(5,SC_NS);
	cs_pin->write(sc_bit('0'));
	for (int i=0;i<7;i++){
		wait();
		cout << "Sending instruction bit " << i+1 << endl;
		bit_out->write(sc_logic('1'));
	}
	wait();
	bit_out->write(sc_logic('0'));
	wait(5,SC_NS);
	cs_pin->write(sc_bit('1'));
	cout << "1 " <<  sc_time_stamp() << endl;
	wait();
	bit_out->write(sc_logic('Z'));
	wait(5,SC_NS);
	cs_pin->write(sc_bit('0'));
	sc_bv<8> read_ins = "11000000";
	for (int i=0;i<8;i++){
		wait();
		sc_logic r = (sc_logic)read_ins[i];
		cout << "Sending instruction bit " << i+1 << endl;
		bit_out->write(r);
	}
	cout << "2 " <<  sc_time_stamp() << endl;
	for (int i=0;i<24;i++){
		wait();
		bit_out->write(sc_logic('1'));
	}
	cout << sc_time_stamp() << endl;
	for (int i=0;i<24;i++){
		wait(clk.negedge_event());
		if (i==0) bit_out->write(sc_logic('Z'));
		wait(SC_ZERO_TIME);
		if (bit_in->read()!= SC_LOGIC_Z)
		{
			sc_logic d = bit_in->read();
			cout << "Bit " << i << " read at time "<< sc_time_stamp() << " "  << d << endl;
		}
		if (i==20)
		{
			cs_pin->write(sc_bit('1'));
			break;	
		}
	}
	wait();
	wait(5,SC_NS);
	cs_pin->write(sc_bit('0'));
	sc_bv<8> write_enable_ins = "01100000";
	for (int i=0;i<8;i++){
		wait();
		sc_logic r = (sc_logic)write_enable_ins[i];
		bit_out->write(r);
	}
	wait(5,SC_NS);
	cs_pin->write(sc_bit('1'));
	wait();
	bit_out->write(sc_logic('Z'));
	wait(5,SC_NS);
	cs_pin->write(sc_bit('0'));
	sc_bv<8> write_ins = "01000000";
	for (int i=0;i<8;i++){
		wait();
		sc_logic r = (sc_logic)write_ins[i];
		bit_out->write(r);
	}
	cout <<  sc_time_stamp() << endl;
	for (int i=0;i<24;i++){
		wait();
		bit_out->write(sc_logic('1'));
	}
	cout << sc_time_stamp() << endl;
	for (int i=0;i<8;i++){
		wait();
		sc_logic r = (sc_logic)write_ins[i];
		bit_out->write(r);
	}
	wait(5,SC_NS);
	cs_pin->write(sc_bit('1'));
	wait();
	bit_out->write(sc_logic('Z'));
	wait(5,SC_NS);
	cs_pin->write(sc_bit('0'));
	for (int i=0;i<8;i++){
		cout << i << endl ;
		wait();
		sc_logic r = (sc_logic)write_enable_ins[i];
		bit_out->write(r);
	}
	wait(5,SC_NS);
	cs_pin->write(sc_bit('1'));
	wait();
	bit_out->write(sc_logic('Z'));
	wait(5,SC_NS);
	sc_bv<8> ins_read_status = "10100000";
	cs_pin->write(sc_bit('0'));
	for (int i=0;i<8;i++){
		cout << i+1 << endl ;
		wait();
		sc_logic r = (sc_logic)ins_read_status[i];
		bit_out->write(r);
		cout << "Written instruction bit " << i+1 << endl;
	}
	for (int i=0;i<8;i++)
	{
		wait(clk.negedge_event());
		if (i==0) bit_out->write(sc_logic('Z'));
		wait(SC_ZERO_TIME);
		sc_logic sr_bit = SC_LOGIC_0;
		sr_bit = bit_in->read();
		cout << "Status regiter bit " << i+1 << " read at time " << sc_time_stamp() << " as " << sr_bit << endl;
	}
	wait();
	wait(5,SC_NS);
	cs_pin->write(sc_bit('1'));
	wait();
	bit_out->write(sc_logic('Z'));
	wait(5,SC_NS);
	cs_pin->write(sc_bit('0'));
	for (int i=0;i<8;i++){
		cout << i << endl ;
		wait();
		sc_logic r = (sc_logic)ins_read_status[i];
		bit_out->write(r);
	}
	for (int i=0;i<8;i++)
	{
		wait(clk.negedge_event());
		if (i==0) bit_out->write(sc_logic('Z'));
		wait(SC_ZERO_TIME);
		sc_logic sr_bit = SC_LOGIC_0;
		sr_bit = bit_in->read();
		cout << "Status regiter bit " << i+1 << " read at time " << sc_time_stamp() << " as " << sr_bit << endl;
	}
	wait();
	wait(5,SC_NS);
	cs_pin->write(sc_bit('1'));
	cout << sc_time_stamp() << endl;
}

void sender::clock(void)
{
	for(int i=0;i<26000;i++)
	{
		clk->write(0);
		wait(10,SC_NS);
		clk->write(1);	
		clk_posedge.notify();
		wait(10,SC_NS);
	}
}
