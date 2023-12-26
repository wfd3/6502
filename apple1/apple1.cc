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
#include "clock.h"

using Address = uint16_t;
using Cell    = uint8_t;

constexpr Address PIA_BASE_ADDRESS = 0xd010;

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
static const char* WOZMON_FILE = BINFILE_PATH "/wozmon.bin";

// Load address and data location for Apple Integer Basic (normally     
// loaded from cassette)
constexpr Address_t apple1BasicAddress = 0xe000;
static const char* APPLESOFT_BASIC_FILE = BINFILE_PATH "/Apple-1_Integer_BASIC.bin";

// bytecode for the sample program from the Apple 1 Manual (normally entered 
// by hand via WozMon)
constexpr Address_t apple1SampleAddress = 0x0000;
std::vector<Cell> apple1SampleProg =
	{ 0xa9, 0x00, 		// lda #$00
	  0xaa,   			// tax
	  0x20, 0xef, 0xff, // jsr $ffef
	  0xe8, 			// inx
	  0x8a, 			// txa
	  0x4c, 0x02, 0x00 	// jmp $0002
	};

// Setup memory map
void setupMemoryMap(){
	// 0x0000-0x5fff - RAM
	// 0xe000-0xefff - Apple 1 Basic (also RAM)
	// 0xd010-0xd013 - MOS6820
	// 0xff00-0xffff - WozMon ROM

	mem.Reset();

	// Map in the 6820/PIA, overwriting existing addresses.
	mem.mapDevice(pia, PIA_BASE_ADDRESS);

	// Map the Wozmon ROM into memory
	mem.loadRomFromFile(WOZMON_FILE, wozmonAddress);
 
	// 8K RAM
	mem.mapRAM(0x0000, 0x1fff);

	// Map RAM and load Apple 1 basic
	mem.mapRAM(0xe000, 0xefff);
	mem.loadDataFromFile(APPLESOFT_BASIC_FILE, apple1BasicAddress);

	// Load Apple 1 sample program and Apple-1 Basic 
	mem.loadData(apple1SampleProg, apple1SampleAddress);
}

int main() {
	fmt::print("A Very Simple Apple I\n");
	fmt::print("  Reset        = Control-\\\n");
	fmt::print("  Clear screen = Control-[\n");
	fmt::print("  Debugger     = Control-]\n");
	fmt::print("  Quit         = Control-Backspace\n");
	fmt::print("\n");

	setupMemoryMap();
	pia->changeTerminalState();	
	busClock.enableTimingEmulation();

	// Order of operations:
	// - Execute one instruction, returning clock cycles that takes, then
	// - Execute the housekeeping functions on all devices, then
	// - Handle any control signals asserted by the devices, then 
	// - Delay however many clock cycles we've used, then
	// - Handle entering or exiting debug mode.

	Cycles_t cyclesUsed;
	bool halt = false;
	bool debug = false;

	cpu.Reset();	    // Exit the CPU from reset
	while (!halt) {
		// If we're in debug mode we have to toggle the terminal out of and in to non-blocking mode
		// so the CPU debugger (implemented in the CPU class) can access the terminal in non-blocking 
		// mode.
		if (debug) {
			pia->setTermBlocking();
			debug = cpu.executeDebug();
			pia->setTermNonblocking();
		} else 
			cpu.execute(halt, debug, cyclesUsed);

		if (halt) 
			continue;
	
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
				debug = true;
				break;
			case Device::Exit:
				fmt::print("\nExiting emulator\n");
				std::exit(0);
			}
		}

		busClock.delay(cyclesUsed);
	}

	pia->setTermNonblocking();	

	return 0;
}
