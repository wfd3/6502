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

unsigned char kbdcr_read() {
	int byteswaiting;

	ioctl(STDIN, FIONREAD, &byteswaiting);

#ifdef DEBUG
	byteswaiting = 1;
#endif
	// This value will get LDA's into A, setting the processor
	// status bits in the process.  Apple 1 expect that the
	// Negative Flag will be set if there are characters pending
	// read from the keyboard.
	if (byteswaiting > 0) 
		byteswaiting = 0x80;

	return byteswaiting;
}

unsigned char kbdread() {
	char c = 0;

#ifdef DEBUG
	// 'Stuff' keypresses into wozmon.  Don't forget 0x0d at the
	// end of keys[]
	static unsigned int i = 0;
	static char keys[] = { '6', '0','0', '0', 'R', 0x0d };
	                  // {'F', 'F', '0', '0', 0x0d};

	c = keys[i++];

	if (i > (sizeof(keys) / sizeof(keys[0])))
	    i = 0;
#else
	// Applesoft Basic does a blind read on the keyboard to check for ^C.
	// That forces us to check for a pending character to protect from a
	// blocking read.  That's probably a good thing to do anyway.
	if (kbdcr_read())
		read(0, &c, 1);
#endif

	// Map modern ascii to Apple 1 keycodes
	switch (c) {
	case '\n':
		c = 0x0d;
		break;
	case 0x7f:		// backspace
		c = '_';
		break;
	case 0x02:		// Ctrl-B
		c = 0x03;	// Fake out a ^C
		break;
	default:
		c = toupper(c);
		break;
	}

	return c | 0x80;	// set hi bit;
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
