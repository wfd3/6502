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
#include <fmt/core.h>

#include <6502.h>
#include <memory.h>

#include "apple1.h"

// Keyboard input 'queue'
bool kbdCharPending = false;
char kbdCharacter = 0;

// Create the memory and CPU
Memory mem(CPU::MAX_MEM);
CPU cpu(mem);

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
		if (isprint(c) || isspace(c))
			fmt::print("{:c}", c);
	}
}

unsigned char dspread() {
	return 0x7f;
}

// Check if characters are pending, return key code if so
unsigned char kbdcr_read() {

	// Important to not look into the STDIN stream until the last
	// character read has been delivered to the running program.
	if (kbdCharPending)
		return kbdCharacter;

	if (getch(kbdCharacter) == false) 
		return 0;
	
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

	kbdCharacter |= (char)0x80;
	kbdCharPending = true;

	return kbdCharacter;
}

// Read characters from the keyboard
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
int main() {
	setupSignals();

	// Map RAM, making this one hefty Apple 1
	mem.mapRAM(0x0000, 0xffff);

	// Keyboard and display memory-mapped IO, overwriting existing
	// addresses if needed.
	mem.mapMIO(KEYBOARD, kbdread, nullptr, true);
	mem.mapMIO(KEYBOARDCR, kbdcr_read, nullptr, true);
	mem.mapMIO(DISPLAY, dspread, dspwrite, true);
	mem.mapMIO(DISPLAYCR, nullptr, nullptr, true);

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

	fmt::print("# Note: {}\n", commandKeyBanner);
	fmt::print("#       ^B is ^C\n"); // todo
	fmt::print("\n");

	// When the emulator enters debug mode we need to reset the
	// display so that keyboard entry works in blocking mode.
	cpu.setDebugEntryExitFunc(disable_raw_mode, enable_raw_mode);

	enable_raw_mode();	// Set the keyboard non-blocking

	cpu.Cycles.enableTimingEmulation();
	cpu.exitReset();	// Exit the CPU from reset
	cpu.execute();		// Start the CPU running

	disable_raw_mode();	// Set the keyboard blocking

	return 0;
}
