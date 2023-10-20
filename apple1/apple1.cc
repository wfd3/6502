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

#include "mos6820.h"
#include "apple1.h"


using Address = uint16_t;
using Cell = uint8_t;

// Create the memory and CPU
Memory<Address, Cell> mem(CPU::MAX_MEM);

CPU cpu(mem);
auto pia = std::make_shared<MOS6820<Address, Cell>>();

//////////
// This section defines which and where the 'built-in' programs 
// will be loaded. 

// Uncomment to load Apple INTEGER BASIC
#define APPLE_INTEGER_BASIC

//Uncomment to load Applesoft Basic Lite
//#define APPLESOFT_BASIC_LITE

#if defined(APPLE_INTEGER_BASIC) && defined(APPLESOFT_BASIC_LITE)
# error "Can't have both Apple Integer Basic and Applesoft Basic loaded "\
	"at the same time"
#endif

// Load addresses and data locations for various built in programs
constexpr Address_t wozmonAddress = 0xff00;
static const char* WOZMON_FILE =
BINFILE_PATH "/wozmon.bin";

#ifdef APPLE_INTEGER_BASIC
constexpr Address_t appleBasicAddress = 0xe000;
static const char* APPLESOFT_BASIC_FILE =
BINFILE_PATH "/Apple-1_Integer_BASIC.bin";
#endif

#ifdef APPLESOFT_BASIC_LITE
constexpr Address_t applesoftBasicLiteAddress = 0x6000;
static const char* APPLE_INTEGER_BASIC_FILE =
BINFILE_PATH "/applesoft-lite-0.4-ram.bin";
#endif

// bytecode for the sample program from the Apple 1 Manual
constexpr Address_t apple1SampleAddress = 0x0000;
std::vector<unsigned char> apple1SampleProg =
	{ 0xa9, 0x00, 0xaa, 0x20, 0xef,0xff, 0xe8, 0x8a, 0x4c, 0x02, 0x00 };

//////////
// Let's pretend to be an Apple1
int main() {

	// Map 64k of RAM, making this one hefty Apple 1
	mem.mapRAM(0x0000, 0xffff);

	// Keyboard and display memory-mapped IO, overwriting existing
	// addresses.
	mem.mapDevice(pia);

	fmt::print("A Very Simple Apple I\n");
	fmt::print("  Reset        = Control-\\\n");
	fmt::print("  Clear screen = Control-^\n");
	fmt::print("  Debugger     = Control-]\n");
	fmt::print("  Quit         = Control-minus\n");
	fmt::print("\n");

	// Load Wozmon, Apple Basic or Applesoft Basic Lite and the
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
	cpu.setDebugEntryExitFunc(&MOS6820<Address, Cell>::teardown, &MOS6820<Address, Cell>::setup);

	MOS6820<Address, Cell>::setup();	// Set the keyboard non-blocking

	cpu.Cycles.enableTimingEmulation();
	cpu.Reset();	    // Exit the CPU from reset

	// - Execute one instruction, consume however may clock cycles that takes
	// - Execute the housekeeping functions on all devices
	// - If any device has asserted any control signals, handle them.
	while (1) {
		cpu.executeOne();
		auto signals = pia->housekeeping();
		for (const auto& signal : signals) {
			switch(signal) {
			case Device::None:
				break;
			case Device::Reset:
				cpu.Reset();
				if (cpu.inReset()) 
					cpu.Reset();
				break;
			case Device::Debug:
				cpu.setDebug(true);
				break;
			case Device::Exit:
				fmt::print("\nExiting emulator\n");
				std::exit(0);
			}
		}
	}

	MOS6820<Address, Cell>::teardown();	// Set the keyboard blocking

	return 0;
}
