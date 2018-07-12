#include "RRAM.h"

#include "sender.h"

int sc_main(int argc, char* argv[])
{
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
	RRAM rram("rram");
	sender controller("controller");

	sc_signal<sc_logic, SC_MANY_WRITERS> data_in;
	sc_signal<sc_logic, SC_MANY_WRITERS> data_out;

	sc_signal<sc_bit> cs;
	sc_signal<bool> clk;
	
	sc_trace_file* tracefile;
	tracefile = sc_create_vcd_trace_file("waveform");
	
	rram.data_in_p(data_in);
	rram.data_out_p(data_out);
	rram.clk_p(clk);
	rram.cs_p(cs);
	controller.bit_out(data_in);
	controller.bit_in(data_out);
	controller.clk(clk);
	controller.cs_pin(cs);

	sc_trace(tracefile,cs,"cs");
	sc_trace(tracefile,clk,"clk");
	sc_trace(tracefile,data_in,"DI");
	sc_trace(tracefile,data_out,"DO");

	sc_start();

	sc_close_vcd_trace_file(tracefile);	

	return 0;
}
