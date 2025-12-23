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
#include "display_interface.h"
#include "terminal_keyboard.h"
#include "cpu_status.h"
#ifdef HAVE_SDL2
#include "sdl2_display.h"
#include "composite_keyboard.h"
#endif
#include "clock.h"

using Address = uint16_t;
using Byte    = uint8_t;

/////////
// Define which and where the ROM and other 'built-in' programs will be loaded. 

// WozMon (in ROM)
constexpr Word wozmonAddress = 0xff00;
static const char* WOZMON_FILE = BINFILE_PATH "/wozmon.bin";

// Apple Integer Basic (normally loaded from cassette)
constexpr Word apple1BasicAddress = 0xe000;
static const char* APPLESOFT_BASIC_FILE = BINFILE_PATH "/Apple-1_Integer_BASIC.bin";

// bytecode for the sample program from the Apple 1 Manual (normally entered 
// by hand via WozMon)
constexpr Word apple1SampleAddress = 0x0000;
std::vector<Byte> apple1SampleProg =
	{ 0xa9, 0x00, 		// lda #$00
	  0xaa,   			// tax
	  0x20, 0xef, 0xff, // jsr $ffef
	  0xe8, 			// inx
	  0x8a, 			// txa
	  0x4c, 0x02, 0x00 	// jmp $0002
	};

constexpr int clockSpeedMHz = 1;
constexpr Address PIA_BASE_ADDRESS = 0xd010;

// Create the memory, CPU and bus clock
Memory<Address, Byte> mem(MOS6502::LAST_ADDRESS);
MOS6502 cpu(mem);

#ifdef HAVE_SDL2
SDL2Display display;  // Handles graphical display and SDL window keyboard
TerminalKeyboard termKeyboard;  // Also accept input from terminal for control keys
CompositeKeyboard keyboard;  // Combines both keyboard sources
#else
TerminalDisplay display;
TerminalKeyboard keyboard;
#endif

std::shared_ptr<MOS6820<Address, Byte>> pia;

BusClock_t busClock(clockSpeedMHz);

// Status display flag
bool showStatus = false;

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
	mem.mapRAM(0x6000, 0x8fff);

	// Map RAM and load Apple 1 basic
	mem.mapRAM(0xe000, 0xefff);
	mem.loadDataFromFile(APPLESOFT_BASIC_FILE, apple1BasicAddress);

	// Load Apple 1 sample program and Apple-1 Basic 
	mem.loadData(apple1SampleProg, apple1SampleAddress);
}

int main() {
	// Initialize keyboard and PIA
#ifdef HAVE_SDL2
	keyboard.addKeyboard(&display);  // SDL window input (primary)
	keyboard.addKeyboard(&termKeyboard);  // Terminal input (for control keys)
	pia = std::make_shared<MOS6820<Address, Byte>>(&display, &keyboard);

	fmt::print("Apple 1 Emulator (6502) - SDL2 Graphics Mode\n");
	fmt::print("Close the window to quit\n");
	fmt::print("Control keys also work from terminal:\n");
	fmt::print("  Reset        = Control-\\\n");
	fmt::print("  Clear screen = Control-[\n");
	fmt::print("  Debugger     = Control-]\n");
	fmt::print("  Status       = Control-T\n");
	fmt::print("  Quit         = Control-Backspace\n");
#else
	pia = std::make_shared<MOS6820<Address, Byte>>(&display, &keyboard);

	fmt::print("A Very Simple Apple I (6502)\n");
	fmt::print("  Reset        = Control-\\\n");
	fmt::print("  Clear screen = Control-[\n");
	fmt::print("  Debugger     = Control-]\n");
	fmt::print("  Status       = Control-T\n");
	fmt::print("  Quit         = Control-Backspace\n");
#endif
	fmt::print("\n");

	setupMemoryMap();
	pia->setTermNonblocking();	
	busClock.enableTimingEmulation();

	// Order of operations:
	// - Execute one instruction, returning clock cycles that takes, then
	// - Execute the housekeeping functions on all devices, then
	// - Handle any control signals asserted by the devices, then 
	// - Delay however many clock cycles we've used, then
	// - Handle entering or exiting debug mode.

	cpu.Reset();	    // Exit the CPU from reset
	while (!cpu.isPCAtHaltAddress()) {
#ifdef HAVE_SDL2
		// Handle SDL2 events (window close, etc.)
		display.handleEvents();
		if (display.shouldQuit()) {
			fmt::print("\nWindow closed, exiting emulator\n");
			break;
		}
#endif

		// If we're in debug mode we have to toggle the terminal out of and in to non-blocking mode
		// so the CPU debugger (implemented in the CPU class) can access the terminal in non-blocking
		// mode.
		auto debug = cpu.isInDebugMode();
		if (debug)
			pia->setTermBlocking();

		cpu.execute();

		if (debug)
			pia->setTermNonblocking();

		auto signals = pia->housekeeping();

#ifdef HAVE_SDL2
		// Refresh SDL2 display after processing I/O
		display.refresh();
#endif

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
				cpu.setDebugMode(true);
				break;
			case Device::Status:
				showStatus = !showStatus;
				if (showStatus) {
					fmt::print("\nStatus display enabled (Ctrl+T to toggle off)\n");
				} else {
					fmt::print("\r\033[K\n");  // Clear status line
					fmt::print("Status display disabled\n");
				}
				break;
			case Device::Exit:
				fmt::print("\nExiting emulator\n");
				std::exit(0);
			}
		}

		// Print CPU status if enabled
		if (showStatus) {
			printCPUStatus(cpu);
		}

		busClock.delay(cpu.usedCycles());
	}

	pia->setTermNonblocking();	

	return 0;
}
