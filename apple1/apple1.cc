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
#include "clock.h"

using Address = uint16_t;
using Cell = uint8_t;

// Create the memory, CPU, PIA and bus clock
Memory<Address, Cell> mem(CPU::MAX_MEM);
CPU cpu(mem);
auto pia = std::make_shared<MOS6820<Address, Cell>>();
BusClock_t busClock;

//////////
// This section defines which and where the ROM and other 'built-in' programs 
// will be loaded. 

// Load addresses and data locations for WozMon (in ROM)
constexpr Address_t wozmonAddress = 0xff00;
static const char* WOZMON_FILE =
BINFILE_PATH "/wozmon.bin";

// Load address and data location for Apple Integer Basic (normally 
// loaded from cassette)
constexpr Address_t appleBasicAddress = 0xe000;
static const char* APPLESOFT_BASIC_FILE =
BINFILE_PATH "/Apple-1_Integer_BASIC.bin";

// bytecode for the sample program from the Apple 1 Manual (normally entered 
// by hand via WozMon)
constexpr Address_t apple1SampleAddress = 0x0000;
std::vector<unsigned char> apple1SampleProg =
	{ 0xa9, 0x00, 0xaa, 0x20, 0xef,0xff, 0xe8, 0x8a, 0x4c, 0x02, 0x00 };

// Setup memory map
void setupMemMap(){
	mem.Reset();
	// Map 64k of RAM, making this one hefty Apple 1
	mem.mapRAM(0x0000, 0xffff);

	// Keyboard and display memory-mapped IO, overwriting existing
	// addresses.
	mem.mapDevice(pia);
	mem.mapROM(0xf000, 0xf0ff);

	// Map the Wozmon ROM into memory
	fmt::print("# Mapping wozmon ROM at {:04x}\n", wozmonAddress);
	mem.loadRomFromFile(WOZMON_FILE, wozmonAddress);
	
	// Load Apple 1 sample program and Apple-1 Basic 
	fmt::print("# Loading Apple I sample program at {:04x}\n", 
		apple1SampleAddress);
	mem.loadData(apple1SampleProg, apple1SampleAddress);

	fmt::print("# Loading Apple I Basic at {:04x}\n", appleBasicAddress);
	mem.loadDataFromFile(APPLESOFT_BASIC_FILE, appleBasicAddress);
}

//////////
// Let's pretend to be an Apple1
int main() {
	Cycles_t cyclesUsed;

	fmt::print("A Very Simple Apple I\n");
	fmt::print("  Reset        = Control-\\\n");
	fmt::print("  Clear screen = Control-^\n");
	fmt::print("  Debugger     = Control-]\n");
	fmt::print("  Quit         = Control-minus\n");
	fmt::print("\n");

	setupMemMap();
	MOS6820<Address, Cell>::setup();	
	busClock.enableTimingEmulation();

	// When the emulator enters debug mode we need to reset the
	// display so that keyboard entry works in blocking mode.
	cpu.setDebugEntryExitFunc(&MOS6820<Address, Cell>::teardown, &MOS6820<Address, Cell>::setup);
	cpu.Reset();	    // Exit the CPU from reset

	// Order of operations:
	// - Execute one instruction, returning clock cycles that takes, then
	// - Execute the housekeeping functions on all devices, then
	// - Handle any control signals asserted by the devices, then 
	// - Delay however many clock cycles we've used.
	while (1) {
		cpu.executeOneInstruction(cyclesUsed);
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
		busClock.delay(cyclesUsed);
	}

	MOS6820<Address, Cell>::teardown();	// Set the keyboard blocking

	return 0;
}
