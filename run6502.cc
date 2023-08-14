#include <iostream>
#include <csignal>
#include <sstream>

#include "./6502.h"
#include "./memory.h"

Memory mem(CPU::MAX_MEM);
CPU cpu(&mem);

void signalHandler( int signum ) {
	printf("Debug toggle\n");
	if (signum == SIGQUIT)
		cpu.ToggleDebug();

}

int main () {
	// register signal SIGINT and signal handler  
	signal(SIGQUIT, signalHandler);
	
	mem.LoadProgramFromFile("./tests/6502_functional_test.bin", 0x0000);
	cpu.Reset(0x400);
	cpu.Debug();
	
	return 0;
}
