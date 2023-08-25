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
	
	mem.loadDataFromFile("./tests/6502_functional_test.bin", 0x0000);
	cpu.setResetVector(0x0400);
	cpu.exitReset();
	constexpr Word exitAddress = 0x3469;
	cpu.setExitAddress(exitAddress);
	cpu.toggleLoopDetection(); // Force break on 'jmp *'

	cpu.Execute();
	
	return 0;
}
