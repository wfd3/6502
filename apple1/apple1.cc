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
#include <queue>
#include <fmt/core.h>

#include <6502.h>
#include <memory.h>

#include "apple1.h"

// Keyboard input 'queue'
bool kbdCRRead = false;
std::queue<char> queue;

// Create the memory and CPU
Memory mem(CPU::MAX_MEM);
CPU cpu(mem);

//////////
// Rough emulation of the MOS6820 Peripheral Interface Adapter

unsigned char dspwrite_data = 0;
bool dspwrite_haveData = false;

// Write a character to the display
void dspwrite(unsigned char c) {
	dspwrite_data = c;
	dspwrite_haveData = true;
}

void dspwrite_loop() {
	if (!dspwrite_haveData) 
		return;

	unsigned char c = dspwrite_data & 0x7f;		// clear hi bit

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
		if (isprint(c) || isspace(c))
			fmt::print("{:c}", c);
	}

	dspwrite_haveData = false;
}

unsigned char dspread() {
	return 0x7f;
}

// This is called after every instruction is processed.  Enqueue
// any keypresses into queue.  Memory-mapped IO will call functions
// to dequeue this data.
void kbd_loop() {
	char ch;	
	if (getch(ch) == false) 
		return;
	
	// Map modern ascii to Apple 1 keycodes
	switch (ch) {

	// Control characters
	case CTRL_RBRACKET:
		fmt::print("\n");
		cpu.Reset();
		if (cpu.inReset())
			cpu.Reset();	
		return;
	
	case CTRL_LBRACKET:
		cpu.setDebug(true);
		return;
	
	case CTRL_MINUS:
		fmt::print("\nExiting emulator\n");
		exit(0);

	// Regular characters
	case '\n':
		ch = CR;
		break;
	
	case DEL:
		ch = '_';
		break;
	
	default:
		ch = std::toupper(ch);
		break;
	}

	ch |= 0x80;
	queue.push(ch);
}

// Check if characters are pending, return key code if so
unsigned char kbdcr_read() {
	kbdCRRead = true;
	if (queue.empty())
		return 0;

	return queue.front();
}

// Read characters from the keyboard
unsigned char kbdread() {
	if (queue.empty())
		return 0;

	auto ch = queue.front();

	// Applesoft Basic Lite does a blind, unchecked read on the keyboard port
	// looking for a ^C.  If it sees one, it then does a read on the keyboard
	// control register, followed by a read of the keyboard port, expecting to
	// get the same ^C.  This logic forces a keyboard control register read 
	// before removing the character from the queue, thus preventing an 
	// infinite loop.
	if (kbdCRRead) {
		queue.pop();	
		kbdCRRead = false;
	}

	return ch;
}	

// Let's pretend to be an Apple1
int main() {
//	setupSignals();

	// Map RAM, making this one hefty Apple 1
	mem.mapRAM(0x0000, 0xffff);

	// Keyboard and display memory-mapped IO, overwriting existing
	// addresses if needed.
	mem.mapMIO(KEYBOARD, kbdread, nullptr);
	mem.mapMIO(KEYBOARDCR, kbdcr_read, nullptr);
	mem.mapMIO(DISPLAY, dspread, dspwrite);
	mem.mapMIO(DISPLAYCR, nullptr, nullptr);

	fmt::print("A Very Simple Apple I\n");
	fmt::print("Reset is Control-]\n");
	fmt::print("Debugger is Control-[\n");
	fmt::print("Quit is Control-minus\n");
	fmt::print("\n");

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

	// When the emulator enters debug mode we need to reset the
	// display so that keyboard entry works in blocking mode.
	cpu.setDebugEntryExitFunc(disable_raw_mode, enable_raw_mode);

	enable_raw_mode();	// Set the keyboard non-blocking

	cpu.Cycles.enableTimingEmulation();
	cpu.Reset();	    // Exit the CPU from reset
//	cpu.execute();		// Start the CPU running

	while (1) {
		cpu.executeOne();
		dspwrite_loop();
		kbd_loop();

	}

	disable_raw_mode();	// Set the keyboard blocking

	return 0;
}
