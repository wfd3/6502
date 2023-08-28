// Emulated Apple 1
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <csignal>
#include <sstream>

#include <stdio.h>
#include <termio.h>
#include <unistd.h>

#include "./6502.h"
#include "./memory.h"

// Uncomment to start in 6502-emulator debugger
//#define DEBUG 

Memory mem(CPU::MAX_MEM);
CPU cpu(&mem);

constexpr unsigned long wozmonAddress = 0xff00;
constexpr unsigned long basicAddress  = 0x6000;

#define STDIN 0

void signalHandler( int signum ) {
	switch (signum) {
	case SIGQUIT:		// Reset computer
		printf("\n");
		cpu.setPendingReset();
		break;

	case SIGUSR1:		// Toggle the 6502 emulator's debugger
		cpu.ToggleDebug();
		break;
	}

}

void enable_raw_mode()
{
    termios term;
    fflush(stdout);
    if (tcgetattr(STDIN, &term) < 0)
	    perror("1 tcsetattr()");
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN, TCSANOW, &term) < 0)
	    printf("2 tcsetattr()");
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    tcflush(0, TCIFLUSH); 

}

void disable_raw_mode()
{
    termios term;
    tcgetattr(STDIN, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

void dspwrite(unsigned char c) {
	c &= 0x7f;		// clear hi bit
	switch (c) {
	case 0x0d:		// \r
		printf("\n");
		break;
	case '_':		// Backspace
		printf("\b");
		break;
	case 0x07:		// Bell
		printf("\a");
		break;
	default:
		if (isprint(c)|| isspace(c))
			printf("%c", c);
	}
}

unsigned char dspread() {
	return 0x7f;
}

bool kbdCharPending = false;
char kbdCharacter = 0;

unsigned char kbdcr_read() {
	int byteswaiting;

	if (kbdCharPending)
		return kbdCharacter;

	ioctl(STDIN, FIONREAD, &byteswaiting);
	if (byteswaiting == 0)
		return 0;

	read(STDIN, &kbdCharacter, 1);

	// Map modern ascii to Apple 1 keycodes
	switch (kbdCharacter) {
	case '\n':
		kbdCharacter = 0x0d;
		break;
 	case 0x7f:		         // backspace
		kbdCharacter = '_';
		break;
	case 0x02:			 // Ctrl-B
		kbdCharacter = 0x03; // Fake out a ^C
		break;
	default:
		kbdCharacter = toupper(kbdCharacter);
		break;
	}


	// This value will get LDA's into A, setting the processor
	// status bits in the process.  Apple 1 expect that the
	// Negative Flag will be set if there are characters pending
	// read from the keyboard, hence the bitwise or with 0x80.
	
	kbdCharacter |= 0x80;
	kbdCharPending = true;
		
	return kbdCharacter;
}

unsigned char kbdread() {
	if (!kbdCharPending) {
		kbdcr_read();
		// If there's a pending character here, return it but do not
		// toggle the kbdCharPending flag.
		//
		// Applesoft Basic lite does a blind, unchecked read
		// on the keyboard port looking for a ^C.  If it sees
		// one, it then does a read on the keyboard control
		// register, followed by a read of the keyboard port,
		// expecting to get the same ^C.  We need this
		// logic to allow that behavior to happen w/out blocking.
		if (kbdCharPending)
			return kbdCharacter;
		return 0;
	}
	
	kbdCharPending = false;
	return kbdCharacter;
}

int main () {
	// register signal SIGINT and signal handler  
	signal(SIGQUIT, signalHandler);
	signal(SIGUSR1, signalHandler);

	mem.mapRAM(0, 0xd00f);
	// Keyboard and display memory-mapped IO
	mem.mapMIO(0xd010, kbdread, NULL);
	mem.mapMIO(0xd011, kbdcr_read, NULL);
	mem.mapMIO(0xd012, dspread, dspwrite);
	mem.mapMIO(0xd013, NULL, NULL);
	mem.mapRAM(0xf014, 0xffff);

	// And load WozMon and Applesoft Basic
	printf("# Loading wozmon at %04lx\n", wozmonAddress);
	mem.loadDataFromFile("./binfiles/wozmon.bin", wozmonAddress);
	printf("# Loading Applesoft Basic I at %04lx\n", basicAddress);
	mem.loadDataFromFile("./binfiles/applesoft-lite-0.4-ram.bin",
			     basicAddress);

	printf("# Loading Apple I sample program at 0\n");
	std::vector<unsigned char> wozProg =
		{0xa9, 0x00, 0xaa, 0x20, 0xef,0xff, 0xe8, 0x8a, 0x4c, 0x02,
		 0x00};
	mem.loadData(wozProg, 0);

	printf("\n");

	// Reset the CPU, jump to Wozmon
	cpu.setResetVector(wozmonAddress);
	cpu.exitReset();

#ifdef DEBUG
	cpu.SetDebug(true);)
	cpu.Execute();
#else
	enable_raw_mode();
	while (1) {
		if (cpu.isDebugEnabled()) {
			disable_raw_mode();
			cpu.Debug();
			enable_raw_mode();
		} else 
			cpu.ExecuteOneInstruction();
	}
	disable_raw_mode();
#endif
	return 0;
}
