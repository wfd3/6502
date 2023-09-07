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

#include <fmt/core.h>

#include <6502.h>
#include <memory.h>

// Uncomment to load Apple INTEGER BASIC
//#define APPLE_INTEGER_BASIC

//Uncomment to load Applesoft Basic Lite
#define APPLESOFT_BASIC_LITE

#if defined(APPLE_INTEGER_BASIC) && defined(APPLESOFT_BASIC_LITE)
# error "Can't have both Apple Integer Basic and Applesoft Basic loaded "\
	"at the same time"
#endif

// Load addresses and data locations for various built in programs
constexpr Address_t wozmonAddress              = 0xff00;
static const char *WOZMON_FILE =
	BINFILE_PATH "/wozmon.bin";

#ifdef APPLE_INTEGER_BASIC
constexpr Address_t appleBasicAddress          = 0xe000;
static const char *APPLESOFT_BASIC_FILE =
	BINFILE_PATH "Apple-1_Integer_BASIC.bin";
#endif

#ifdef APPLESOFT_BASIC_LITE
constexpr Address_t applesoftBasicLiteAddress  = 0x6000;
static const char *APPLE_INTEGER_BASIC_FILE =
	BINFILE_PATH "/applesoft-lite-0.4-ram.bin";
#endif

// bytecode for the sample program from the Apple 1 Manual
constexpr Address_t apple1SampleAddress        = 0x0000;
std::vector<unsigned char> apple1SampleProg =
             {0xa9, 0x00, 0xaa, 0x20, 0xef,0xff, 0xe8, 0x8a, 0x4c, 0x02, 0x00};

// Memory-mapped IO address for Keyboard and display
constexpr Word KEYBOARD   = 0xd010;
constexpr Word KEYBOARDCR = 0xd011;
constexpr Word DISPLAY    = 0xd012;
constexpr Word DISPLAYCR  = 0xd013;

// Apple 1 keycodes
constexpr char CR   = 0x0d;
constexpr char BELL = 0x0a;
constexpr char DEL  = 0x7f;
constexpr char CTRLA= 0x01;
constexpr char CTRLB= 0x02;
constexpr char CTRLC= 0x03;
constexpr char CTRLD= 0x04;

// Keyboard input 'queue'
bool kbdCharPending = false;
char kbdCharacter = 0;

// Create the memory and CPU
Memory mem(CPU::MAX_MEM);
CPU cpu(mem);

void signalHandler( int signum ) {
	switch (signum) {
	case SIGQUIT:		// Reset computer
		fmt::print("\n");
		cpu.setPendingReset();
		break;

	case SIGUSR1:		// Toggle the 6502 emulator's debugger
		cpu.setDebug(true);
		break;

	case SIGINT:
		fmt::print("\nExiting emulator\n");
		exit(0);
	}
}

// Make the terminal non-blocking
void enable_raw_mode()
{
    termios term;
    fflush(stdout);
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    tcflush(0, TCIFLUSH); 
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

//////////
// Rough emulation of the MOS6820 Peripheral Interface Adapter

// Write a character to the display
void dspwrite(unsigned char c) {
	c &= 0x7f;		// clear hi bit
	switch (c) {
	case CR:	// \r
		fmt::print("\n");
		break;
	case '_':		// Backspace
		fmt::print("\b");
		break;
	case BELL:		// Bell
		fmt::print("\a");
		break;
	default:
		if (isprint(c)|| isspace(c))
			fmt::print("{:c}", c);
	}
}

unsigned char dspread() {
	return 0x7f;
}

// Read characters from the keyboard
unsigned char kbdcr_read() {
	int byteswaiting;

	// Important to not look into the STDIN stream until the last
	// character read has been delivered to the running program.
	if (kbdCharPending)
		return kbdCharacter;

	ioctl(STDIN_FILENO, FIONREAD, &byteswaiting);
	if (byteswaiting == 0)
		return 0;

	read(STDIN_FILENO, &kbdCharacter, 1);

	// Map modern ascii to Apple 1 keycodes
	switch (kbdCharacter) {
	case '\n':
		kbdCharacter = CR;
		break;
 	case DEL:		     
		kbdCharacter = '_';
		break;
	case CTRLB:		     
		kbdCharacter = CTRLC; // Fake out a ^C
		break;
	case CTRLA:		     
		fmt::print("\n");
		cpu.setPendingReset();
		return 0;
	case CTRLD:		     
		cpu.setDebug(true);
		return 0;
	default:
		kbdCharacter =
			static_cast<char>(std::toupper(static_cast<unsigned char>(kbdCharacter)));
		//		std::toupper(kbdCharacter);
		break;
	}

	// The keycode value will get LDA'ed into A, setting the
	// processor status bits in the process.  Apple 1 expect that
	// the Negative Flag will be set if there are characters
	// pending read from the keyboard, hence the bitwise or with
	// 0x80.
	
	kbdCharacter |= (char) 0x80;
	kbdCharPending = true;
		
	return kbdCharacter;
}

unsigned char kbdread() {
	if (!kbdCharPending) {
		kbdcr_read();
		// If there's a pending character here, return it but do not
		// toggle the kbdCharPending flag.
		//
		// Applesoft Basic Lite does a blind, unchecked read
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

// Let's pretend to be an Apple1
int main () {
	// register signal SIGINT and signal handler  
	signal(SIGQUIT, signalHandler); // Reset
	signal(SIGUSR1, signalHandler); // force into debugger
	signal(SIGINT,  signalHandler); // Quit

	// Map RAM, making this one hefty Apple 1
	mem.mapRAM(0x0000, 0xffff);

	// Keyboard and display memory-mapped IO, overwriting existing
	// addresses if needed.
	mem.mapMIO(KEYBOARD,   kbdread, NULL, true);
	mem.mapMIO(KEYBOARDCR, kbdcr_read, NULL, true);
	mem.mapMIO(DISPLAY,    dspread, dspwrite, true );
	mem.mapMIO(DISPLAYCR,  NULL, NULL, true);

	fmt::print("A Very Simple Apple I\n");
	// Load Wozmon, Apple Basic, Applesoft Basic Lite and the
	// Apple 1 sample program
	fmt::print("# Loading Apple I sample program at {:04x}\n",
		   apple1SampleAddress);
	mem.loadData(apple1SampleProg, apple1SampleAddress);

	fmt::print("# Loading wozmon at {:04x}\n", wozmonAddress);
	mem.loadDataFromFile(WOZMON_FILE, wozmonAddress);

#ifdef APPLE_INTEGER_BASIC
	fmt::print("# Loading Apple Integer Basic at {:04x}\n",
		  appleBasicAddress);
	mem.loadDataFromFile(APPLESOFT_BASIC_FILE, appleBasicAddress);
#endif

#ifdef APPLESOFT_BASIC_LITE
	fmt::print("# Loading Applesoft Basic Lite at {:04x}\n",
	       applesoftBasicLiteAddress);
	mem.loadDataFromFile(APPLE_INTEGER_BASIC_FILE,
			     applesoftBasicLiteAddress);
#endif

	fmt::print("# Note: ^A is Reset, ^D is Debugger\n");
	fmt::print("#       ^B is ^C\n"); // todo
	fmt::print("\n");

	// When the emulator enters debug mode we need to reset the
	// display so that keyboard entry works in blocking mode.
	cpu.setDebugEntryExitFunc(disable_raw_mode, enable_raw_mode);

	enable_raw_mode();	// Set the keyboard non-blocking

	cpu.exitReset();	// Exit the CPU from reset
	cpu.execute();		// Start the CPU running

	disable_raw_mode();	// Set the keyboard blocking

	return 0;
}
