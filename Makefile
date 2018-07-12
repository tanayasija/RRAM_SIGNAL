all: test.exe

test.exe: sender.o  RRAM.o main.o
	g++ -L$(SYSTEMC_HOME)/lib-linux64 main.o sender.o  RRAM.o -lsystemc -o test.exe

RRAM.o: RRAM.cpp RRAM.h
	g++ -c -I$(SYSTEMC_HOME)/include RRAM.cpp

sender.o: sender.cpp sender.h
	g++ -c -I$(SYSTEMC_HOME)/include sender.cpp

main.o: main.cpp RRAM.h sender.h
	g++ -c -I$(SYSTEMC_HOME)/include main.cpp

run:
	./test.exe
clean:
	rm ./*.o ./test.exe ./*.vcd ./*.log*
						
